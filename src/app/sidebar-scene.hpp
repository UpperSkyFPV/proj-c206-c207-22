#pragma once

#include "chatview-scene.hpp"
#include "conn.hpp"
#include "dao/chat.hpp"
#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "fmt/color.h"
#include "key.hpp"
#include "models/chat.hpp"
#include "state.hpp"
#include "stmt.hpp"
#include "vector2.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace uppr::app {

/**
 * This scene is the sidebar on the left of the viewport.
 *
 * In this scene we show all of the chats owned by the user (which means that we
 * have _read access_ to the database `Chat` table). It is possible to hide the
 * sidebar completally by using the `ctrl+n` keybind.
 *
 * Selecting each of the chats is done using the `c` key and cycle all of them.
 */
class SidebarScene : public eng::Scene {
public:
    /**
     * We need a scene to live on the right of us, and a connection to the
     * database.
     */
    SidebarScene(std::shared_ptr<eng::Scene> c, shared_ptr<AppState> s)
        : content{c}, chatview{std::make_unique<ChatViewScene>(s)}, state{s} {}

    void update(eng::Engine &engine) override {
        chatview->update(engine);
        content->update(engine);
    }

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        // The sidebar has the size of 1/3 of the screen width
        const auto width = show_sidebar ? size.getx() / 3 : 0;
        if (show_sidebar) {
            chatview->draw(engine, transform, {width, size.gety()}, screen);

            screen.vline(width, 0, size.gety(), '|');
        }

        content->draw(engine, transform.move(width + 1, 0),
                      size - term::Size{width - 1, 0}, screen);
    }

    void mount(eng::Engine &engine) override {
        // Event handler for the `ctrl+n` key
        hide_sidebar_keybind_handle = engine.get_eventbus().appendListener(
            term::ctrl('n'), [this](char c) { show_sidebar = !show_sidebar; });

        chatview->mount(engine);
        content->mount(engine);
    }

    void unmount(eng::Engine &engine) override {
        // Remove the event handler for the `ctrl+n` key
        engine.get_eventbus().removeListener(term::ctrl('n'),
                                             hide_sidebar_keybind_handle);

        chatview->unmount(engine);
        content->unmount(engine);
    }

    static std::shared_ptr<SidebarScene> make(std::shared_ptr<eng::Scene> c,
                                              shared_ptr<AppState> s) {
        return std::make_shared<SidebarScene>(c, s);
    }

private:
    std::shared_ptr<eng::Scene> content;
    std::unique_ptr<ChatViewScene> chatview;

    eng::Engine::EventBus::Handle hide_sidebar_keybind_handle;

    shared_ptr<AppState> state;

    bool show_sidebar{true};
};
} // namespace uppr::app
