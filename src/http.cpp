#include "http.h"
#include <windows.h>
#include <winhttp.h>

#include <string>

static std::wstring toWide(const std::string& s) {
    if (s.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], len);
    return w;
}

std::string httpGet(const std::string& host, int port, const std::string& path) {
    std::string result;
    HINTERNET session = WinHttpOpen(L"ChassisDcoder/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return result;

    std::wstring whost = toWide(host);
    HINTERNET connect = WinHttpConnect(session, whost.c_str(), (INTERNET_PORT)port, 0);
    if (connect) {
        std::wstring wpath = toWide(path);
        DWORD flags = (port == INTERNET_DEFAULT_HTTPS_PORT) ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET request = WinHttpOpenRequest(connect, L"GET", wpath.c_str(),
            nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (request) {
            if (WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                    WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
                if (WinHttpReceiveResponse(request, nullptr)) {
                    char buffer[8192];
                    DWORD read = 0;
                    while (WinHttpReadData(request, buffer, sizeof(buffer), &read) && read > 0) {
                        result.append(buffer, read);
                    }
                }
            }
            WinHttpCloseHandle(request);
        }
        WinHttpCloseHandle(connect);
    }
    WinHttpCloseHandle(session);
    return result;
}
