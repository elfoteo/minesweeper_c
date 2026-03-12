#ifndef MAIN_H
#define MAIN_H

#define DEFAULT_SCREEN_WIDTH 500
#define DEFAULT_SCREEN_HEIGHT 600
#define XRAY false

enum GameState { STATE_MAIN_MENU, STATE_DIFFICULTY, STATE_PLAYING, STATE_WIN, STATE_LOSE, STATE_ABOUT, STATE_EXIT_NOW };
typedef enum GameState GameState;
extern GameState game_state;
#endif // MAIN_H
