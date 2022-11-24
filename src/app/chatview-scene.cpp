#include "chatview-scene.hpp"
#include "fmt/color.h"

namespace uppr::app {

void ChatViewScene::update(eng::Engine &engine) {
    if (chats_needs_update) update_chat();
}

void ChatViewScene::draw(eng::Engine &engine, term::Transform transform,
                         term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    screen.print(transform, emphasis::bold, "'{}' at port {}",
                 state->get_name(), state->get_port());
    transform += {0, 2};

    screen.print(transform, emphasis::underline, "> {} Available chats",
                 state->get_chats().size());
    transform += {0, 2};

    usize idx{};
    for (const auto &item : state->get_chats()) {
        // Print the title of the chat (its name) in bold
        auto name_style = fg(color::white) | emphasis::bold;
        // If the current chat is the selected one, then mark it in some
        // visible manner
        if (idx == state->get_selected_chat()) {
            name_style |= emphasis::reverse;
        }
        screen.print(transform.move(1, 0), name_style, "{}", item.name);

        // Print at the right corner the id of the chat
        screen.print(transform.move(size.getx() - 4, 0), "{:>3}", item.id);

        // move one line down
        transform += term::Transform{0, 1};

        // Print the description of the chat (limiting the size and adding
        // '...' if needed)
        // TODO: Make multiline if needed.
        const auto maxwidth = size.getx() - 4;
        const auto descr =
            static_cast<string_view>(item.description).substr(0, maxwidth);
        screen.print(transform.move(1, 0), emphasis::faint, "{}{}", descr,
                     item.description.length() > maxwidth ? "..." : "");

        // Move 2 lines down and add the horizontal separator
        transform += term::Transform{0, 2};
        screen.hline(transform.getx(), size.getx(), transform.gety(), '-');

        // Move to the next line before the next iteration
        transform += term::Transform{0, 1};
        idx++;
    }
}

void ChatViewScene::mount(eng::Engine &engine) {
    LOG_F(5, "Mount for chatview scene");

    cycle_chat_keybind_handle =
        engine.get_eventbus().appendListener('c', [this](char c) {
            const auto s = state->select_next_chat();
            LOG_F(7, "Selected next chat: {}", s);
        });
}

void ChatViewScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener('c', cycle_chat_keybind_handle);
}
} // namespace uppr::app
