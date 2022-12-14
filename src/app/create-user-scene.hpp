#pragma once

#include "engine.hpp"
#include "fmt/color.h"
#include "scene.hpp"
#include "state.hpp"
#include "vector2.hpp"
#include <iostream>
#include <string>

namespace uppr::app {

/**
 * This is a simple form that requires the input for creating a new user.
 */
class CreateUserScene : public eng::Scene {
    enum class Item { username, addr_host, addr_port, confirm };

    struct CreateData {
        std::string username;
        std::string addr_host;
        std::string addr_port;

        void reset() {
            username.clear();
            addr_host.clear();
            addr_port.clear();
        }

        bool any_empty() const {
            return username.empty() || addr_host.empty() || addr_port.empty();
        }
    };

public:
    CreateUserScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<CreateUserScene> make(shared_ptr<AppState> s) {
        return std::make_shared<CreateUserScene>(s);
    }

private:
    /**
     * Move the `selected_item` to the next possible item in the order `username
     * -> host -> port -> username -> ...`
     */
    void select_next_item();

    /**
     * Move the `selected_item` to the previous possible item in the order
     * `username -> confirm -> addr_port -> addr_host -> username -> ...`
     */
    void select_prev_item();

    /**
     * Called when we want to save data!
     */
    void confirm(term::Transform transform, term::TermScreen &screen);

private:
    /**
     * Shared app state.
     */
    shared_ptr<AppState> state;

    /**
     * Handle for the callback for entering edit mode.
     */
    eng::Engine::EventBus::Handle edit_item_keybind_handle;

    /**
     * Handle for the callback for selecting the next input
     */
    eng::Engine::EventBus::Handle select_next_keybind_handle;

    /**
     * Handle for the callback for selecting the previous input
     */
    eng::Engine::EventBus::Handle select_prev_keybind_handle;

    /**
     * Data used during creation of data.
     */
    CreateData create_data;

    /**
     * The currently selected input item.
     */
    Item selected_item{Item::username};

    /**
     * If the user pressed a key that says: I want to type into the field.
     */
    bool wants_input{};
};
} // namespace uppr::app
