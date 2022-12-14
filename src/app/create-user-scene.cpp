#include "create-user-scene.hpp"
#include "event.hpp"
#include "fmt/color.h"
#include "utils/draw_input_box.hpp"
#include "vector2.hpp"

#include <charconv>
#include <string_view>

namespace uppr::app {

void CreateUserScene::draw(eng::Engine &engine, term::Transform transform,
                           term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    // Draw a border box
    screen.box(transform, size.getx(), size.gety(), {});
    screen.print(transform.move(2, 0), "Create New User ({} saved)",
                 state->get_users().size());
    transform += {2, 1};
    size -= 1;

    const auto initial = transform;

    transform = draw_input_box(Item::username, selected_item, transform, screen,
                               "Username", create_data.username);
    transform = draw_input_box(Item::addr_host, selected_item, transform,
                               screen, "Address Host", create_data.addr_host);
    transform = draw_input_box(Item::addr_port, selected_item, transform,
                               screen, "Address Port", create_data.addr_port);

    transform += {0, 1};
    transform =
        draw_basic_button(Item::confirm, selected_item, create_data.any_empty(),
                          transform, screen, "Create");

    if (wants_input) { confirm(initial, screen); }
}

void CreateUserScene::mount(eng::Engine &engine) {
    state->fetch_users();

    edit_item_keybind_handle =
        engine.get_eventbus().appendListener('\r', [this](char c) {
            wants_input = true;
            LOG_F(9, "wants input");
        });

    select_next_keybind_handle = engine.get_eventbus().appendListener(
        '\t', [this](char c) { select_next_item(); });

    select_prev_keybind_handle = engine.get_eventbus().appendListener(
        eng::Event::NonChar::shift_tab, [this](char c) { select_prev_item(); });
}

void CreateUserScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener('\r', edit_item_keybind_handle);
    engine.get_eventbus().removeListener('\t', select_next_keybind_handle);
    engine.get_eventbus().removeListener(eng::Event::NonChar::shift_tab,
                                         select_prev_keybind_handle);
}

void CreateUserScene::select_next_item() {
    LOG_F(8, "create-user-scene select_next_item");
    switch (selected_item) {
    case Item::username: selected_item = Item::addr_host; break;
    case Item::addr_host: selected_item = Item::addr_port; break;
    case Item::addr_port: {
        if (create_data.any_empty())
            selected_item = Item::username;
        else
            selected_item = Item::confirm;
    } break;
    case Item::confirm: selected_item = Item::username; break;
    }
}

void CreateUserScene::select_prev_item() {
    LOG_F(8, "create-user-scene select_prev_item");
    switch (selected_item) {
    case Item::username: {
        if (create_data.any_empty())
            selected_item = Item::addr_port;
        else
            selected_item = Item::confirm;
    } break;
    case Item::addr_host: selected_item = Item::username; break;
    case Item::addr_port: selected_item = Item::addr_host; break;
    case Item::confirm: selected_item = Item::addr_port; break;
    }
}

void CreateUserScene::confirm(term::Transform transform,
                              term::TermScreen &screen) {
    switch (selected_item) {
    case Item::username: {
        create_data.username.clear();
        const auto data = screen.inputline(transform.move(1, 1), 28);
        create_data.username = data;
    } break;
    case Item::addr_host: {
        create_data.addr_host.clear();
        const auto data = screen.inputline(transform.move(1, 4), 28);
        create_data.addr_host = data;
    } break;
    case Item::addr_port: {
        create_data.addr_port.clear();
        const auto data = screen.inputline(transform.move(1, 7), 28);
        create_data.addr_port = data;
    } break;
    case Item::confirm: {
        LOG_F(INFO, "Saving! '{}', '{}', '{}'", create_data.username,
              create_data.addr_host, create_data.addr_port);

        state->insert_new_user({-1, create_data.username, -1},
                               {-1, create_data.addr_host,
                                std::atoi(create_data.addr_port.c_str())});
        create_data.reset();
    } break;
    }

    select_next_item();
    wants_input = false;
}
} // namespace uppr::app
