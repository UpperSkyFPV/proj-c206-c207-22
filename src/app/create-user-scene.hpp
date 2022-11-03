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
     * Draw one input field.
     *
     * @param kind is used to decide if the input title should be highlighted.
     * @param transform Where to place the field.
     * @param screen reference to the screen object
     * @param title The title to give to the input widget
     * @param contents What to render inside the input
     */
    term::Transform draw_input(Item item_kind, term::Transform transform,
                               term::TermScreen &screen, string_view title,
                               string_view contents);

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

    Item selected_item{Item::username};

    bool wants_input{};
};
} // namespace uppr::app
