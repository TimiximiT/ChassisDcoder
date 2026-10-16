#pragma once

#include <string>
#include "vin.h"

// Builds a console/GUI-ready report from a decoded vehicle.
void buildVinReport(const VehicleInfo& info,
                    const std::string& requestedVin,
                    const std::string& decodeError,
                    std::string& summary,
                    std::string& fullDetails);

