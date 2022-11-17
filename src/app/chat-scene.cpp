#include "chat-scene.hpp"
#include "key.hpp"

namespace uppr::app {

void ChatScene::update(eng::Engine &engine) {
    chat_info->update(engine);
    write_msg->update(engine);
}

void ChatScene::draw(eng::Engine &engine, term::Transform transform,
                     term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    const auto info_size = term::Size{size.getx(), 5};

    chat_info->draw(engine, transform, info_size, screen);
    transform += {0, static_cast<int>(info_size.gety())};

    screen.hline(transform.getx() + 1, transform.getx() + size.getx() - 1,
                 transform.gety(), '=');

    const auto limit = transform;

    // Move to the bottom of the screen
    transform.y = size.gety() - 4;
    screen.hline(transform.getx() + 1, transform.getx() + size.getx() - 1,
                 transform.gety(), '=');
    transform += {0, 1};

    write_msg->draw(engine, transform, {size.getx(), 2}, screen);

    transform -= {0, 3};
    for (const auto &msg : state->get_messages_of_current_chat()) {
        const auto sent_by = state->find_user(msg.sent_by);
        const auto name = sent_by ? sent_by->get().name : "unknown";
        screen.print(transform, " > {}: {}", name, msg.content);
        transform -= {0, 2};

        // Stop when we run out of space
        if (transform.gety() <= limit.gety())
            break;
    }
}

void ChatScene::mount(eng::Engine &engine) {
    chat_info->mount(engine);
    write_msg->mount(engine);
}

void ChatScene::unmount(eng::Engine &engine) {
    chat_info->unmount(engine);
    write_msg->unmount(engine);
}
} // namespace uppr::app
