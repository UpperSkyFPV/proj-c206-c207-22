#pragma once

#include "db/result.hpp"
#include "loguru.hpp"

namespace uppr::except {

/**
 * Put a try catch over a function call (the single argument) that captures a
 * `DatabaseError` and log it as a FATAL (which terminates the program).
 *
 * Use this in places where the database **MUST** work.
 */
constexpr auto wrap_fatal_exception(auto &&executor) {
    try {
        return executor();
    } catch (const db::DatabaseError &e) {
        LOG_F(FATAL, "Exception captured by wrap: %s", e.what());

        // unreachable
        __builtin_unreachable();
    }
}
} // namespace uppr::except
