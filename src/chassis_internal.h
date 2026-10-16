#pragma once
#include "chassis.h"
#include <string>

std::vector<ChassisGuess> decodeBmw(const std::string& make, const std::string& model, const std::string& year);
std::vector<ChassisGuess> decodeMini(const std::string& make, const std::string& model, const std::string& year);
std::vector<ChassisGuess> decodeMercedes(const std::string& make, const std::string& model, const std::string& year);
std::vector<ChassisGuess> decodeMazda(const std::string& make, const std::string& model, const std::string& year);
std::vector<ChassisGuess> decodeToyota(const std::string& make, const std::string& model, const std::string& year);
std::vector<ChassisGuess> decodeNissan(const std::string& make, const std::string& model, const std::string& year);
std::vector<ChassisGuess> decodeSubaru(const std::string& make, const std::string& model, const std::string& year);
