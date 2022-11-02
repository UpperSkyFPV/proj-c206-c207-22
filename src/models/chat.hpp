#pragma once

#include "commom.hpp"

namespace uppr::models {

/**
 * The in-program structure to represent a row in the `Chat` table.
 */
struct ChatModel {
    int id;
    std::string name;
    std::string description;
};
} // namespace uppr::models
