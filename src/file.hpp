#pragma once

#include <fstream>

#include "commom.hpp"

namespace uppr::file {

/**
 * Read an entire text file into a string and return it.
 */
std::string read_file_text(const std::string &filename);
} // namespace uppr::file
