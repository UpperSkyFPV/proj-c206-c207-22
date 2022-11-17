#include "writemsg-scene.hpp"
#include "key.hpp"

namespace uppr::app {
void WriteMsgScene::draw(eng::Engine &engine, term::Transform transform,
                         term::Size size, term::TermScreen &screen) {
    transform += {1, 0};
    screen.print(transform, ">");
    transform += {2, 0};

    if (wants_input) {
        const auto data = screen.inputline(transform, 45);

        state->push_message(data);

        wants_input = false;
    }
}

void WriteMsgScene::mount(eng::Engine &engine) {
    start_writing_keybind_handle =
        engine.get_eventbus().appendListener('m', [this](char) {
            if (!state->has_chat_selected()) return;

            wants_input = !wants_input;
            LOG_F(9, "wants input");
        });
}

void WriteMsgScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener('m', start_writing_keybind_handle);
}
} // namespace uppr::app
