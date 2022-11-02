#pragma once

#include "conn.hpp"
#include "dao/chat.hpp"
#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "fmt/color.h"
#include "key.hpp"
#include "models/chat.hpp"
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
    SidebarScene(std::shared_ptr<eng::Scene> c,
                 std::shared_ptr<db::Connection> db_)
        : chat_dao{db_}, content{c} {}

    void update(eng::Engine &engine) override {
        // Update the chat if needed
        if (need_to_get_chats_from_db) update_chat();

        content->update(engine);
    }

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        // The sidebar has the size of 1/3 of the screen width
        const auto width = show_sidebar ? size.getx() / 3 : size.getx();
        if (show_sidebar) {
            draw_list(engine, transform, {width, size.gety()}, screen);

            screen.vline(width, 0, size.gety(), '|');
        }

        content->draw(engine, transform.move(width + 1, 0),
                      size - term::Size{width - 1, 0}, screen);
    }

    void mount(eng::Engine &engine) override {
        // Event handler for the `ctrl+n` key
        hide_sidebar_keybind_handle = engine.get_eventbus().appendListener(
            term::ctrl('n'), [this](char c) { show_sidebar = !show_sidebar; });

        cycle_chat_keybind_handle =
            engine.get_eventbus().appendListener('c', [this](char c) {
                selected_chat = (selected_chat + 1) % chats.size();
                LOG_F(7, "selected_chat={}", selected_chat);
            });

        content->mount(engine);
    }

    void unmount(eng::Engine &engine) override {
        // Remove the event handler for the `ctrl+n` key
        engine.get_eventbus().removeListener(term::ctrl('n'),
                                             hide_sidebar_keybind_handle);
        engine.get_eventbus().removeListener('c', cycle_chat_keybind_handle);

        content->unmount(engine);
    }

    static std::shared_ptr<SidebarScene>
    make(std::shared_ptr<eng::Scene> content,
         std::shared_ptr<db::Connection> db) {
        return std::make_shared<SidebarScene>(content, db);
    }

private:
    /**
     * Draw the list of chats.
     */
    void draw_list(eng::Engine &engine, term::Transform transform,
                   term::Size size, term::TermScreen &screen) {
        using namespace fmt;

        auto t = transform.move(1, 0);
        usize idx{};
        for (const auto &item : chats) {
            // Print the title of the chat (its name) in bold
            auto name_style = fg(color::white) | emphasis::bold;
            // If the current chat is the selected one, then mark it in some
            // visible manner
            if (idx == selected_chat) { name_style |= emphasis::reverse; }
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

    void update_chat() {
        chats = chat_dao.all();

        need_to_get_chats_from_db = false;
    }

private:
    dao::ChatDAO chat_dao;

    std::shared_ptr<eng::Scene> content;
    eng::Engine::EventBus::Handle hide_sidebar_keybind_handle;
    eng::Engine::EventBus::Handle cycle_chat_keybind_handle;

    std::vector<models::ChatModel> chats;
    usize selected_chat{};

    bool need_to_get_chats_from_db{true};

    bool show_sidebar{true};
};
} // namespace uppr::app
