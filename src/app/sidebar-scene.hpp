#pragma once

#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "key.hpp"
#include "vector2.hpp"

namespace uppr::app {

class SidebarScene : public eng::Scene {
public:
    SidebarScene(std::shared_ptr<eng::Scene> c) : content{c} {}

    void update(eng::Engine &engine) override { content->update(engine); }

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

    static std::shared_ptr<SidebarScene>
    make(std::shared_ptr<eng::Scene> content) {
        return std::make_shared<SidebarScene>(content);
    }

    void mount(eng::Engine &engine) override {
        j_handle = engine.get_eventbus().appendListener(
            'n', [this](char c) { show_sidebar = !show_sidebar; });

        content->mount(engine);
    }

    void unmount(eng::Engine &engine) override {
        engine.get_eventbus().removeListener('j', j_handle);

        content->unmount(engine);
    }

private:
    void draw_list(eng::Engine &engine, term::Transform transform,
                   term::Size size, term::TermScreen &screen) {
        std::vector<std::string> items{
            "item a",
            "item b",
            "item c",
        };

        auto t = transform.move(1, 0);
        for (const auto &item : items) {
            screen.print(t, "item: {}", item);
            t += term::Transform{0, 2};
            screen.hline(t.getx(), size.getx(), t.gety(), '-');

            t += term::Transform{0, 1};
        }
    }

private:
    std::shared_ptr<eng::Scene> content;

    eng::Engine::EventBus::Handle j_handle;

    bool show_sidebar;
};
} // namespace uppr::app
