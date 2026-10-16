#include "vin.h"
#include "http.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <regex>
#include <string>
#include <vector>

bool isValidVin(const std::string& vin) {
    if (vin.size() != 17) return false;
    for (char c : vin) {
        if (!std::isalnum(static_cast<unsigned char>(c))) return false;
        char u = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (u == 'I' || u == 'O' || u == 'Q') return false;
    }
    return true;
}

std::string normalizeVin(const std::string& vin) {
    std::string out = vin;
    for (char& c : out) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return out;
}

static std::string unescapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char nx = s[i + 1];
            if (nx == '"') { out += '"'; ++i; }
            else if (nx == '\\') { out += '\\'; ++i; }
            else if (nx == 'n') { out += '\n'; ++i; }
            else if (nx == 'r') { out += '\r'; ++i; }
            else if (nx == 't') { out += '\t'; ++i; }
            else { out += s[i]; }
        } else {
            out += s[i];
        }
    }
    return out;
}

static std::map<std::string, std::string> parseFields(const std::string& body) {
    std::map<std::string, std::string> fields;
    std::regex re("\"([A-Za-z0-9_]+)\"\\s*:\\s*\"((?:\\\\.|[^\"\\\\])*)\"");
    for (std::sregex_iterator it(body.begin(), body.end(), re), end; it != end; ++it) {

        std::string key = (*it)[1].str();
        std::string val = unescapeJson((*it)[2].str());
        // trim
        auto trim = [](std::string& x) {
            while (!x.empty() && (x.front() == ' ' || x.front() == '\t' || x.front() == '\r' || x.front() == '\n')) x.erase(x.begin());
            while (!x.empty() && (x.back() == ' ' || x.back() == '\t' || x.back() == '\r' || x.back() == '\n')) x.pop_back();
        };
        trim(val);
        fields[key] = val;
    }
    return fields;
}

VehicleInfo decodeVin(const std::string& vin, std::string& errorMessage) {
    VehicleInfo info;
    info.vin = vin;
    errorMessage.clear();

    std::string path = "/api/vehicles/DecodeVinValues/" + vin + "?format=json";
    std::string body = httpGet("vpic.nhtsa.dot.gov", 443, path);
    if (body.empty()) {
        errorMessage = "HTTP request failed or returned empty response.";
        return info;
    }

    info.fields = parseFields(body);
    if (info.fields.empty()) {
        errorMessage = "Could not parse vehicle data from NHTSA response.";
        return info;
    }

    auto get = [&](const char* key) -> std::string {
        auto it = info.fields.find(key);
        return (it != info.fields.end()) ? it->second : std::string();
    };

    info.year = get("ModelYear");
    info.make = get("Make");
    info.model = get("Model");

    if (info.make.empty() && info.model.empty() && info.year.empty()) {
        errorMessage = "NHTSA did not return year/make/model for this VIN.";
    }
    return info;
}

namespace {
struct AdasKey {
    const char* key;
    const char* label;
};

const AdasKey kAdasKeys[] = {
    {"ABS", "ABS (Anti-lock Braking)"},
    {"ESC", "ESC (Electronic Stability Control)"},
    {"TractionControl", "Traction Control"},
    {"ForwardCollisionWarning", "Forward Collision Warning"},
    {"BlindSpotMon", "Blind Spot Monitoring"},
    {"BlindSpotIntervention", "Blind Spot Intervention"},
    {"LaneDepartureWarning", "Lane Departure Warning"},
    {"LaneKeepSystem", "Lane Keep Assist"},
    {"LaneCenteringAssistance", "Lane Centering Assistance"},
    {"AdaptiveCruiseControl", "Adaptive Cruise Control"},
    {"AutomaticPedestrianAlertingSound", "Automatic Pedestrian Alerting Sound"},
    {"PedestrianAutomaticEmergencyBraking", "Pedestrian Automatic Emergency Braking"},
    {"RearAutomaticEmergencyBraking", "Rear Automatic Emergency Braking"},
    {"RearCrossTrafficAlert", "Rear Cross-Traffic Alert"},
    {"ParkAssist", "Park Assist"},
    {"RearVisibilitySystem", "Rear Visibility System"},
    {"DynamicBrakeSupport", "Dynamic Brake Support"},
    {"ActiveSafetySysNote", "Active Safety System Note"},
    {"DriverAssist", "Driver Assist"},
    {"CIB", "Crash Imminent Braking (CIB)"},
    {"DaytimeRunningLight", "Daytime Running Lights"},
    {"SemiautomaticHeadlampBeamSwitching", "Semi-Automatic Headlamp Beam Switching"},
    {"AdaptiveDrivingBeam", "Adaptive Driving Beam"},
    {"AdaptiveHeadlights", "Adaptive Headlights"},
    {"AutoReverseSystem", "Auto Reverse System"},
    {"CombinedBrakingSystem", "Combined Braking System"},
};
}

std::vector<std::pair<std::string, std::string>> adasItems(const VehicleInfo& info) {
    std::vector<std::pair<std::string, std::string>> out;
    for (const auto& ak : kAdasKeys) {
        auto it = info.fields.find(ak.key);
        if (it != info.fields.end() && !it->second.empty()) {
            out.emplace_back(ak.label, it->second);
        }
    }
    return out;
}
