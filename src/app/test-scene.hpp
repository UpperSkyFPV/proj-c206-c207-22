#pragma once

#include "commom.hpp"
#include "eng/engine.hpp"
#include "key.hpp"
#include "screen.hpp"
#include "vector2.hpp"

namespace uppr::app {

class TestScene : public eng::Scene {
public:
    void update(eng::Engine &engine) override {
        const auto c = engine.readc();
        if (term::is_ctrl(c, 'q')) { engine.finalize(); }

        const auto [width, heigh] = engine.get_screen_size();
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
    }

    void draw(eng::Engine &engine, term::Transform transform,
              term::TermScreen &screen) override {
        using namespace fmt;

        screen.print(transform, "Hello there!");
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
