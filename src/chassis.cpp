#include "chassis_internal.h"

#include <algorithm>
#include <cctype>
#include <string>

static std::string lower(const std::string& s) {
    std::string out = s;
    for (char& c : out) c = (char)std::tolower((unsigned char)c);
    return out;
}

bool makeHasChassisHeuristics(const std::string& make) {
    std::string m = lower(make);
    return m.find("bmw") != std::string::npos ||
           m.find("mini") != std::string::npos ||
           m.find("mercedes") != std::string::npos ||
           m.find("mazda") != std::string::npos ||
           m.find("toyota") != std::string::npos ||
           m.find("nissan") != std::string::npos ||
           m.find("subaru") != std::string::npos;
}

std::vector<ChassisGuess> decodeChassis(const std::string& make,
                                       const std::string& model,
                                       const std::string& year) {
    std::string lm = lower(make);
    if (lm.find("bmw") != std::string::npos) return decodeBmw(make, model, year);
    if (lm.find("mini") != std::string::npos) return decodeMini(make, model, year);
    if (lm.find("mercedes") != std::string::npos) return decodeMercedes(make, model, year);
    if (lm.find("mazda") != std::string::npos) return decodeMazda(make, model, year);
    if (lm.find("toyota") != std::string::npos) return decodeToyota(make, model, year);
    if (lm.find("nissan") != std::string::npos) return decodeNissan(make, model, year);
    if (lm.find("subaru") != std::string::npos) return decodeSubaru(make, model, year);
    return {};
}
