#include "engine.hpp"
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

        if (current_scene) {
            // Run updates on the current scene
            current_scene->update(*this);

            const auto update_end = steady_clock::now();
            update_time =
                duration_cast<microseconds>(update_end - start).count();

            // And then draw
            current_scene->draw(*this, *screen);

            const auto draw_end = steady_clock::now();
            draw_time =
                duration_cast<microseconds>(draw_end - update_end).count();
        } else {
            // Stop if we ever have no scene, as this is an error
            LOG_F(FATAL, "No scene in engine!");
        }
 
        const auto end_scene = steady_clock::now();
        scene_time = duration_cast<microseconds>(end_scene - start).count();

        // Actually commit the screen pixels to the terminal
        screen->commit();

        const auto end = steady_clock::now();
        frame_time = duration_cast<microseconds>(end - start).count();
        scene_time = duration_cast<microseconds>(end - end_scene).count();

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
} // namespace uppr::eng
