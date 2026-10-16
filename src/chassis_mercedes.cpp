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
static std::vector<ChassisGuess> M(std::vector<std::string> c, const std::string& n) {
    std::vector<ChassisGuess> o;
    for (auto& x : c) o.push_back({x, "possible/likely", n});
    return o;
}

std::vector<ChassisGuess> decodeMercedes(const std::string& /*make*/, const std::string& model, const std::string& year) {
    int y = yi(year);
    std::string compact;
    for (char c : lower(model)) if (c != ' ' && c != '-') compact.push_back(c);

    std::string cls;
    if (compact.find("cla") == 0) cls = "CLA";
    else if (compact.find("cle") == 0) cls = "CLE";
    else if (compact.find("cls") == 0) cls = "CLS";
    else if (compact.find("glc") == 0) cls = "GLC";
    else if (compact.find("gle") == 0) cls = "GLE";
    else if (compact.find("gls") == 0) cls = "GLS";
    else if (compact.find("gla") == 0) cls = "GLA";
    else if (compact.find("glb") == 0) cls = "GLB";
    else if (compact.find("slc") == 0 || compact.find("slk") == 0) cls = "SLK";
    else if (compact.find("sl") == 0) cls = "SL";
    else if (!compact.empty() && std::isalpha((unsigned char)compact[0])) cls = std::string(1, (char)std::toupper((unsigned char)compact[0]));

    if (cls == "A") {
        if (y >= 2005 && y <= 2012) return M({"W169"}, "Mercedes A-Class W169");
        if (y >= 2013 && y <= 2018) return M({"W176"}, "Mercedes A-Class W176");
        if (y >= 2019) return M({"W177"}, "Mercedes A-Class W177");
    } else if (cls == "B") {
        if (y >= 2005 && y <= 2011) return M({"W245"}, "Mercedes B-Class W245");
        if (y >= 2012 && y <= 2018) return M({"W246"}, "Mercedes B-Class W246");
        if (y >= 2019) return M({"W247"}, "Mercedes B-Class W247");
    } else if (cls == "C") {
        if (y >= 2001 && y <= 2007) return M({"W203", "S203", "CL203"}, "Mercedes C-Class W203");
        if (y >= 2007 && y <= 2014) return M({"W204", "S204", "C204"}, "Mercedes C-Class W204");
        if (y >= 2015 && y <= 2021) return M({"W205", "S205", "C205"}, "Mercedes C-Class W205");
        if (y >= 2022) return M({"W206", "S206"}, "Mercedes C-Class W206");
    } else if (cls == "E") {
        if (y >= 2003 && y <= 2009) return M({"W211", "S211"}, "Mercedes E-Class W211");
        if (y >= 2010 && y <= 2016) return M({"W212", "S212", "C207"}, "Mercedes E-Class W212");
        if (y >= 2017 && y <= 2023) return M({"W213", "S213", "C238"}, "Mercedes E-Class W213");
        if (y >= 2024) return M({"W214"}, "Mercedes E-Class W214");
    } else if (cls == "S") {
        if (y >= 2000 && y <= 2005) return M({"W220"}, "Mercedes S-Class W220");
        if (y >= 2006 && y <= 2013) return M({"W221"}, "Mercedes S-Class W221");
        if (y >= 2014 && y <= 2020) return M({"W222"}, "Mercedes S-Class W222");
        if (y >= 2021) return M({"W223"}, "Mercedes S-Class W223");
    } else if (cls == "CLA") {
        if (y <= 2019) return M({"C117"}, "Mercedes CLA C117");
        return M({"C118"}, "Mercedes CLA C118");
    } else if (cls == "CLS") {
        if (y <= 2010) return M({"C219"}, "Mercedes CLS C219");
        if (y <= 2018) return M({"C218"}, "Mercedes CLS C218");
        return M({"C257"}, "Mercedes CLS C257");
    } else if (cls == "GLA") {
        if (y <= 2019) return M({"X156"}, "Mercedes GLA X156");
        return M({"H247"}, "Mercedes GLA H247");
    } else if (cls == "GLB") {
        return M({"X247"}, "Mercedes GLB X247");
    } else if (cls == "GLC") {
        if (y <= 2022) return M({"X253"}, "Mercedes GLC X253");
        return M({"X254"}, "Mercedes GLC X254");
    } else if (cls == "GLE") {
        if (y <= 2018) return M({"W166"}, "Mercedes GLE (W166)");
        return M({"V167"}, "Mercedes GLE V167");
    } else if (cls == "GLS") {
        if (y <= 2019) return M({"X166"}, "Mercedes GL/GLS X166");
        return M({"X167"}, "Mercedes GLS X167");
    } else if (cls == "G") {
        if (y <= 2018) return M({"W460", "W461", "W463"}, "Mercedes G-Class older");
        return M({"W463", "W464", "W465"}, "Mercedes G-Class new gen");
    }
    return {};
}
