#pragma once

#include "pixel.hpp"

namespace uppr::term {

struct BoxOptions {
    Pixel line_left = '|';
    Pixel line_right = '|';
    Pixel line_top = '-';
    Pixel line_bottom = '-';

    Pixel edge_topleft = '+';
    Pixel edge_topright = '+';
    Pixel edge_bottomleft = '+';
    Pixel edge_bottomright = '+';
};

} // namespace uppr::term
