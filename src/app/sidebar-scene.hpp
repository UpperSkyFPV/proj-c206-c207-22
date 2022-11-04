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

    void update(eng::Engine &engine) override;

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

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
