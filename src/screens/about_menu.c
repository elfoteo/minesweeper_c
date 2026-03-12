#include "../asset_declarations.h"
#include "../button.h"
#include "../main.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#define NAME_COUNT 3

const char *about_title = "About";
const char *desc1 = "Minesweeper written in C using raylib.\n";
const char *desc2 = "Thanks for playing!";
const char *credits = "People who worked on the project:";
const char *names[NAME_COUNT] = {"pako", "elfoteo", "ding yu"};

Shader rainbow = {.id = 0, .locs = 0};
int timeLoc = 0;

void screen_about_draw() {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    // Load shader from memory
    if (rainbow.id == 0) {
        // The shader code needs to be null-terminated, but xxd doesn't do that.
        // We need to copy it to a new buffer and add the null terminator.
        char *shader_code = (char *)malloc(assets_rainbow_fs_len + 1);
        memcpy(shader_code, assets_rainbow_fs, assets_rainbow_fs_len);
        shader_code[assets_rainbow_fs_len] = '\0';

        rainbow = LoadShaderFromMemory(0, shader_code);
        free(shader_code);
        timeLoc = GetShaderLocation(rainbow, "time");
    }

    float time = (float)GetTime();
    SetShaderValue(rainbow, timeLoc, &time, SHADER_UNIFORM_FLOAT);

    ClearBackground(BLACK);

    // Scaling factors based on screen size
    int title_fs = (int)(sh * 0.08f);
    if (title_fs < 24) title_fs = 24;

    int desc_fs = (int)(sh * 0.033f);
    if (desc_fs < 14) desc_fs = 14;

    int name_fs = (int)(sh * 0.06f);
    if (name_fs < 20) name_fs = 20;

    int button_width = (int)(sw * 0.4f);
    if (button_width < 150) button_width = 150;

    int button_height = (int)(sh * 0.1f);
    if (button_height < 40) button_height = 40;

    // Draw starting from the top (title) and proceed downwards with every element
    int cursor = (int)(sh * 0.05f);

    // Draw title
    DrawText(about_title, (int)sw / 2 - MeasureText(about_title, title_fs) / 2, cursor, title_fs, WHITE);
    cursor += title_fs + (int)(sh * 0.033f);

    // Description
    DrawText(desc1, (int)sw / 2 - MeasureText(desc1, desc_fs) / 2, cursor, desc_fs, LIGHTGRAY);
    cursor += desc_fs + 2;
    DrawText(desc2, (int)sw / 2 - MeasureText(desc2, desc_fs) / 2, cursor, desc_fs, LIGHTGRAY);
    cursor += (int)(sh * 0.1f);

    // Credits label
    DrawText(credits, (int)sw / 2 - MeasureText(credits, desc_fs) / 2, cursor, desc_fs, GRAY);
    cursor += desc_fs + (int)(sh * 0.05f);

    // Rainbow names
    BeginShaderMode(rainbow);
    for (int i = 0; i < NAME_COUNT; i++) {
        DrawText(names[i], (int)sw / 2 - MeasureText(names[i], name_fs) / 2, cursor, name_fs, WHITE);
        cursor += name_fs + (int)(sh * 0.033f);
    }
    EndShaderMode();

    cursor = (int)(sh - button_height - sh * 0.05f);
    ButtonStyle style = button_default_style();
    style.fontSize = (int)(button_height * 0.4f);

    if (button_draw_centered("Back", (int)sw / 2, cursor, button_width, button_height, style)) {
        game_state = STATE_MAIN_MENU;
    }
}
