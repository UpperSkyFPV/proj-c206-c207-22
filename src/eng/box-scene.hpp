#pragma once

#include "box-options.hpp"
#include "scene.hpp"
#include "vector2.hpp"
#include <memory>

namespace uppr::eng {

/**
 * A scene that has a border arround it.
 */
class BoxScene : public Scene {
public:
    BoxScene(const term::Transform &t, const term::Size &s,
             const term::BoxOptions &box_opts,
             std::shared_ptr<Scene> child_scene)
        : opts{box_opts}, child{child_scene}, origin{t}, size{s} {}

    void update(Engine &engine) override { child->update(engine); }

    void draw(Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        transform += origin;
        screen.box(transform, size.getx(), size.gety(), opts);

        child->draw(engine, origin + term::Transform{1, 1}, size - 1, screen);
    }

    void mount(Engine &engine) override { child->mount(engine); }

    void unmount(Engine &engine) override { child->unmount(engine); }

    static std::shared_ptr<Scene> make(const term::Transform &t,
                                       const term::Size &s,
                                       const term::BoxOptions &box_opts,
                                       std::shared_ptr<Scene> child_scene) {
        return std::make_shared<BoxScene>(t, s, box_opts, child_scene);
    }

private:
    term::BoxOptions opts;

    std::shared_ptr<Scene> child;
    term::Transform origin;
    term::Size size;
};
} // namespace uppr::eng
