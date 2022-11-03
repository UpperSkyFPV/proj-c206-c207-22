#include "create-user-scene.hpp"
#include "fmt/color.h"
#include "vector2.hpp"
#include <string_view>

namespace uppr::app {

void CreateUserScene::draw(eng::Engine &engine, term::Transform transform,
                           term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    // Draw a border box
    screen.box(transform, size.getx(), size.gety(), {});
    transform += {2, 1};
    size -= 1;

    const auto initial = transform;

    transform = draw_input(Item::username, transform, screen, "Username",
                           create_data.username);
    transform = draw_input(Item::addr_host, transform, screen, "Address Host",
                           create_data.addr_host);
    transform = draw_input(Item::addr_port, transform, screen, "Address Port",
                           create_data.addr_port);

    if (selected_item == Item::confirm)
        screen.print(transform, emphasis::reverse, "Create");
    else if (create_data.any_empty())
        screen.print(transform, emphasis::faint, "Create");
    else
        screen.print(transform, "Create");
    transform += {0, 1};

    if (wants_input) { confirm(initial, screen); }
}

void CreateUserScene::mount(eng::Engine &engine) {
    edit_item_keybind_handle =
        engine.get_eventbus().appendListener('\r', [this](char c) {
            wants_input = true;
            LOG_F(9, "wants input");
        });

    select_next_keybind_handle = engine.get_eventbus().appendListener(
        '\t', [this](char c) { select_next_item(); });
}

void CreateUserScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener('\r', edit_item_keybind_handle);
    engine.get_eventbus().removeListener('\t', select_next_keybind_handle);
}

term::Transform CreateUserScene::draw_input(Item item_kind,
                                            term::Transform transform,
                                            term::TermScreen &screen,
                                            string_view title,
                                            string_view contents) {
    using namespace fmt;

    if (selected_item == item_kind)
        screen.print(transform, emphasis::reverse, "{}", title);
    else
        screen.print(transform, "{}", title);
    transform += {0, 1};
    screen.box(transform, 30, 2, {});

    screen.print(transform.move(1, 1), emphasis::underline, "{}", contents);

    transform += {0, 3};

    return transform;
}

void CreateUserScene::confirm(term::Transform transform,
                              term::TermScreen &screen) {
    switch (selected_item) {
    case Item::username: {
        const auto data = screen.inputline(transform.move(1, 2), 28);
        create_data.username = data;
    } break;
    case Item::addr_host: {
        const auto data = screen.inputline(transform.move(1, 6), 28);
        create_data.addr_host = data;
    } break;
    case Item::addr_port: {
        const auto data = screen.inputline(transform.move(1, 10), 28);
        create_data.addr_port = data;
    } break;
    case Item::confirm: {
        LOG_F(INFO, "Confirming!");
        create_data.reset();
    } break;
    }

    select_next_item();

    LOG_F(INFO, "Reading things! '{}', '{}', '{}'", create_data.username,
          create_data.addr_host, create_data.addr_port);

    wants_input = false;
}
} // namespace uppr::app
