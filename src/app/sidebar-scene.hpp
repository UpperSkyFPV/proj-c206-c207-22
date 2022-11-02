#pragma once

#include "conn.hpp"
#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "fmt/color.h"
#include "key.hpp"
#include "models/chat.hpp"
#include "stmt.hpp"
#include "vector2.hpp"
#include <string_view>
#include <vector>

namespace uppr::app {

class SidebarScene : public eng::Scene {
public:
    SidebarScene(std::shared_ptr<eng::Scene> c,
                 std::shared_ptr<db::Connection> db_)
        : db{db_}, content{c} {}

    void update(eng::Engine &engine) override {
        if (need_to_get_chats_from_db) {
            db->execute_many("SELECT * FROM Chat;", [this](
                                                        const db::PreparedStmt
                                                            &stmt) {
                const auto n = stmt.column_count();
                const auto id = stmt.column_int(0);
                const auto name = stmt.column_text(1);
                const auto descr = stmt.column_text(2);

                chats.push_back({id, std::string{name}, std::string{descr}});

                LOG_F(INFO, "stmt[{}]@{}{{ {}, {}, {} }}", n, fmt::ptr(&stmt),
                      id, name, descr);
            });

            need_to_get_chats_from_db = false;
        }

        content->update(engine);
    }

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        const auto width = show_sidebar ? size.getx() / 3 : size.getx();
        if (show_sidebar) {
            draw_list(engine, transform, {width, size.gety()}, screen);

            screen.vline(width, 0, size.gety(), '|');
        }

        content->draw(engine, transform.move(width + 1, 0),
                      size - term::Size{width - 1, 0}, screen);
    }

    void mount(eng::Engine &engine) override {
        j_handle = engine.get_eventbus().appendListener(
            term::ctrl('n'), [this](char c) { show_sidebar = !show_sidebar; });

        content->mount(engine);
    }

    void unmount(eng::Engine &engine) override {
        engine.get_eventbus().removeListener(term::ctrl('c'), j_handle);

        content->unmount(engine);
    }

    static std::shared_ptr<SidebarScene>
    make(std::shared_ptr<eng::Scene> content,
         std::shared_ptr<db::Connection> db) {
        return std::make_shared<SidebarScene>(content, db);
    }

private:
    void draw_list(eng::Engine &engine, term::Transform transform,
                   term::Size size, term::TermScreen &screen) {
        using namespace fmt;

        auto t = transform.move(1, 0);
        for (const auto &item : chats) {
            // Print the title of the chat (its name) in bold
            auto name_style = fg(color::white) | emphasis::bold;
            // If the current chat is the selected one, then mark it in some
            // visible manner
            if (item.id == 1) { name_style |= emphasis::reverse; }
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
        }
    }

private:
    std::shared_ptr<db::Connection> db;

    std::shared_ptr<eng::Scene> content;
    eng::Engine::EventBus::Handle j_handle;

    std::vector<models::ChatModel> chats;

    bool need_to_get_chats_from_db{true};

    bool show_sidebar{true};
};
} // namespace uppr::app
