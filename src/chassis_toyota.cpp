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

std::vector<ChassisGuess> decodeToyota(const std::string& /*make*/, const std::string& model, const std::string& year) {
    int y = yi(year);
    std::string m = lower(model);
    if (has(model, "corolla")) {
        if (y >= 2003 && y <= 2008) return M({"E120", "E130"}, "Corolla E120/E130");
        if (y >= 2009 && y <= 2013) return M({"E140", "E150"}, "Corolla E140/E150");
        if (y >= 2014 && y <= 2019) return M({"E160", "E170", "E180"}, "Corolla E160/E170/E180");
        if (y >= 2020) return M({"E210"}, "Corolla E210");
    }
    if (has(model, "camry")) {
        if (y >= 2002 && y <= 2006) return M({"XV30"}, "Camry XV30");
        if (y >= 2007 && y <= 2011) return M({"XV40"}, "Camry XV40");
        if (y >= 2012 && y <= 2017) return M({"XV50"}, "Camry XV50");
        if (y >= 2018) return M({"XV70"}, "Camry XV70");
    }
    if (has(model, "rav4") || has(model, "rav 4")) {
        if (y >= 2001 && y <= 2005) return M({"XA20"}, "RAV4 XA20");
        if (y >= 2006 && y <= 2012) return M({"XA30"}, "RAV4 XA30");
        if (y >= 2013 && y <= 2018) return M({"XA40"}, "RAV4 XA40");
        if (y >= 2019) return M({"XA50"}, "RAV4 XA50");
    }
    if (has(model, "yaris")) {
        if (y >= 2006 && y <= 2010) return M({"XP90"}, "Yaris XP90");
        if (y >= 2011 && y <= 2019) return M({"XP130"}, "Yaris XP130");
        if (y >= 2020) return M({"XP210"}, "Yaris XP210");
    }
    if (has(model, "prius")) {
        if (y >= 2004 && y <= 2009) return M({"XW20"}, "Prius XW20");
        if (y >= 2010 && y <= 2015) return M({"XW30"}, "Prius XW30");
        if (y >= 2016) return M({"XW50"}, "Prius XW50");
    }
    if (has(model, "highlander") || has(model, "kluger")) {
        if (y >= 2008 && y <= 2013) return M({"ASU40", "GSU40"}, "Highlander gen");
        if (y >= 2014 && y <= 2019) return M({"ASU40", "GSU45"}, "Highlander gen");
        if (y >= 2020) return M({"ASU50", "GSU55"}, "Highlander gen");
    }
    return {};
}
