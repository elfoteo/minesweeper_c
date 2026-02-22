#ifndef SCREENS_H
#define SCREENS_H

typedef struct GridSettings GridSettings;
struct GridSettings {
    char *name;
    int grid_size;
    int mines;
};

void screen_menu_draw();
void screen_game_draw(GridSettings *gs);
void screen_about_draw();
void screen_settings_draw();
void screen_win_draw();
void screen_loose_draw();
GridSettings *screen_difficulty_draw();

#endif // SCREENS_H
