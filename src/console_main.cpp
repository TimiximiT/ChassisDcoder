#include "report.h"
#include "vin.h"

#include <windows.h>

#include <algorithm>
#include <iostream>
#include <string>

namespace {
void setConsoleUtf8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

bool askYesNo(const std::string& prompt, bool defaultValue) {
    using namespace std;
    cout << prompt << (defaultValue ? " [Y/n]: " : " [y/N]: ");
    string answer;
    if (!getline(cin, answer)) return defaultValue;
    for (char& c : answer) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    if (answer.empty()) return defaultValue;
    return answer == "y" || answer == "yes";
}
} // namespace

int main(int argc, char** argv) {
    using namespace std;

    if (argc > 1 && (string(argv[1]) == "-h" || string(argv[1]) == "--help")) {
        cout << "ChassisDcoder console VIN decoder\n\n";
        cout << "Usage:\n";
        cout << "  chassisdcoder_console.exe <VIN> [--full]\n";
        cout << "  chassisdcoder_console.exe [--full]\n\n";
        cout << "If the VIN is omitted, the program prompts for it.\n";
        cout << "--full prints every non-blank field returned by NHTSA.\n";
        return 0;
    }

    setConsoleUtf8();

    bool fullDetails = false;
    std::string vin;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--full" || arg == "-f") {
            fullDetails = true;
        } else if (arg.empty() || arg[0] != '-') {
            if (!vin.empty()) {
                std::cerr << "Multiple VIN arguments provided.\n";
                return 1;
            }
            vin = arg;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n\n";
            std::cerr << "Usage:\n";
            std::cerr << "  chassisdcoder_console.exe <VIN> [--full]\n";
            return 1;
        }
    }

    if (vin.empty()) {
        std::cout << "Enter VIN: ";
        if (!std::getline(std::cin, vin)) {
            std::cerr << "Failed to read VIN.\n";
            return 1;
        }
        fullDetails = askYesNo("Show full NHTSA details?", false);
    }

    vin = normalizeVin(vin);
    if (!isValidVin(vin)) {
        cerr << "Invalid VIN. VINs must be 17 characters (no I, O, or Q).\n";
        return 1;
    }

    string decodeError;
    VehicleInfo info = decodeVin(vin, decodeError);
    if (!decodeError.empty() &&
        info.year.empty() && info.make.empty() && info.model.empty()) {
        cerr << "Decode failed: " << decodeError << "\n";
        return 1;
    }

    std::string summary;
    std::string reportFullDetails;
    buildVinReport(info, vin, decodeError, summary, reportFullDetails);
    cout << summary;

    if (fullDetails) {
        cout << reportFullDetails;
    }

    if (!decodeError.empty()) {
        cout << "NHTSA warning: " << decodeError << "\n";
    }

    return 0;
}
