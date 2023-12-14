#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::string map_dump(std::vector<std::vector<size_t>>& reference_map, std::string dump_file_name = "");
