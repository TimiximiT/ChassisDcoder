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

std::vector<ChassisGuess> decodeBmw(const std::string& make, const std::string& model, const std::string& year) {
    (void)make;
    int y = yi(year);
    std::string sl = lower(model);
    std::string series;
    if (has(model, "i4")) series = "i4";
    else if (has(model, "i5")) series = "i5";
    else if (has(model, "i7")) series = "i7";
    else if (has(model, "i8")) series = "i8";
    else if (has(model, "ix")) series = "iX";
    else if (has(model, "x1")) series = "X1";
    else if (has(model, "x2")) series = "X2";
    else if (has(model, "x3")) series = "X3";
    else if (has(model, "x4")) series = "X4";
    else if (has(model, "x5")) series = "X5";
    else if (has(model, "x6")) series = "X6";
    else if (has(model, "x7")) series = "X7";
    else if (has(model, "z4")) series = "Z4";
    else {
        for (char c : model) if (std::isdigit((unsigned char)c)) { series = std::string(1, c) + " Series"; break; }
    }

    std::vector<ChassisGuess> res;
    if (series.rfind("1", 0) == 0 || series == "1 Series") {
        if (y >= 2004 && y <= 2014) res = M({"E81", "E82", "E87", "E88"}, "BMW 1 Series (1st gen)");
        else if (y >= 2011 && y <= 2019) res = M({"F20", "F21"}, "BMW 1 Series (2nd gen)");
        else if (y >= 2019) res = M({"F40", "F44", "F52", "U06"}, "BMW 1 Series (3rd gen)");
    } else if (series.rfind("2", 0) == 0 || series == "2 Series") {
        if (y >= 2014 && y <= 2021) res = M({"F22", "F23", "F44", "F45", "F46"}, "BMW 2 Series");
        else if (y >= 2022) res = M({"G42", "U06", "F74"}, "BMW 2 Series newer");
    } else if (series.rfind("3", 0) == 0 || series == "3 Series") {
        if (y >= 1998 && y <= 2006) res = M({"E46"}, "BMW 3 Series E46");
        else if (y >= 2005 && y <= 2013) res = M({"E90", "E91", "E92", "E93"}, "BMW 3 Series E9x");
        else if (y >= 2012 && y <= 2018) res = M({"F30", "F31", "F34", "F35"}, "BMW 3 Series F3x");
        else if (y >= 2019) res = M({"G20", "G21", "G28"}, "BMW 3 Series G2x");
    } else if (series.rfind("4", 0) == 0 || series == "4 Series") {
        if (y >= 2013 && y <= 2020) res = M({"F32", "F33", "F36"}, "BMW 4 Series F3x");
        else if (y >= 2021) res = M({"G22", "G23", "G26"}, "BMW 4 Series G2x");
    } else if (series.rfind("5", 0) == 0 || series == "5 Series") {
        if (y >= 2004 && y <= 2010) res = M({"E60", "E61"}, "BMW 5 Series E6x");
        else if (y >= 2010 && y <= 2017) res = M({"F10", "F11", "F07"}, "BMW 5 Series F1x");
        else if (y >= 2017 && y <= 2023) res = M({"G30", "G31"}, "BMW 5 Series G3x");
        else if (y >= 2024) res = M({"G60", "G61", "G68"}, "BMW 5 Series G6x");
    }
    else if (series.rfind("6", 0) == 0 || series == "6 Series") {
        if (y >= 2004 && y <= 2010) res = M({"E63", "E64"}, "BMW 6 Series E6x");
        else if (y >= 2011 && y <= 2018) res = M({"F12", "F13", "F06"}, "BMW 6 Series F1x");
        else if (y >= 2018) res = M({"G32"}, "BMW 6 Series G32");
    } else if (series.rfind("7", 0) == 0 || series == "7 Series") {
        if (y >= 2002 && y <= 2008) res = M({"E65", "E66"}, "BMW 7 Series E65");
        else if (y >= 2009 && y <= 2015) res = M({"F01", "F02", "F03", "F04"}, "BMW 7 Series F0x");
        else if (y >= 2016 && y <= 2022) res = M({"G11", "G12"}, "BMW 7 Series G1x");
        else if (y >= 2023) res = M({"G70", "G71"}, "BMW 7 Series G7x");
    } else if (series.rfind("8", 0) == 0 || series == "8 Series") {
        if (y >= 2018) res = M({"G14", "G15", "G16"}, "BMW 8 Series G1x");
    } else if (series == "X1") {
        if (y >= 2009 && y <= 2015) res = M({"E84"}, "BMW X1 E84");
        else res = M({"F48", "U11"}, "BMW X1 F/U");
    } else if (series == "X3") {
        if (y >= 2004 && y <= 2010) res = M({"E83"}, "BMW X3 E83");
        else if (y >= 2011 && y <= 2017) res = M({"F25"}, "BMW X3 F25");
        else if (y >= 2018) res = M({"G01", "G08"}, "BMW X3 G01");
    } else if (series == "X5") {
        if (y >= 2000 && y <= 2006) res = M({"E53"}, "BMW X5 E53");
        else if (y >= 2007 && y <= 2013) res = M({"E70"}, "BMW X5 E70");
        else if (y >= 2014 && y <= 2018) res = M({"F15"}, "BMW X5 F15");
        else if (y >= 2019) res = M({"G05"}, "BMW X5 G05");
    } else if (series == "X6") {
        if (y >= 2008 && y <= 2014) res = M({"E71"}, "BMW X6 E71");
        else if (y >= 2015 && y <= 2019) res = M({"F16"}, "BMW X6 F16");
        else if (y >= 2020) res = M({"G06"}, "BMW X6 G06");
    } else if (series == "Z4") {
        if (y >= 2003 && y <= 2008) res = M({"E85"}, "BMW Z4 E85");
        else if (y >= 2009 && y <= 2016) res = M({"E89"}, "BMW Z4 E89");
        else if (y >= 2019) res = M({"G29"}, "BMW Z4 G29");
    } else if (series == "i4") {
        res = M({"G26"}, "BMW i4");
    } else if (series == "i5") {
        res = M({"G60"}, "BMW i5");
    } else if (series == "i7") {
        res = M({"G70"}, "BMW i7");
    } else if (series == "i8") {
        res = M({"I12"}, "BMW i8");
    } else if (series == "iX") {
        res = M({"I20"}, "BMW iX");
    }
    (void)sl;
    return res;
}

