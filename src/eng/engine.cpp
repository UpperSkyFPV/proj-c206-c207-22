#include "engine.hpp"
#include <chrono>
#include <thread>

namespace uppr::eng {

void Engine::run() {
    using namespace std::chrono;

    // First things first, reset the terminal state
    term->clear_term();

    while (should_run()) {
        const auto start = steady_clock::now();

        term->clear_term();

        if (current_scene) {
            current_scene->update(*this);
        } else {
            // Stop if we ever have no scene, as this is an error
            LOG_F(FATAL, "No scene in engine!");
        }

        const auto end = steady_clock::now();
       frame_time = duration_cast<microseconds>(end - start).count();
        if (frame_time < period_millis)
            std::this_thread::sleep_for(microseconds{period_millis - frame_time});
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
