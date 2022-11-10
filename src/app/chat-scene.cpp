#include "chat-scene.hpp"
#include "key.hpp"

namespace uppr::app {

void ChatScene::update(eng::Engine &engine) { chat_info->update(engine); }

void ChatScene::draw(eng::Engine &engine, term::Transform transform,
                     term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    const auto info_size = term::Size{size.getx(), size.gety() / 5};

    chat_info->draw(engine, transform, info_size, screen);
    transform += {0, static_cast<int>(info_size.gety())};

    screen.hline(transform.getx() + 1, transform.getx() + size.getx() - 1,
                 transform.gety(), '=');
}

void ChatScene::mount(eng::Engine &engine) {
    chat_info->mount(engine);
}

void ChatScene::unmount(eng::Engine &engine) {
    chat_info->unmount(engine);
}
} // namespace uppr::app
