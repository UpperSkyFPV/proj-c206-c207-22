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
class StackScene : public Scene {
public:
    void update(Engine &engine) override {
        for (const auto &[_, child] : children) {
            child->update(engine);
        }
    }

    void draw(Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        transform += origin;

        for (const auto &[_, child] : children) {
            child->draw(engine, transform, size, screen);
        }
    }

    void mount(Engine &engine) override {
        for (auto &[mounted, child] : children) {
            if (!mounted) {
                child->mount(engine);
                mounted = true;
            }
        }
    }

    void unmount(Engine &engine) override {
        for (auto &[mounted, child] : children) {
            if (mounted) {
                child->unmount(engine);
                mounted = false;
            }
        }
    }

    void add_scene(Engine &engine, std::shared_ptr<Scene> new_child) {
        children.push_back({true, new_child});

        new_child->mount(engine);
    }

    static std::shared_ptr<StackScene> make() {
        return std::make_shared<StackScene>();
    }

private:
    /**
     * All of the sub-scenes.
     */
    std::vector<std::pair<bool, std::shared_ptr<Scene>>> children;

    /**
     * Possible to offset the entire scene tree under this one.
     */
    term::Transform origin;
};
} // namespace uppr::eng
