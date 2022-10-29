#pragma once

#include "scene.hpp"
#include "vector2.hpp"
#include <memory>
#include <vector>

namespace uppr::eng {

/**
 * A scene that store several scenes, that are updated and drawn one after the
 * other.
 */
class SeqScene : public Scene {
public:
    void update(Engine &engine) override {
        for (const auto &child : children) {
            child->update(engine);
        }
    }

    void draw(Engine &engine, term::Transform transform,
              term::TermScreen &screen) override {
        transform += origin;

        for (const auto &child : children) {
            child->draw(engine, transform, screen);
        }
    }

    void mount(Engine &engine) override {
        for (const auto &child : children) {
            child->mount(engine);
        }
    }

    void unmount(Engine &engine) override {
        for (const auto &child : children) {
            child->unmount(engine);
        }
    }

    void add_scene(std::shared_ptr<Scene> new_child) {
        children.push_back(new_child);
    }

private:
    /**
     * All of the sub-scenes.
     */
    std::vector<std::shared_ptr<Scene>> children;

    /**
     * Possible to offset the entire scene tree under this one.
     */
    term::Transform origin;
};
} // namespace uppr::eng
