#include "sidebar-scene.hpp"

namespace uppr::app {

void SidebarScene::update(eng::Engine &engine) {
    chatview->update(engine);
    content->update(engine);
}

void SidebarScene::draw(eng::Engine &engine, term::Transform transform,
                        term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    // The sidebar has the size of 1/3 of the screen width
    const auto width = show_sidebar ? size.getx() / 3 : 0;
    if (show_sidebar) {
        chatview->draw(engine, transform, {width, size.gety()}, screen);

        screen.vline(width, 0, size.gety(), '|');
    }

    content->draw(engine, transform.move(width + show_sidebar, 0),
                  size - term::Size{width - show_sidebar, 0}, screen);
}

void SidebarScene::mount(eng::Engine &engine) {
    hide_sidebar_keybind_handle = engine.get_eventbus().appendListener(
        term::ctrl('e'), [this](char c) { show_sidebar = !show_sidebar; });

    chatview->mount(engine);
    content->mount(engine);
}

void SidebarScene::unmount(eng::Engine &engine) {
    // Remove the event handler for the `ctrl+n` key
    engine.get_eventbus().removeListener(term::ctrl('e'),
                                         hide_sidebar_keybind_handle);

    chatview->unmount(engine);
    content->unmount(engine);
}
} // namespace uppr::app
