#ifndef _FONTS_H
#define _FONTS_H

/*

[Plan]

It would be nice to simply select a buffer to draw from that holds the 
char along with any attributes like color.

ie:

my_text_buffer -> ['c', (0,0), red]['a', (15, 0), green]['t', (30, 0), blue]

use_font(&my_font);
use_buffer_as_text_rendering_source(&my_text_buffer);
render_text();

*/

typedef struct renderable_char_t
{
    struct { u16 x0, x1, y0, y1; } texcoords;

} renderable_char_t;

#endif