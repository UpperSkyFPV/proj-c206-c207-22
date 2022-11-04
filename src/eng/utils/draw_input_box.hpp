#pragma once

#include "screen.hpp"
#include "vector2.hpp"
namespace uppr::app {

/**
 * Draw one input field.
 *
 * @param input_kind is used to decide if the input title should be highlighted
 * @param selected_kind is the type of the selected input (the one that will be
 * highlighted)
 * @param transform Where to place the field.
 * @param screen reference to the screen object
 * @param title The title to give to the input widget
 * @param contents What to render inside the input
 */
template <typename T>
term::Transform draw_input_box(T item_kind, T selected_kind,
                               term::Transform transform,
                               term::TermScreen &screen, string_view title,
                               string_view contents, usize input_width = 30) {
    using namespace fmt;

    screen.box(transform, input_width, 2, {});

    if (selected_kind == item_kind)
        screen.print(transform.move(1, 0), emphasis::reverse, "{}", title);
    else
        screen.print(transform.move(1, 0), "{}", title);

    screen.print(transform.move(1, 1), emphasis::underline, "{}", contents);

    transform += {0, 3};

    return transform;
}

template <typename T>
term::Transform draw_basic_button(T item_kind, T selected_kind, bool disabled,
                                  term::Transform transform,
                                  term::TermScreen &screen,
                                  string_view content) {
    using namespace fmt;

    if (selected_kind == item_kind)
        screen.print(transform, emphasis::reverse, "{}", content);
    else if (disabled)
        screen.print(transform, emphasis::faint, "{}", content);
    else
        screen.print(transform, "{}", content);

    transform += {0, 1};

    return transform;
}
} // namespace uppr::app
