#pragma once

#include "commom.hpp"
#include "screen.hpp"
#include "term.hpp"

namespace uppr::eng {

class Engine;

/**
 * This is the basis for creating "widgets".
 */
class Scene {
public:
    virtual ~Scene() {}

    /**
     * Function to run the current frame.
     */
    virtual void update(Engine& engine) = 0;

    /**
     * Function to draw the current frame.
     */
    virtual void draw(Engine& engine, term::TermScreen& screen) = 0;

    /**
     * Called when the scene becomes the active scene in the engine.
     */
    virtual void mount(Engine& engine) {}

    /**
     * Called when the scene is removed from beeing the active scene in the engine.
     *
     * Note that this is not called if the engine is stopped.
     */
    virtual void unmount(Engine& engine) {}

private:
    /**
     * We reference the terminal driver.
     */
    std::shared_ptr<term::Term> term;
};
}