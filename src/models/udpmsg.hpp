#pragma once

#include "commom.hpp"

namespace uppr::models {

struct UdpMessage {
    std::string content;
    std::string sent_by;   // name of who sent the message
    std::string sent_from; // name of the chat where the message was sent to

    template <typename T>
    void pack(T &o) {
        o(content, sent_by, sent_from);
    }
};
} // namespace uppr::models
