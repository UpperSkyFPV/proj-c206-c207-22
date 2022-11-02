#pragma once

#include "eventpp/eventdispatcher.h"
#include "scene.hpp"
#include "screen.hpp"
#include "vector2.hpp"

namespace uppr::eng {

/**
 * Basic resource and frame management.
 *
 * Execution of a frame is divided as follows:
 * ```
 *     update scene  <-+------------------+
 *                     |                  |
 *                     | update time      |
 *                     |                  |
 *                   <-+                  |
 *       draw scene                       | frame time
 *                   <---+                |
 *                       |                |
 *                       | draw time      |
 *                       |                |
 *                   <---+                |
 * commit to screen                       |
 *                   <------+             |
 *                          |             |
 *                          | commit time |
 *                          |             |
 * wait excess time  <------+-------------+
 *
 *
 *
 * ```
 */
class Engine {
public:
    using EventBus = eventpp::EventDispatcher<char, void(char)>;

    Engine(int fps_, std::shared_ptr<term::TermScreen> t)
        : screen{t}, fps{fps_}, period_millis{max_frame_time()} {}
    Engine(int fps_, std::shared_ptr<term::TermScreen> t,
           std::shared_ptr<Scene> s)
        : screen{t}, fps{fps_}, period_millis{max_frame_time()} {
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
     * Get the size of the screen.
     */
    constexpr term::Size get_screen_size() const { return screen->get_size(); }

    /**
     * Get the time that the last frame took.
     */
    constexpr int get_frame_time() const { return frame_time; }

    /**
     * Get the time taken to update the logic.
     */
    constexpr int get_update_time() const { return update_time; }

    /**
     * get the time taken to draw to the pixel buffer.
     */
    constexpr int get_draw_time() const { return draw_time; }

    /**
     * get the time taken to commit the pixel buffer to the terminal
     */
    constexpr int get_commit_time() const { return commit_time; }

    /**
     * Get the maximum time budget of a frame.
     */
    constexpr int get_max_frame_time() const { return period_millis; }

    /**
     * Read a single character from the screen.
     */
    char readc() const { return screen->readc(); }

    /**
     * Get the event bus in order to add and remove listeners.
     */
    EventBus &get_eventbus() { return eventbus; }

private:
    /**
     * Get the maximum frame time in microseconds.
     */
    constexpr int max_frame_time() const { return 1000000 / fps; }

    /**
     * Poll the stdin for keys pressed and transform it to events.
     */
    void poll_events();

private:
    /**
     * This is the active scene.
     */
    std::shared_ptr<Scene> current_scene{};

    /**
     * The terminal driver/wrapper thing instance.
     */
    std::shared_ptr<term::TermScreen> screen;

    /**
     * Event queue for events (duh)
     */
    EventBus eventbus;

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
    int frame_time{};

    /**
     * The last update time.
     */
    int update_time{};

    /**
     * The last draw time.
     */
    int draw_time{};

    /**
     * The last time during commiting to terminal.
     */
    int commit_time{};

    /**
     * If the engine is currently running. If this goes false, then the mainloop
     * exits.
     */
    bool running{true};
};
} // namespace uppr::eng
