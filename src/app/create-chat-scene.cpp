#include "create-chat-scene.hpp"
#include "utils/draw_input_box.hpp"

namespace uppr::app {
void CreateChatScene::draw(eng::Engine &engine, term::Transform transform,
                           term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    // Draw a border box
    screen.box(transform, size.getx(), size.gety(), {});
    screen.print(transform.move(2, 0), "Create New Chat ({} saved)",
                 state->get_chats().size());
    transform += {2, 1};
    size -= 1;

    const auto initial = transform;

    transform = draw_input_box(Item::name, selected_item, transform, screen,
                               "Name", create_data.name);
    transform = draw_input_box(Item::description, selected_item, transform,
                               screen, "Description", create_data.description);

    transform += {0, 1};
    transform =
        draw_basic_button(Item::confirm, selected_item, create_data.any_empty(),
                          transform, screen, "Create");

    if (wants_input) { confirm(initial, screen); }
}

void CreateChatScene::mount(eng::Engine &engine) {
    state->fetch_chats();

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

void CreateChatScene::unmount(eng::Engine &engine) {}

void CreateChatScene::select_next_item() {
    switch (selected_item) {
    case Item::name: selected_item = Item::description; break;
    case Item::description: {
        if (create_data.any_empty())
            selected_item = Item::name;
        else
            selected_item = Item::confirm;
    } break;
    case Item::confirm: selected_item = Item::name; break;
    }
}

void CreateChatScene::select_prev_item() {
    switch (selected_item) {
    case Item::name: {
        if (create_data.any_empty())
            selected_item = Item::description;
        else
            selected_item = Item::confirm;
    } break;
    case Item::description: selected_item = Item::name; break;
    case Item::confirm: selected_item = Item::description; break;
    }
}

void CreateChatScene::confirm(term::Transform transform,
                              term::TermScreen &screen) {
    switch (selected_item) {
    case Item::name: {
        create_data.name.clear();
        const auto data = screen.inputline(transform.move(1, 1), 28);
        create_data.name = data;
    } break;
    case Item::description: {
        create_data.description.clear();
        const auto data = screen.inputline(transform.move(1, 4), 28);
        create_data.description = data;
    } break;
    case Item::confirm: {
        LOG_F(INFO, "Saving! '{}', '{}'", create_data.name,
              create_data.description);

        state->insert_new_chat({-1, create_data.name, create_data.description});
        create_data.reset();
    } break;
    }

    select_next_item();
    wants_input = false;
}
} // namespace uppr::app
