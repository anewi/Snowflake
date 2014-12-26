Snowflake To-Do List
====================

- Make draw_snowflake_antialiased() draw lines between vertices as well as the vertices themselves
- Replace all uses of create_snowflake_vertex() with { } notation for initialising a struct
- Have a maximum of the size of the SDL_Point array created by draw_snowflake(), because SDL_RenderDrawLines() causes a Segmentation Fault when it is called with more than approximately one million points (on my system). Make multiple calls to SDL_RenderDrawLines() with smaller arrays instead.
- Perform anti-aliasing on multiple separate threads
- Change the method of ensuring each equilateral triangle points outward to save the need to store a "centre" for each vertex.
- Ability to render to a .png file as an alternative to on the screen.
- Flexibilty in the size of the rendered snowflake, and in the number of samples used for each pixel by the anti-aliasing algorithm.
- Storing data compressed to disk while calculating large iterations, limiting memory usage.
- Any possible speed and memory improvements.

Eventual goal is the ability to perform a long-running calculation of e.g. a 4000 x 4000 snowflake, using 256 samples for each pixel in the anti-aliasing (allowing every shade of gray from #000000 to #FFFFFF), exported as a PNG file.