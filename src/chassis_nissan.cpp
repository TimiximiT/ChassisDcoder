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

std::vector<ChassisGuess> decodeNissan(const std::string& /*make*/, const std::string& model, const std::string& year) {
    int y = yi(year);
    if (has(model, "sentra")) {
        if (y >= 2000 && y <= 2006) return M({"B15"}, "Sentra B15");
        if (y >= 2007 && y <= 2012) return M({"B16"}, "Sentra B16");
        if (y >= 2013 && y <= 2019) return M({"B17"}, "Sentra B17");
        if (y >= 2020) return M({"B18"}, "Sentra B18");
    }
    if (has(model, "altima")) {
        if (y >= 2002 && y <= 2006) return M({"L31"}, "Altima L31");
        if (y >= 2007 && y <= 2012) return M({"L32"}, "Altima L32");
        if (y >= 2013 && y <= 2018) return M({"L33"}, "Altima L33");
        if (y >= 2019) return M({"L34"}, "Altima L34");
    }
    if (has(model, "maxima")) {
        if (y >= 2004 && y <= 2008) return M({"A34"}, "Maxima A34");
        if (y >= 2009 && y <= 2014) return M({"A35"}, "Maxima A35");
        if (y >= 2015) return M({"A36"}, "Maxima A36");
    }
    if (has(model, "rogue")) {
        if (y >= 2008 && y <= 2015) return M({"T31"}, "Rogue first gen");
        if (y >= 2014 && y <= 2020) return M({"T32"}, "Rogue second gen");
        if (y >= 2021) return M({"T33"}, "Rogue third gen");
    }
    if (has(model, "leaf")) {
        if (y <= 2017) return M({"ZE0"}, "Leaf ZE0");
        if (y <= 2024) return M({"ZE1"}, "Leaf ZE1");
        return M({"ZE2"}, "Leaf ZE2");
    }
    if (has(model, "qashqai") || has(model, "rogue sport")) {
        if (y <= 2013) return M({"J10"}, "Qashqai J10");
        if (y <= 2021) return M({"J11"}, "Qashqai J11");
        return M({"J12"}, "Qashqai J12");
    }
    return {};
}
