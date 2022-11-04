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
    ModalScene(shared_ptr<Scene> m, shared_ptr<Scene> b,
               const term::Transform &t = {})
        : modal{m}, background{b}, origin{t} {}

    void update(Engine &engine) override;

    void draw(Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(Engine &engine) override;

    void unmount(Engine &engine) override;

    void show_modal(Engine &engine);

    void hide_modal(Engine &engine);

    /**
     * If the modal is currently beeing rendered.
     */
    bool is_showing() const { return should_show_modal; }

    static std::shared_ptr<ModalScene> make(shared_ptr<Scene> m,
                                            shared_ptr<Scene> b,
                                            const term::Transform &t = {}) {
        return std::make_shared<ModalScene>(m, b, t);
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
     * If we should show the modal.
     */
    bool should_show_modal{};

    /**
     * If the modal has been mounted.
     */
    bool modal_mounted{};
};
} // namespace uppr::eng
