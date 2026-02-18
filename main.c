#include "raylib.h"
#define screenWidth 500
#define screenHeight 500
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define GRID_W screenWidth
#define GRID_H screenHeight
#define BOMBNUM 10

typedef struct Cell Cell;

struct Cell {
  bool uncovered;
  bool mine;
  int number;
  bool fresh;
  bool flag;
};
Cell matrix[GRID_W][GRID_H];
bool running = true;

bool is_oob(int x, int y) {
  return x < 0 || x > GRID_W - 1 || y < 0 || y > GRID_H - 1;
}
int inputx;
int inputy;

void autoexplode() {
  if (matrix[inputx][inputy].number == 0) {
    matrix[inputx][inputy].fresh = 1;
    for (int x = 0; x < GRID_H; x++) {
      for (int y = 0; y < GRID_H; y++) { // For every cell in the
        if (matrix[x][y].fresh) {        // If it's fresh air
          matrix[x][y].fresh = 0;
          for (int x0 = -1; x0 < 2; x0++) {
            for (int y0 = -1; y0 < 2;
                 y0++) { // Check in a 3x3 area its neighbours
              if (!is_oob(x + x0, y + y0)) {           // Skip if
                if (!matrix[x + x0][y + y0].uncovered) // for (int
                { // If its earth set it to air
                  matrix[x + x0][y + y0].uncovered = 1;
                  if (matrix[x + x0][y + y0].number == 0) {
                    matrix[x + x0][y + y0].fresh = 1;
                  }
                }
              }
            }
          }
          x = 0;
          y = 0;
        }
      }
    }
  }
}
int main() {
  srand(time(NULL));

  for (int x = 0; x < GRID_W; x++) {
    for (int y = 0; y < GRID_H; y++) {
      matrix[x][y].mine = 0;
      matrix[x][y].uncovered = 0;
      matrix[x][y].number = 0;
      matrix[x][y].fresh = 0;
    }
  }

  for (int i = 0; i < BOMBNUM;) {
    int rand_x = rand() % GRID_W;
    int rand_y = rand() % GRID_H;
    if (!matrix[rand_x][rand_y].mine) {
      // place_mine(); // Places the mine and increments adjacent
      // tiles number by one
      matrix[rand_x][rand_y].mine = true;
      // Loop all surrounding tiles and tile.number++;
      for (int x = -1; x < 2; x++) {
        for (int y = -1; y < 2; y++) {
          int ax = rand_x + x;
          int ay = rand_y + y;
          if (is_oob(ax, ay) || (x == 0 && y == 0)) {
            continue; // Skip this iteration of the loop
          }
          matrix[ax][ay].number++;
        }
      }
      i++;
    }
  }
  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    Color squarecolour = RED;

    BeginDrawing();

    ClearBackground(RAYWHITE);

    for (int x = 0; x < screenWidth; x += 50) {
      for (int y = 0; y < screenHeight; y += 50) {
        Vector2 squarelocation = {(float)x, (float)y};
        Vector2 compare = GetMousePosition();
        if (matrix[x][y].uncovered) {
          squarecolour = BLUE;
          if (compare.x > squarelocation.x && compare.y > squarelocation.y &&
              compare.x < squarelocation.x + 40 &&
              compare.y < squarelocation.y + 40) {
            squarecolour = DARKBLUE;
          }

        } else if (matrix[x][y].flag) {
          squarecolour = YELLOW;
          if (compare.x > squarelocation.x && compare.y > squarelocation.y &&
              compare.x < squarelocation.x + 40 &&
              compare.y < squarelocation.y + 40) {
            squarecolour = ORANGE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
              matrix[x][y].flag = false;
          }

        } else if (compare.x > squarelocation.x &&
                   compare.y > squarelocation.y &&
                   compare.x < squarelocation.x + 40 &&
                   compare.y < squarelocation.y + 40) {
          squarecolour = MAROON;
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            matrix[x][y].uncovered = true;
            inputx = x;
            inputy = y;
            // autoexplode(); TODO fix this bs
          } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            matrix[x][y].flag = true;
          }
        } else
          squarecolour = RED;
        DrawRectangle(x, y, 40, 40, squarecolour);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
