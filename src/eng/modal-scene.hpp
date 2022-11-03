#pragma once

#include "engine.hpp"
#include "scene.hpp"
#include "vector2.hpp"
#include <memory>

namespace uppr::eng {

/**
 * This scene shows another one if requested via a flag.
 */
class ModalScene : public Scene {
public:
    ModalScene(shared_ptr<Scene> m, shared_ptr<Scene> b, const term::Size &ms,
               const term::Transform &t = {})
        : modal{m}, background{b}, origin{t}, modal_size{ms} {}

    void update(Engine &engine) override {
        background->update(engine);

        if (modal && should_show_modal) modal->update(engine);
    }

    void draw(Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        background->draw(engine, transform, size, screen);

        if (modal && should_show_modal) {
            transform += origin;

            modal->draw(engine, transform, modal_size, screen);
        }
    }

    void mount(Engine &engine) override {
        background->mount(engine);

        if (should_show_modal && !modal_mounted) {
            modal->mount(engine);
            modal_mounted = true;
        }
    }

    void unmount(Engine &engine) override {
        background->unmount(engine);

        if (should_show_modal && modal_mounted) {
            modal->unmount(engine);
            modal_mounted = false;
        }
    }

    void show_modal(Engine &engine) {
        if (!should_show_modal && !modal_mounted) {
            modal->mount(engine);
            modal_mounted = true;
        }

        should_show_modal = true;
    }

    void hide_modal(Engine &engine) {
        if (should_show_modal && modal_mounted) {
            modal->unmount(engine);
            modal_mounted = false;
        }

        should_show_modal = false;
    }

    static std::shared_ptr<ModalScene> make(shared_ptr<Scene> m,
                                            shared_ptr<Scene> b,
                                            const term::Size &ms,
                                            const term::Transform &t = {}) {
        return std::make_shared<ModalScene>(m, b, ms, t);
    }

private:
    /**
     * The modal popup scene.
     */
    shared_ptr<Scene> modal;

    /**
     * The background scene (That should always be rendered behind the modal).
     */
    shared_ptr<Scene> background;

    /**
     * The offset for the popup scene.
     */
    term::Transform origin;

    /**
     * The size of the modal.
     */
    term::Size modal_size;

    /**
     * If we should show the modal.
     */
    bool should_show_modal{};

    /**
     * If the modal has been mounted.
     */
    bool modal_mounted{};
};
} // namespace uppr::eng
