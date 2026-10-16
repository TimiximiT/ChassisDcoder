#pragma once
#include <string>
#include <vector>

struct ChassisGuess {
    std::string code;
    std::string confidence;
    std::string note;
};

// Returns zero or more likely Autel-style chassis codes for the given make/model/year.
std::vector<ChassisGuess> decodeChassis(const std::string& make,
                                        const std::string& model,
                                        const std::string& year);

// True if we have any make-specific heuristics for this make.
bool makeHasChassisHeuristics(const std::string& make);
