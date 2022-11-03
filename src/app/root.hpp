#pragma once

#include "screen.hpp"
#include <memory>
namespace uppr::app {

/**
 * Launch the app!
 */
int start_app(shared_ptr<term::TermScreen> term_screen);
} // namespace uppr::app
