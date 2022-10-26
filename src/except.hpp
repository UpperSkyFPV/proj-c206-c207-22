#pragma once

#include "db/result.hpp"
#include "loguru.hpp"

namespace uppr::except {

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
