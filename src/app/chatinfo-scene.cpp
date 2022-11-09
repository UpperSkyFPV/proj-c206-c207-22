#include "chatinfo-scene.hpp"
#include "fmt/color.h"
#include "fmt/core.h"
#include "key.hpp"

namespace uppr::app {
void ChatInfoScene::update(eng::Engine &engine) {
    if (last_selected_chat != state->get_selected_chat()) { update_users(); }
}

void ChatInfoScene::draw(eng::Engine &engine, term::Transform transform,
                         term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    const auto selected_chat = state->get_selected_chatmodel();
    if (selected_chat) {
        draw_chat_info(*selected_chat, engine, transform, size, screen);
    } else {
        constexpr auto fmt = "Nothing selected"sv;
        const auto x = size.getx() / 2 - fmt.length() / 2;

        screen.print(transform.move(x, 5), fmt);
    }
}

void ChatInfoScene::mount(eng::Engine &engine) {
    update_users();

    add_user_keybind_handle =
        engine.get_eventbus().appendListener(term::ctrl('a'), [this](char c) {
            if (state->has_chat_selected()) {
                LOG_F(9, "Adding user to chat!");
            }
        });
}

void ChatInfoScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener(term::ctrl('a'),
                                         add_user_keybind_handle);
}

void ChatInfoScene::draw_chat_info(const models::ChatModel &chat,
                                   eng::Engine &engine,
                                   term::Transform transform, term::Size size,
                                   term::TermScreen &screen) {
    using namespace fmt;

    transform += {0, 1};
    screen.print(transform, emphasis::bold | emphasis::underline, "{}: {}",
                 chat.id, chat.name);

    transform += {0, 2};

    std::vector<std::string> stringified;
    stringified.reserve(users_in_current_chat.size());

    for (const auto &user : users_in_current_chat)
        stringified.push_back(format("{} [{}]", user.name, user.id));

    screen.print(transform, "members: {}", join(stringified, ", "));
}
} // namespace uppr::app
