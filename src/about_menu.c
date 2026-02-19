#include "button.h"
#include "main.h"
#include <raylib.h>

#define NAME_COUNT 2
static const char *rainbowFragmentShader = "#version 330 core\n"
                                           "in vec2 fragTexCoord;\n"
                                           "in vec4 fragColor;\n"
                                           "out vec4 finalColor;\n"
                                           "uniform sampler2D texture0;\n"
                                           "uniform float time;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    vec4 texColor = texture(texture0, fragTexCoord);\n"
                                           "    float wave = (gl_FragCoord.x - gl_FragCoord.y) * 0.02;\n"
                                           "    float t = time;\n"
                                           "    float r = 0.5 + 0.5 * sin(wave - t);\n"
                                           "    float g = 0.5 + 0.5 * sin(wave - t + 2.094);\n"
                                           "    float b = 0.5 + 0.5 * sin(wave - t + 4.188);\n"
                                           "    vec3 rainbow = vec3(r, g, b);\n"
                                           "    finalColor = vec4(rainbow, texColor.a);\n"
                                           "}\n";
const char *title = "About";
const int title_fontsize = 48;
const char *desc1 = "Minesweeper written in C using raylib.\n";
const char *desc2 = "Thanks for playing!";
const char *credits = "People who worked on the project:";
const char *names[NAME_COUNT] = {"pako", "elfoteo"};

Shader rainbow = {.id = 0, .locs = 0};
int timeLoc = 0;

void about_menu_draw() {
    // Load shader from memory
    if (rainbow.id == 0) {
        rainbow = LoadShaderFromMemory(0, rainbowFragmentShader);
        timeLoc = GetShaderLocation(rainbow, "time");
    }

    float time = (float)GetTime();
    SetShaderValue(rainbow, timeLoc, &time, SHADER_UNIFORM_FLOAT);

    ClearBackground(BLACK);

    // Draw starting from the top (title) and proceed downwards with every element
    int cursor = 20;

    // Draw title
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, title_fontsize) / 2, cursor, title_fontsize, WHITE);
    cursor += title_fontsize;

    // Description
    cursor += 20;
    DrawText(desc1, SCREEN_WIDTH / 2 - MeasureText(desc1, 20) / 2, cursor, 20, LIGHTGRAY);
    cursor += 22;
    DrawText(desc2, SCREEN_WIDTH / 2 - MeasureText(desc2, 20) / 2, cursor, 20, LIGHTGRAY);
    cursor += 80;

    // Credits label
    DrawText(credits, SCREEN_WIDTH / 2 - MeasureText(credits, 20) / 2, cursor, 20, GRAY);

    cursor += 40;

    // Rainbow names
    int nameSize = 36;

    BeginShaderMode(rainbow);
    for (int i = 0; i < NAME_COUNT; i++) {
        DrawText(names[i], SCREEN_WIDTH / 2 - MeasureText(names[i], nameSize) / 2, cursor, nameSize, WHITE);
        cursor += nameSize + 20;
    }

    EndShaderMode();

    cursor += 100;
    if (button_draw_centered("Back", SCREEN_WIDTH / 2, cursor, 200, 60, BUTTON_DEFAULT_STYLE)) {
        game_state = STATE_MAIN_MENU;
    }
}
