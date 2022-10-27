#pragma once

#include "commom.hpp"
#include "eng/engine.hpp"
#include "key.hpp"

namespace uppr::app {

class TestScene : public eng::Scene {
public:
    void update(eng::Engine &engine) override {
        const auto c = engine.get_term()->readc();
        if (term::is_ctrl(c, 'q')) { engine.finalize(); }

        const auto [width, heigh] = engine.get_term()->get_size();
        const auto start = 0 + 20;
        const auto end = width - 20;
        if (position < start) {
            position = start;
            velocity = 1;
        }
        if (position > end) {
            position = end;
            velocity = -1;
        }

        position += velocity;

        engine.get_term()->print(position, heigh / 2, fmt::fg(fmt::color::yellow), "@");

        engine.get_term()->print(2, 2, "{}/{}us", engine.get_frame_time(), engine.get_max_frame_time());
    }

    void mount(eng::Engine &engine) override { LOG_F(INFO, "mount TestScene"); }

    void unmount(eng::Engine &engine) override {
        LOG_F(INFO, "unmount TestScene");
    }

private:
    int position{};
    int velocity{1};
};
} // namespace uppr::app
