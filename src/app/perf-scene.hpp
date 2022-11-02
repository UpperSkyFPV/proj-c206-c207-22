#pragma once

#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "key.hpp"
#include "vector2.hpp"

namespace uppr::app {

/**
 * This scene simply show performance information at the top-right edge of the
 * parent scene.
 */
class PerfScene : public eng::Scene {
public:
    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        const auto f = engine.get_frame_time();
        const auto m = engine.get_max_frame_time();
        const auto p = (static_cast<double>(f) / m) * 100;

        screen.print(transform.move(size.getx() - 30, 0),
                     "{:05d}/{:05d}us ({:0.2f}%)", f, m, p);
    }

    /**
     * Create an instance of this scene as a `shared_ptr`.
     */
    static shared_ptr<PerfScene> make() {
        return std::make_shared<PerfScene>();
    }

private:
};
} // namespace uppr::app
