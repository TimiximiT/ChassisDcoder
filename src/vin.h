#pragma once
#include <map>
#include <string>
#include <utility>
#include <vector>

struct VehicleInfo {
    std::string vin;
    std::string year;
    std::string make;
    std::string model;
    std::map<std::string, std::string> fields;
};

bool isValidVin(const std::string& vin);
std::string normalizeVin(const std::string& vin);

// Decodes a VIN via the NHTSA VPIC API. Sets errorMessage on failure.
VehicleInfo decodeVin(const std::string& vin, std::string& errorMessage);

// Returns (friendly name, value) pairs for ADAS/safety items with data.
std::vector<std::pair<std::string, std::string>> adasItems(const VehicleInfo& info);
