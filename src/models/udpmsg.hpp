#pragma once

#include "commom.hpp"

namespace uppr::models {

struct UdpMessage {
    std::string content;
    std::string sent_by; // name of who sent the message

    template <typename T>
    void pack(T &o) {
        o(content, sent_by);
    }
};
} // namespace uppr::models
