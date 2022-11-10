#include "event.hpp"
#include "fmt/color.h"
#include "key.hpp"
#include "select-view.hpp"

namespace uppr::app {

void SelectViewScene::update(eng::Engine &engine) {
    create_user_modal->update(engine);
    create_chat_modal->update(engine);
    add_user_to_chat_modal->update(engine);
}

void SelectViewScene::draw(eng::Engine &engine, term::Transform transform,
                           term::Size size, term::TermScreen &screen) {
    create_user_modal->draw(engine, transform, size, screen);
    create_chat_modal->draw(engine, transform, size, screen);
    add_user_to_chat_modal->draw(engine, transform, size, screen);

    draw_bottom_panel(engine, transform, size, screen);
}

void SelectViewScene::mount(eng::Engine &engine) {
    const auto open_user_listener = [this, &engine](char c) {
        // ignore if the other is already up
        if (any_is_open()) return;

        create_user_modal->show_modal(engine);
        add_user_close_handler(engine);
    };

    const auto open_chat_listener = [this, &engine](char c) {
        // ignore if the other is already up
        if (any_is_open()) return;

        create_chat_modal->show_modal(engine);
        add_chat_close_handler(engine);
    };

    const auto open_adduser_listener = [this, &engine](char c) {
        // ignore if the other is already up
        if (any_is_open() || !state->has_chat_selected()) return;

        add_user_to_chat_modal->show_modal(engine);
        add_adduser_close_handler(engine);
    };

    // Event handler for the `ctrl+n` key
    open_create_user_keybind_handle = engine.get_eventbus().appendListener(
        term::ctrl('u'), open_user_listener);
    open_create_chat_keybind_handle = engine.get_eventbus().appendListener(
        term::ctrl('c'), open_chat_listener);
    open_create_chat_keybind_handle = engine.get_eventbus().appendListener(
        term::ctrl('a'), open_adduser_listener);
}

void SelectViewScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener(term::ctrl('u'),
                                         open_create_user_keybind_handle);

    if (close_any_modal_keybind_handle) remove_close_handler(engine);
}

void SelectViewScene::draw_bottom_panel(eng::Engine &engine,
                                        term::Transform transform,
                                        term::Size size,
                                        term::TermScreen &screen) const {
    using namespace fmt;

    transform += {0, static_cast<int>(size.gety() - 1)};

    const auto style = bg(color::dark_gray) | fg(color::black);

    screen.hline(0, size.getx(), transform.gety(), {' ', style});

    constexpr auto create_user_help = "<ctrl+u> Create User |"sv;
    screen.print(transform, style, create_user_help);
    transform += {1 + create_user_help.size(), 0};

    constexpr auto create_chat_help = "<ctrl+c> Create Chat |"sv;
    screen.print(transform, style, create_chat_help);
    transform += {1 + create_chat_help.size(), 0};
}

void SelectViewScene::add_user_close_handler(eng::Engine &engine) {
    // Avoid leaking handlers
    if (close_any_modal_keybind_handle) remove_close_handler(engine);

    // When <ESC> is pressed, we close the create user modal
    close_any_modal_keybind_handle = engine.get_eventbus().appendListener(
        eng::Event::NonChar::esc, [this, &engine](char c) {
            LOG_F(8, "closing create user");

            create_user_modal->hide_modal(engine);
            remove_close_handler(engine);
        });
}

void SelectViewScene::add_chat_close_handler(eng::Engine &engine) {
    // Avoid leaking handlers
    if (close_any_modal_keybind_handle) remove_close_handler(engine);

    // When <ESC> is pressed, we close the create user modal
    close_any_modal_keybind_handle = engine.get_eventbus().appendListener(
        eng::Event::NonChar::esc, [this, &engine](char c) {
            LOG_F(8, "closing create chat");

            create_chat_modal->hide_modal(engine);
            remove_close_handler(engine);
        });
}

void SelectViewScene::add_adduser_close_handler(eng::Engine &engine) {
    // Avoid leaking handlers
    if (close_any_modal_keybind_handle) remove_close_handler(engine);

    // When <ESC> is pressed, we close the create user modal
    close_any_modal_keybind_handle = engine.get_eventbus().appendListener(
        eng::Event::NonChar::esc, [this, &engine](char c) {
            LOG_F(8, "closing add user to chat");

            add_user_to_chat_modal->hide_modal(engine);
            remove_close_handler(engine);
        });
}

void SelectViewScene::remove_close_handler(eng::Engine &engine) {
    engine.get_eventbus().removeListener(eng::Event::NonChar::esc,
                                         close_any_modal_keybind_handle);
}
} // namespace uppr::app
