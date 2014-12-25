Snowflake
=========
Iteratively calculates a [Koch snowflake](https://en.wikipedia.org/wiki/Koch_snowflake), and renders it to the screen using the SDL2 library.

To compile and run:
   Without anti-aliasing:
   $ g++ snowflake.cpp -o snowflake -lSDL2 && ./snowflake

   With anti-aliasing:
   $ g++ snowflake.cpp -o snowflake -lSDL2 && ./snowflake -aa