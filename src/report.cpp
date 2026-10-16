#include "report.h"
#include "chassis.h"

#include <sstream>
#include <string>


void buildVinReport(const VehicleInfo& info,
                    const std::string& requestedVin,
                    const std::string& decodeError,
                    std::string& summary,
                    std::string& fullDetails) {
    std::ostringstream out;
    std::ostringstream details;

    if (!decodeError.empty()) {
        out << "Note: " << decodeError << "\r\n\r\n";
    }

    out << "VIN:   " << (info.vin.empty() ? requestedVin : info.vin) << "\r\n";
    out << "Year:  " << (info.year.empty() ? "(unknown)" : info.year) << "\r\n";
    out << "Make:  " << (info.make.empty() ? "(unknown)" : info.make) << "\r\n";
    out << "Model: " << (info.model.empty() ? "(unknown)" : info.model) << "\r\n";
    std::string trim = safeField(info, "Trim");
    if (!trim.empty()) {
        out << "Trim:  " << trim << "\r\n";
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
    if (!decodeError.empty()) {
        details << "Note: " << decodeError << "\r\n\r\n";
    }
    details << "VIN:   " << (info.vin.empty() ? requestedVin : info.vin) << "\r\n";
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
