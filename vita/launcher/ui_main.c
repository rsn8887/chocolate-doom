#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "configs.h"
#include "ui.h"

void UI_MenuMain_Init(void);
void UI_MenuMain_Update(void);
void UI_MenuMain_Draw(void);

struct Menu ui_menu_main =
{
    MENU_MAIN,
    "Game",
    "Select game",
    NULL, 0, 0, 0,
    UI_MenuMain_Init,
    UI_MenuMain_Update,
    UI_MenuMain_Draw,
};

static struct Menu *self = &ui_menu_main;

void UI_MenuMain_Init(void)
{
    for (ui_game = 0; ui_game < GAME_COUNT; ++ui_game)
        if (fs_games[ui_game].present) break;
}

void UI_MenuMain_Update(void)
{
    int prev_selection = ui_game;

    if (IN_ButtonPressed(B_DDOWN))
    {
        for (ui_game += 1; ui_game < GAME_COUNT; ++ui_game)
            if (fs_games[ui_game].present) break;
        if (ui_game >= GAME_COUNT) ui_game = prev_selection;
    }
    else if (IN_ButtonPressed(B_DUP))
    {
        for (ui_game -= 1; ui_game >= 0; --ui_game)
            if (fs_games[ui_game].present) break;
        if (ui_game < 0) ui_game = prev_selection;
    }

    if (ui_game != prev_selection)
    {
        // HACK
        int tmp = ui_game;
        ui_game = prev_selection;
        UI_SaveOptions();
        ui_game = tmp;
        UI_ReloadOptions();
    }

    if (IN_ButtonPressed(B_CROSS))
    {
        UI_SaveOptions();
        CFG_SaveAll();
        FS_ExecGame(ui_game);
    }
}

void UI_MenuMain_Draw(void)
{
    int y = 160;
    for (int i = 0; i < GAME_COUNT; ++i)
    {
        if (!fs_games[i].present) continue;

        unsigned c = (ui_game == i) ? C_GREEN : C_WHITE;
        R_Print(P_XCENTER, SCR_CX, y, c, fs_games[i].name);

        y += 24;
    }
}
