#pragma once

#include "engine.hpp"
#include "modal-scene.hpp"
#include "scene.hpp"
#include "state.hpp"
#include <memory>

namespace uppr::app {

class SelectViewScene : public eng::Scene {
public:
    SelectViewScene(shared_ptr<AppState> s,
                    shared_ptr<eng::ModalScene> create_user_modal_,
                    shared_ptr<eng::ModalScene> create_chat_modal_,
                    shared_ptr<eng::ModalScene> add_user_to_chat_modal_,
                    shared_ptr<eng::ModalScene> remove_user_from_chat_modal_)
        : state{s}, create_user_modal{create_user_modal_},
          create_chat_modal{create_chat_modal_},
          add_user_to_chat_modal{add_user_to_chat_modal_},
          remove_user_from_chat_modal{remove_user_from_chat_modal_} {}

    void update(eng::Engine &engine) override;

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<SelectViewScene>
    make(shared_ptr<AppState> s, shared_ptr<eng::ModalScene> create_user_modal,
         shared_ptr<eng::ModalScene> create_chat_modal,
         shared_ptr<eng::ModalScene> add_user_to_chat_modal,
         shared_ptr<eng::ModalScene> remove_user_from_chat_modal) {
        return std::make_shared<SelectViewScene>(
            s, create_user_modal, create_chat_modal, add_user_to_chat_modal,
            remove_user_from_chat_modal);
    }

private:
    void draw_bottom_panel(eng::Engine &engine, term::Transform transform,
                           term::Size size, term::TermScreen &screen) const;

    /**
     * Add the handler for closing the create user modal.
     */
    void add_user_close_handler(eng::Engine &engine);

    /**
     * Add the handler for closing the create chat modal.
     */
    void add_chat_close_handler(eng::Engine &engine);

    /**
     * Add the handler for closing the add user to chat modal.
     */
    void add_adduser_close_handler(eng::Engine &engine);

    /**
     * Add the handler for closing the remove user to chat modal.
     */
    void add_removeuser_close_handler(eng::Engine &engine);

    /**
     * Remove the handler for closing any modal.
     */
    void remove_close_handler(eng::Engine &engine);

    /**
     * Returns true if any of the modals is open.
     */
    bool any_is_open() const {
        return create_chat_modal->is_showing() ||
               create_user_modal->is_showing() ||
               add_user_to_chat_modal->is_showing();
    }

private:
    /**
     * Shared app state.
     */
    shared_ptr<AppState> state;

    shared_ptr<eng::ModalScene> create_user_modal;
    shared_ptr<eng::ModalScene> create_chat_modal;
    shared_ptr<eng::ModalScene> add_user_to_chat_modal;
    shared_ptr<eng::ModalScene> remove_user_from_chat_modal;

    eng::Engine::EventBus::Handle open_create_user_keybind_handle;
    eng::Engine::EventBus::Handle open_create_chat_keybind_handle;
    eng::Engine::EventBus::Handle open_add_user_to_chat_keybind_handle;
    eng::Engine::EventBus::Handle open_remove_user_from_chat_keybind_handle;
    eng::Engine::EventBus::Handle close_any_modal_keybind_handle;
};
} // namespace uppr::app
