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

        const auto max_frame = engine.get_max_frame_time();
        const auto frame = engine.get_frame_time();
        const auto update = engine.get_update_time();
        const auto draw = engine.get_draw_time();
        const auto commit = engine.get_commit_time();

        const auto percent = [&](double time) {
            return (time / max_frame) * 100;
        };

        const auto pframe = percent(frame);
        const auto pupdate = percent(update);
        const auto pdraw = percent(draw);
        const auto pcommit = percent(commit);

        screen.print(transform.move(size.getx() - 60, 0),
                     "{:05d}/{:05d}us ({:0.2f}%, {:0.2f}%, {:0.2f}%, {:0.2f}%)",
                     frame, max_frame, pframe, pupdate, pdraw, pcommit);
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
