/**
 * Application to draw a Koch Snowflake using the SDL2 library.
 *
 * To compile and run:
 *   Without anti-aliasing:
 *     g++ snowflake.cpp -o snowflake -lSDL2 && ./snowflake
 *
 *   With anti-aliasing:
 *     g++ snowflake.cpp -o snowflake -lSDL2 && ./snowflake -aa
 *
 * @author Anna Winters
 */

#include <SDL2/SDL.h>

#include <array>
#include <bitset>
#define _USE_MATH_DEFINES //Allow use of M_PI
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

struct snowflake_vertex {
  double x;
  double y;
};

inline snowflake_vertex create_snowflake_vertex(double x, double y) {
  snowflake_vertex vertex;
  vertex.x = x;
  vertex.y = y;
  return vertex;
}

/**
 * @param x x co-ord of centre of triangle
 * @param y y co-ord of centre of triangle
 * @param length The distance between the centre of the triangle and each vertex.
 */
void generate_initial_triangle(std::vector<snowflake_vertex>* triangle, double x, double y, double length) {
  double side_length, height;

  (*triangle).push_back(create_snowflake_vertex(x, y-length)); //Add top vertex, which is simply length distance above the centre

  //Calculate length of all of the sides of the triangle using cosine rule and known angles/lengths
  side_length = sqrt(pow(length, 2.0) + pow(length, 2.0) - 2*length*length*cos((120 * M_PI)/ 180));

  //The side of the triangle, half of the base and the height form a right-angled triangle. Use pythag to calculate triangle height.
  height = sqrt(pow(side_length, 2.0)-pow(side_length/2.0, 2.0));

  (*triangle).push_back(create_snowflake_vertex(x-(side_length/2.0), y-length+height)); //Bottom-left vertex
  
  (*triangle).push_back(create_snowflake_vertex(x+(side_length/2.0), y-length+height)); //Bottom-right vertex
}

void draw_snowflake(SDL_Renderer* renderer, std::vector<snowflake_vertex>* snowflake) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  int count = (*snowflake).size();
  SDL_Point points [count+1];
  for (int i = 0; i < count; i++) {
    points[i].x = round((*snowflake)[i].x);
    points[i].y = round((*snowflake)[i].y);
  }
  points[count].x = points[0].x;
  points[count].y = points[0].y;
  SDL_RenderDrawLines(renderer, points, count+1);
}

//An alternative draw_snowflake that uses 16 samples for each pixel
void draw_snowflake_antialiased(SDL_Renderer* renderer, std::vector<snowflake_vertex>* snowflake) {
  std::array<std::bitset<3200>, 3200> superscreen; //Array of bitsets representing rows of pixels on a 3200 x 3200 screen
  std::array<std::vector<SDL_Point>, 16> screen; //Vectors of SDL_Points to be drawn in 16 shades of gray

  int count = (*snowflake).size();
  SDL_Point point;

  for (int i = 0; i < count; i++) {
    superscreen[round((*snowflake)[i].x * 4)][round((*snowflake)[i].y * 4)] = true;
  }

  for (int x = 0; x < 800; x++) {
    for (int y = 0; y < 800; y++) {
      int subpixels = 0;
      for (int subx = 0; subx < 4; subx++) {
	for (int suby = 0; suby < 4; suby++) {
	  if (superscreen[x * 4 + subx][y * 4 + suby] == true) {
	    subpixels++;
	  }
	}
      }
      point.x = x;
      point.y = y;
      screen[subpixels].push_back(point);
    }
  }
  
  int shade;
  for (int s = 0; s < 16; s++) {
    shade = s*64;
    SDL_SetRenderDrawColor(renderer, shade, shade, shade, 255);
    SDL_RenderDrawPoints(renderer, screen[s].data(), screen[s].size());
    std::cout << screen[s].size() << std::endl;
  }

}

void draw_background(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 88, 88, 88, 255);
  SDL_RenderClear(renderer);
}

void iterate_snowflake(std::vector<snowflake_vertex>* previous_snowflake, std::vector<snowflake_vertex>* snowflake){
  int end_vertex_offset; //Used to take into account that the final vertex connects up with the first
  double x1, y1, x2, y2, x3, y3; //Co-ordinates of the vertices of each new equilateral triangle
  double xd, yd; //The differences in x and y across the base of the new triangle
  double side_length, height; //Side lengths and height of the new equilateral triangle
  double xh, yh; //The differences in x and y across the height of the new triangle (perpendicular to xd, yd)

  (*snowflake).clear(); //Empty the vector we will write into

  for (int i = 0; i < (*previous_snowflake).size(); i++) { //Loop through every current side of the snowflake
    (*snowflake).push_back((*previous_snowflake)[i]);

    end_vertex_offset = (i == (*previous_snowflake).size() - 1 ? 0 : i + 1); //0 if at end; i + 1 otherwise

    x1 = ((*previous_snowflake)[i].x * 2.0 + (*previous_snowflake)[end_vertex_offset].x * 1.0) / 3.0;
    y1 = ((*previous_snowflake)[i].y * 2.0 + (*previous_snowflake)[end_vertex_offset].y * 1.0) / 3.0;

    x3 = ((*previous_snowflake)[i].x * 1.0 + (*previous_snowflake)[end_vertex_offset].x * 2.0) / 3.0;
    y3 = ((*previous_snowflake)[i].y * 1.0 + (*previous_snowflake)[end_vertex_offset].y * 2.0) / 3.0;

    xd = x1 - x3;
    yd = y1 - y3;

    side_length = sqrt(pow(xd, 2.0) + pow(yd, 2.0)); //Calculate side length using pythag on differences in x, y co-ords

    height = sqrt(pow(side_length, 2.0) - pow(side_length/2.0, 2.0)); //Calculate triangle height from right-angled triangle with side length as hypotenuse

    yh = sqrt(pow(height, 2.0) / (pow(yd, 2.0) / pow(xd, 2.0) + 1));

    //    yh = -(xd/yd)*sqrt(pow(height, 2.0) - pow(yd, 2.0));

    xh = -(yd*yh)/xd;

    x2 = (x1 + x3)/2.0 + xh;
    y2 = (y1 + y3)/2.0 + yh;

    (*snowflake).push_back(create_snowflake_vertex(x1, y1));
    (*snowflake).push_back(create_snowflake_vertex(x2, y2));
    (*snowflake).push_back(create_snowflake_vertex(x3, y3));
  }
}

int main(int argc, char *argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;

  std::vector<snowflake_vertex> buffer_snowflake;
  std::vector<snowflake_vertex> render_snowflake;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_OPENGL, &window, &renderer);
  SDL_SetWindowTitle(window, "Snowflake");

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

  for (int i = 0; i < 12; i++) {
    if (i == 0) {
      generate_initial_triangle(&buffer_snowflake, 400.0, 400.0, 300.0);
    } else {
      iterate_snowflake(&render_snowflake, &buffer_snowflake);
    }
    std::swap(buffer_snowflake, render_snowflake);
    std::cout << "Current snowflake has " << render_snowflake.size() << " vertices." << std::endl;
    draw_background(renderer);
    if (argc > 1 && strncmp(argv[1], "-aa", 3) == 0) {
      draw_snowflake_antialiased(renderer, &render_snowflake);
    }else{
      draw_snowflake(renderer, &render_snowflake);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
  }
  SDL_Delay(3000);

  return 0;
}