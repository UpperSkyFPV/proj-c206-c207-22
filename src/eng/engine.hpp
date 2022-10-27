#pragma once

#include "scene.hpp"
#include "term.hpp"

namespace uppr::eng {

class Engine {
public:
    Engine(int fps_, std::shared_ptr<term::Term> t)
        : term{t}, fps{fps_}, period_millis{max_frame_time()} {}
    Engine(int fps_, std::shared_ptr<term::Term> t, std::shared_ptr<Scene> s)
        : term{t}, fps{fps_}, period_millis{max_frame_time()} {
        switch_scene(s);
    }

    /**
     * Run the engine.
     *
     * This enters a loop that runs until the engine is `finalize()`d.
     */
    void run();

    /**
     * Change scenes.
     */
    void switch_scene(std::shared_ptr<Scene> s);

    /**
     * If the engine is running.
     */
    constexpr bool should_run() const { return running; }

    /**
     * Stop the engine.
     */
    constexpr void finalize() { running = false; }

    /**
     * Get the terminal driver thing.
     */
    std::shared_ptr<term::Term> get_term() const { return term; }

    constexpr int get_frame_time() const { return frame_time; }
    constexpr int get_max_frame_time() const { return period_millis; }

private:
    /**
     * Get the maximum frame time in microseconds.
     */
    constexpr int max_frame_time() const { return 1000000 / fps; }

private:
    /**
     * This is the active scene.
     */
    std::shared_ptr<Scene> current_scene{};

    /**
     * The terminal driver/wrapper thing instance.
     */
    std::shared_ptr<term::Term> term;

    /**
     * What FPS to run the engine at (or at least try to).
     */
    int fps;

    /**
     * The period of a frame, in microseconds.
     */
    int period_millis;

    /**
     * The last frame time.
     */
    int frame_time;

    /**
     * If the engine is currently running. If this goes false, then the mainloop
     * exits.
     */
    bool running{true};
};
} // namespace uppr::eng
