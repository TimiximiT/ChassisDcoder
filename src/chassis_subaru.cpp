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

std::vector<ChassisGuess> decodeSubaru(const std::string& /*make*/, const std::string& model, const std::string& year) {
    int y = yi(year);
    if (has(model, "impreza") || has(model, "wrx") || has(model, "sti")) {
        if (y >= 1993 && y <= 2000) return M({"GC", "GF"}, "Impreza GC/GF");
        if (y >= 2001 && y <= 2007) return M({"GD", "GG"}, "Impreza GD/GG");
        if (y >= 2008 && y <= 2011) return M({"GE", "GH"}, "Impreza GE/GH");
        if (y >= 2012 && y <= 2016) return M({"GJ", "GP"}, "Impreza GJ/GP");
        if (y >= 2017) return M({"GK", "GT"}, "Impreza GK/GT");
    }
    if (has(model, "forester")) {
        if (y >= 1998 && y <= 2002) return M({"SF"}, "Forester SF");
        if (y >= 2003 && y <= 2008) return M({"SG"}, "Forester SG");
        if (y >= 2009 && y <= 2013) return M({"SH"}, "Forester SH");
        if (y >= 2014 && y <= 2018) return M({"SJ"}, "Forester SJ");
        if (y >= 2019) return M({"SK"}, "Forester SK");
    }
    if (has(model, "outback")) {
        if (y >= 2000 && y <= 2009) return M({"BP", "BL"}, "Outback generation");
        if (y >= 2010 && y <= 2014) return M({"BR"}, "Outback BR");
        if (y >= 2015 && y <= 2019) return M({"BS"}, "Outback BS");
        if (y >= 2020) return M({"BT"}, "Outback BT");
    }
    if (has(model, "crosstrek") || has(model, "xv")) {
        if (y <= 2017) return M({"GP"}, "Crosstrek GP");
        return M({"GT"}, "Crosstrek GT");
    }
    if (has(model, "legacy")) {
        if (y <= 2004) return M({"BH", "BK"}, "Legacy BH/BK");
        if (y <= 2009) return M({"BL", "BP"}, "Legacy BL/BP");
        if (y <= 2014) return M({"BM", "BR"}, "Legacy BM/BR");
        return M({"BN", "BT"}, "Legacy BN/BT");
    }
    return {};
}
