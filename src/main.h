#ifndef MAIN_H
#define MAIN_H

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

enum GameState { STATE_MAIN_MENU, STATE_PLAYING, STATE_WIN, STATE_LOSE, STATE_ABOUT, STATE_EXIT_NOW };
typedef enum GameState GameState;
extern GameState game_state;
#endif // MAIN_H
