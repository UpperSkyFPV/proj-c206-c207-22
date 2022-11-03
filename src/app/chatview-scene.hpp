#pragma once

#include "engine.hpp"
#include "scene.hpp"
#include "state.hpp"

namespace uppr::app {

class ChatViewScene : public eng::Scene {
public:
    ChatViewScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {
        if (chats_needs_update) update_chat();
    }

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        auto t = transform.move(2, 0);
        usize idx{};
        for (const auto &item : state->get_chats()) {
            // Print the title of the chat (its name) in bold
            auto name_style = fg(color::white) | emphasis::bold;
            // If the current chat is the selected one, then mark it in some
            // visible manner
            if (idx == state->get_selected_chat()) {
                name_style |= emphasis::reverse;
            }
            screen.print(t, name_style, "{}", item.name);

            // Print at the right corner the id of the chat
            screen.print(t.move(size.getx() - 4, 0), "{:>3}", item.id);

            // move one line down
            t += term::Transform{0, 1};

            // Print the description of the chat (limiting the size and adding
            // '...' if needed)
            // TODO: Make multiline if needed.
            const auto maxwidth = size.getx() - 4;
            const auto descr =
                static_cast<string_view>(item.description).substr(0, maxwidth);
            screen.print(t, emphasis::faint, "{}{}", descr,
                         item.description.length() > maxwidth ? "..." : "");

            // Move 2 lines down and add the horizontal separator
            t += term::Transform{0, 2};
            screen.hline(t.getx(), size.getx(), t.gety(), '-');

            // Move to the next line before the next iteration
            t += term::Transform{0, 1};
            idx++;
        }
    }

    void mount(eng::Engine &engine) override {
        cycle_chat_keybind_handle = engine.get_eventbus().appendListener(
            'c', [this](char c) { state->select_next_chat(); });
    }

    void unmount(eng::Engine &engine) override {
        engine.get_eventbus().removeListener('c', cycle_chat_keybind_handle);
    }

private:
    /**
     * Update the chats stored in the state.
     *
     * Also, resets the `chats_needs_update` flag.
     */
    void update_chat() {
        state->fetch_chats();

        chats_needs_update = false;
    }

private:
    /**
     * Shared app state.
     */
    shared_ptr<AppState> state;

    /**
     * Handle for the callback for cycling the chat.
     */
    eng::Engine::EventBus::Handle cycle_chat_keybind_handle;

    /**
     * If we need to update the chats from the database.
     */
    bool chats_needs_update{true};
};
} // namespace uppr::app
