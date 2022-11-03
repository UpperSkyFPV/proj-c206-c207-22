#include "engine.hpp"
#include "key.hpp"
#include <bits/chrono.h>
#include <chrono>
#include <thread>

namespace uppr::eng {

void Engine::run() {
    using namespace std::chrono;

    // First things first, reset the terminal state
    // screen->clear_term();

    while (should_run()) {
        // Record when the frame was started
        const auto start = steady_clock::now();

        poll_events();

        screen->clear();

        if (current_scene) {
            // Run updates on the current scene
            current_scene->update(*this);

            const auto update_end = steady_clock::now();
            update_time =
                duration_cast<microseconds>(update_end - start).count();

            // And then draw
            current_scene->draw(*this, {}, screen->get_size(), *screen);

            const auto draw_end = steady_clock::now();
            draw_time =
                duration_cast<microseconds>(draw_end - update_end).count();
        } else {
            // Stop if we ever have no scene, as this is an error
            LOG_F(FATAL, "No scene in engine!");
        }

        const auto end_scene = steady_clock::now();

        // Actually commit the screen pixels to the terminal
        screen->commit();

        const auto end = steady_clock::now();
        frame_time = duration_cast<microseconds>(end - start).count();
        commit_time = duration_cast<microseconds>(end - end_scene).count();

        // Dont go too fast
        if (frame_time < period_millis)
            std::this_thread::sleep_for(
                microseconds{period_millis - frame_time});
    }
}

void Engine::switch_scene(std::shared_ptr<Scene> s) {
    // Call unmount hook before removing
    if (current_scene) current_scene->unmount(*this);

    current_scene = s;

    // Call mount hook after adding
    if (current_scene) current_scene->mount(*this);
}

void Engine::poll_events() {
    while (true) {
        const auto c = screen->readc();
        if (c == 0) break;

        // Ctrl+Q is quit, always
        if (term::is_ctrl(c, 'q')) finalize();
        if (term::is_escseq(c)) {
            LOG_F(9, "received escape seq");

            screen->set_termios_control(1, 0);
            screen->commit_termios(false);

            std::array<char, 8> buf;
            const auto sequence = screen->read(buf);
            screen->set_termios_control(0, 0);
            screen->commit_termios(false);

            // The ESC key
            if (term::is_esc(sequence)) {
                LOG_F(9, "received key: 'ESC'");
                eventbus.dispatch(Event::NonChar::esc, Event::NonChar::esc);
            } else if (term::is_shif_tab(sequence)) {
                LOG_F(9, "received key: 'shift+tab'");
                eventbus.dispatch(Event::NonChar::shift_tab,
                                  Event::NonChar::shift_tab);
            } else {
                LOG_F(8, "received unknown escape sequence: '{}'", sequence);
            }
        }

        LOG_F(9, "received key: '{}' ({:d}, {:d})", c, c, term::ctrl(c));

        eventbus.dispatch(c, c);
    }
}
} // namespace uppr::eng
