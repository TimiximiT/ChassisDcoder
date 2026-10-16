#include "chassis.h"
#include "vin.h"

#include <windows.h>

#include <memory>
#include <sstream>
#include <string>

namespace {
enum {
    ID_VIN_EDIT = 100,
    ID_DECODE_BUTTON = 101,
    ID_RESULT_EDIT = 102,
    ID_FULLDETAILS_BUTTON = 103,
    WM_DECODE_DONE = WM_APP + 1
};

HWND g_main = nullptr;
HWND g_vinEdit = nullptr;
HWND g_decodeButton = nullptr;
HWND g_resultEdit = nullptr;
HWND g_fullDetailsButton = nullptr;
std::string g_lastSummary;
std::string g_lastFullDetails;
bool g_showingFullDetails = false;
HFONT g_font = nullptr;

HANDLE g_thread = nullptr;
bool g_decoding = false;

std::wstring utf8ToWide(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()), nullptr, 0);
    if (n <= 0) return {};
    std::wstring w(static_cast<size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()), &w[0], n);
    return w;
}

std::string wideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
    if (n <= 0) return {};
    std::string s(static_cast<size_t>(n), '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), &s[0], n, nullptr, nullptr);
    return s;
}

struct DecodeTask {
    std::string vin;
    HWND hwnd;
};

void buildResults(const std::string& vin, std::string& summary, std::string& fullDetails) {
    std::ostringstream out;
    std::ostringstream details;
    std::string error;
    VehicleInfo info = decodeVin(vin, error);

    if (!error.empty()) {
        out << "Note: " << error << "\r\n\r\n";
    }

    out << "VIN:   " << (info.vin.empty() ? vin : info.vin) << "\r\n";
    out << "Year:  " << (info.year.empty() ? "(unknown)" : info.year) << "\r\n";
    out << "Make:  " << (info.make.empty() ? "(unknown)" : info.make) << "\r\n";
    out << "Model: " << (info.model.empty() ? "(unknown)" : info.model) << "\r\n";
    auto trimIt = info.fields.find("Trim");
    if (trimIt != info.fields.end() && !trimIt->second.empty()) {
        out << "Trim:  " << trimIt->second << "\r\n";
    }
    out << "\r\n";

    out << "Chassis Code:\r\n";
    if (!makeHasChassisHeuristics(info.make)) {
        out << "  No chassis-code heuristic for make '" << info.make << "'.\r\n";
    } else {
        auto guesses = decodeChassis(info.make, info.model, info.year);
        if (guesses.empty()) {
            out << "  No year/model-specific guess available. Verify physically.\r\n";
        } else {
            for (const auto& g : guesses) {
                out << "  Chassis Code: " << g.code
                    << "  (" << g.note << ") [" << g.confidence << "]\r\n";
            }
            out << "  These are estimates; confirm against the vehicle/Autel database.\r\n";
        }
    }
    out << "\r\n";

    auto adas = adasItems(info);
    out << "ADAS / Safety features (NHTSA data present):\r\n";
    if (adas.empty()) {
        out << "  (none reported)\r\n";
    } else {
        for (const auto& kv : adas) {
            out << "  " << kv.first << ": " << kv.second << "\r\n";
        }
    }
    out << "\r\n";

    out << "Note: The manufacturer may not report all safety features to NHTSA.\r\n";
    out << "A blank NHTSA value does not prove a feature is absent.\r\n\r\n";
    summary = out.str();

    details << "Full decoded vehicle details\r\n";
    details << "============================\r\n\r\n";
    if (!error.empty()) {
        details << "Note: " << error << "\r\n\r\n";
    }
    details << "VIN:   " << (info.vin.empty() ? vin : info.vin) << "\r\n";
    details << "Year:  " << (info.year.empty() ? "(unknown)" : info.year) << "\r\n";
    details << "Make:  " << (info.make.empty() ? "(unknown)" : info.make) << "\r\n";
    details << "Model: " << (info.model.empty() ? "(unknown)" : info.model) << "\r\n\r\n";
    details << "NHTSA fields with data:\r\n";
    if (info.fields.empty()) {
        details << "  (none)\r\n";
    } else {
        bool anyDetail = false;
        for (const auto& kv : info.fields) {
            if (kv.second.empty()) continue;
            anyDetail = true;
            details << "  " << kv.first << ": " << kv.second << "\r\n";
        }
        if (!anyDetail) {
            details << "  (no non-blank fields returned)\r\n";
        }
    }
    details << "\r\nChassis-code attempt (Autel-style, heuristic):\r\n";
    if (!makeHasChassisHeuristics(info.make)) {
        details << "  No chassis-code heuristic for make '" << info.make << "'.\r\n";
    } else {
        auto guesses = decodeChassis(info.make, info.model, info.year);
        if (guesses.empty()) {
            details << "  No year/model-specific guess available. Verify physically.\r\n";
        } else {
            for (const auto& g : guesses) {
                details << "  " << g.code << " - " << g.note
                        << " [" << g.confidence << "]\r\n";
            }
        }
    }
    fullDetails = details.str();
}

DWORD WINAPI DecodeProc(LPVOID param) {
    std::unique_ptr<DecodeTask> task(static_cast<DecodeTask*>(param));
    std::string result;
    std::string fullDetails;
    buildResults(task->vin, result, fullDetails);
    g_lastSummary = result;
    g_lastFullDetails = fullDetails;
    g_showingFullDetails = false;
    std::string* payload = new std::string(std::move(result));
    if (!PostMessage(task->hwnd, WM_DECODE_DONE, 0, reinterpret_cast<LPARAM>(payload))) {
        delete payload;
    }
    return 0;
}

void SetResultText(const std::wstring& text) {
    SetWindowTextW(g_resultEdit, text.c_str());
}

void StartDecode() {
    if (g_decoding) return;

    wchar_t buf[256] = {0};
    GetWindowTextW(g_vinEdit, buf, 256);
    std::string vin = normalizeVin(wideToUtf8(buf));

    if (!isValidVin(vin)) {
        SetResultText(L"Invalid VIN. VINs must be 17 characters (no I, O, or Q).");
        return;
    }

    g_decoding = true;
    EnableWindow(g_decodeButton, FALSE);
    SetWindowTextW(g_decodeButton, L"Decoding...");
    SetResultText(L"Please wait, contacting NHTSA...");
    ShowWindow(g_fullDetailsButton, SW_HIDE);
    g_showingFullDetails = false;

    DecodeTask* task = new DecodeTask{vin, g_main};
    g_thread = CreateThread(nullptr, 0, DecodeProc, task, 0, nullptr);
    if (!g_thread) {
        delete task;
        g_decoding = false;
        EnableWindow(g_decodeButton, TRUE);
        SetWindowTextW(g_decodeButton, L"Decode");
        SetResultText(L"Failed to start decode thread.");
    }
}
} // namespace
// END_PART1
namespace {
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_font = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        if (!g_font) {
            g_font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        }

        CreateWindowW(L"STATIC", L"VIN:", WS_CHILD | WS_VISIBLE,
            10, 14, 40, 20, hwnd, nullptr, nullptr, nullptr);
        g_vinEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
            70, 16, 540, 28, hwnd, reinterpret_cast<HMENU>(ID_VIN_EDIT), nullptr, nullptr);
        g_decodeButton = CreateWindowW(L"BUTTON", L"Decode",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            620, 16, 110, 28, hwnd, reinterpret_cast<HMENU>(ID_DECODE_BUTTON), nullptr, nullptr);
        g_resultEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
            24, 56, 704, 500, hwnd, reinterpret_cast<HMENU>(ID_RESULT_EDIT), nullptr, nullptr);
        g_fullDetailsButton = CreateWindowW(L"BUTTON", L"Full Details",
            WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
            24, 566, 150, 28, hwnd, reinterpret_cast<HMENU>(ID_FULLDETAILS_BUTTON), nullptr, nullptr);

        if (g_font) {
            SendMessageW(g_vinEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_font), TRUE);
            SendMessageW(g_decodeButton, WM_SETFONT, reinterpret_cast<WPARAM>(g_font), TRUE);
            SendMessageW(g_resultEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_font), TRUE);
            SendMessageW(g_fullDetailsButton, WM_SETFONT, reinterpret_cast<WPARAM>(g_font), TRUE);
        }

        SetWindowTextW(g_resultEdit, L"Enter a 17-character VIN and click Decode.");
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_DECODE_BUTTON && HIWORD(wParam) == BN_CLICKED) {
            StartDecode();
            return 0;
        }
        if (LOWORD(wParam) == ID_FULLDETAILS_BUTTON && HIWORD(wParam) == BN_CLICKED) {
            if (g_showingFullDetails) {
                SetWindowTextW(g_resultEdit, utf8ToWide(g_lastSummary).c_str());
                SetWindowTextW(g_fullDetailsButton, L"Full Details");
                g_showingFullDetails = false;
            } else {
                SetWindowTextW(g_resultEdit, utf8ToWide(g_lastFullDetails).c_str());
                SetWindowTextW(g_fullDetailsButton, L"Summary");
                g_showingFullDetails = true;
            }
            return 0;
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_RETURN && GetFocus() == g_vinEdit) {
            StartDecode();
            return 0;
        }
        break;

    case WM_DECODE_DONE: {
        auto payload = reinterpret_cast<std::string*>(lParam);
        if (payload) {
            SetWindowTextW(g_resultEdit, utf8ToWide(*payload).c_str());
            delete payload;
        } else {
            SetWindowTextW(g_resultEdit, utf8ToWide(g_lastSummary).c_str());
        }
        g_decoding = false;
        EnableWindow(g_decodeButton, TRUE);
        SetWindowTextW(g_decodeButton, L"Decode");
        ShowWindow(g_fullDetailsButton, SW_SHOW);
        EnableWindow(g_fullDetailsButton, TRUE);
        SetWindowTextW(g_fullDetailsButton, L"Full Details");
        g_showingFullDetails = false;
        if (g_thread) {
            CloseHandle(g_thread);
            g_thread = nullptr;
        }
        return 0;
    }

    case WM_CLOSE:
        if (g_decoding) {
            MessageBoxW(hwnd, L"Please wait for decoding to finish.", L"Busy", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        if (g_thread) {
            WaitForSingleObject(g_thread, 10000);
            CloseHandle(g_thread);
            g_thread = nullptr;
        }
        if (g_font) {
            DeleteObject(g_font);
            g_font = nullptr;
        }
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}
} // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"ChassisDcoderWindow";
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    g_main = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"ChassisDcoder - VIN Decoder",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        760, 680,
        nullptr, nullptr, instance, nullptr);

    if (!g_main) {
        MessageBoxW(nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(g_main, showCommand);
    UpdateWindow(g_main);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (IsDialogMessage(g_main, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

