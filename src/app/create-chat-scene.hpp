#pragma once

#include "scene.hpp"
#include "state.hpp"
#include "engine.hpp"

namespace uppr::app {
/**
 * This is a simple form that requires the input for creating a new user.
 */
class CreateChatScene : public eng::Scene {
    enum class Item { name, description, confirm };

    struct CreateData {
        std::string name;
        std::string description;

        void reset() {
            name.clear();
            description.clear();
        }

        bool any_empty() const { return name.empty() || description.empty(); }
    };

public:
    CreateChatScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<CreateChatScene> make(shared_ptr<AppState> s) {
        return std::make_shared<CreateChatScene>(s);
    }

private:
    /**
     * Move the `selected_item` to the next possible item in the order `name ->
     * description -> confirm -> name -> ...`
     */
    void select_next_item();

    /**
     * Move the `selected_item` to the previous possible item in the order
     * `name -> confirm -> description -> name -> ...`
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
    Item selected_item{Item::name};

    /**
     * If the user pressed a key that says: I want to type into the field.
     */
    bool wants_input{};
};
} // namespace uppr::app
