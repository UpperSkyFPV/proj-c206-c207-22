#include "modal-scene.hpp"

namespace uppr::eng {

void ModalScene::update(Engine &engine) {
    if (modal && should_show_modal) modal->update(engine);
}

void ModalScene::draw(Engine &engine, term::Transform transform,
                      term::Size size, term::TermScreen &screen) {
    if (modal && should_show_modal) {
        transform += origin;

        const auto one_third = size.getx() / 3;
        const auto one_quarter = size.gety() / 4;
        const auto on_half = one_quarter * 2;

        const term::Transform newt{static_cast<int>(one_third),
                                   static_cast<int>(one_quarter)};
        const term::Size news{one_third, on_half};

        modal->draw(engine, newt, news, screen);
    }
}

void ModalScene::mount(Engine &engine) {
    if (should_show_modal && !modal_mounted) {
        modal->mount(engine);
        modal_mounted = true;
    }
}

void ModalScene::unmount(Engine &engine) {
    if (should_show_modal && modal_mounted) {
        modal->unmount(engine);
        modal_mounted = false;
    }
}

void ModalScene::show_modal(Engine &engine) {
    if (!should_show_modal && !modal_mounted) {
        modal->mount(engine);
        modal_mounted = true;
    }

    should_show_modal = true;
}

void ModalScene::hide_modal(Engine &engine) {
    if (should_show_modal && modal_mounted) {
        modal->unmount(engine);
        modal_mounted = false;
    }

    should_show_modal = false;
}
} // namespace uppr::eng
