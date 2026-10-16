#pragma once
#include <string>

// Returns the response body of an HTTPS GET request, or an empty string on failure.
std::string httpGet(const std::string& host, int port, const std::string& path);
