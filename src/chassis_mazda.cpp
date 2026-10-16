#include "chassis_internal.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

static std::string lower(const std::string& s) {
    std::string o = s;
    for (char& c : o) c = (char)std::tolower((unsigned char)c);
    return o;
}
static int yi(const std::string& s) { try { return std::stoi(s); } catch (...) { return 0; } }
static bool has(const std::string& s, const std::string& p) { return lower(s).find(lower(p)) != std::string::npos; }
static std::vector<ChassisGuess> M(std::vector<std::string> c, const std::string& n) {
    std::vector<ChassisGuess> o;
    for (auto& x : c) o.push_back({x, "possible/likely", n});
    return o;
}

std::vector<ChassisGuess> decodeMazda(const std::string& /*make*/, const std::string& model, const std::string& year) {
    int y = yi(year);
    std::string m = lower(model);
    if (has(model, "mx-5") || has(model, "mx5") || has(model, "miata")) {
        if (y <= 1997) return M({"NA"}, "MX-5 NA");
        if (y <= 2005) return M({"NB"}, "MX-5 NB");
        if (y <= 2015) return M({"NC"}, "MX-5 NC");
        return M({"ND"}, "MX-5 ND");
    }
    if (has(model, "mazda3") || has(model, "mazda 3") || (m.find("3") != std::string::npos && m.find("mazda6") == std::string::npos)) {
        if (y >= 2003 && y <= 2009) return M({"BK"}, "Mazda3 BK");
        if (y >= 2009 && y <= 2013) return M({"BL"}, "Mazda3 BL");
        if (y >= 2014 && y <= 2018) return M({"BM", "BN", "BY"}, "Mazda3 BM/BN/BY");
        if (y >= 2019) return M({"BP"}, "Mazda3 BP");
    }
    if (has(model, "mazda6") || has(model, "mazda 6")) {
        if (y >= 2002 && y <= 2008) return M({"GG", "GY"}, "Mazda6 GG/GY");
        if (y >= 2009 && y <= 2012) return M({"GH"}, "Mazda6 GH");
        if (y >= 2013 && y <= 2021) return M({"GJ"}, "Mazda6 GJ");
        if (y >= 2022) return M({"GL"}, "Mazda6 GL");
    }
    if (has(model, "cx-5") || has(model, "cx5")) {
        if (y <= 2016) return M({"KE"}, "CX-5 KE");
        return M({"KF"}, "CX-5 KF");
    }
    if (has(model, "cx-3")) return M({"DK"}, "CX-3 DK");
    if (has(model, "cx-30")) return M({"DM"}, "CX-30 DM");
    if (has(model, "cx-9")) {
        if (y <= 2015) return M({"TB"}, "CX-9 TB");
        return M({"TC"}, "CX-9 TC");
    }
    if (has(model, "mazda2") || has(model, "mazda 2") || has(model, "demio")) {
        if (y <= 2014) return M({"DE", "DJ"}, "Mazda2 DE/DJ");
        if (y <= 2019) return M({"DJ"}, "Mazda2 DJ");
        return M({"DL"}, "Mazda2 DL");
    }
    return {};
}
