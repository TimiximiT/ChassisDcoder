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

std::vector<ChassisGuess> decodeMini(const std::string& /*make*/, const std::string& model, const std::string& year) {
    int y = yi(year);
    if (y <= 2006) {
        std::vector<std::string> codes = {"R50", "R53"};
        if (has(model, "convert")) codes.push_back("R52");
        return M(codes, "Mini (classic BMW-era, trim/body dependent)");
    }
    if (y <= 2013) {
        if (has(model, "countryman")) return M({"R60"}, "Mini Countryman R60");
        if (has(model, "pacaman") || has(model, "paceman")) return M({"R61"}, "Mini Paceman R61");
        if (has(model, "clubman")) return M({"R55"}, "Mini Clubman R55");
        if (has(model, "convert")) return M({"R57"}, "Mini Convertible R57");
        if (has(model, "coupe")) return M({"R58"}, "Mini Coupe R58");
        if (has(model, "roadster")) return M({"R59"}, "Mini Roadster R59");
        return M({"R56"}, "Mini Hatch R56");
    }
    if (has(model, "countryman")) return M({"F60"}, "Mini Countryman F60");
    if (has(model, "clubman")) return M({"F54"}, "Mini Clubman F54");
    if (has(model, "convert")) return M({"F57"}, "Mini Convertible F57");
    if (has(model, "5") && has(model, "door")) return M({"F55"}, "Mini 5-door F55");
    if (y >= 2024 && has(model, "electric")) return M({"J01"}, "Mini Electric J01");
    return M({"F56"}, "Mini 3-door F56");
}
