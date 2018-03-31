#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuMain_Init(void);
void UI_MenuMain_Update(void);
void UI_MenuMain_Draw(void);

struct Menu ui_menu_main =
{
    MENU_MAIN,
    "Select game",
    -1, -1,
    UI_MenuMain_Init,
    UI_MenuMain_Update,
    UI_MenuMain_Draw,
};

static struct Menu *self = &ui_menu_main;
static struct Menu *parent = NULL;
static int selection = 0;

void UI_MenuMain_Init(void)
{
    for (selection = 0; selection < GAME_COUNT; ++selection)
        if (fs_games[selection].present) break;
}

void UI_MenuMain_Update(void)
{
    int prev_selection = selection;

    if (IN_ButtonPressed(B_DDOWN))
    {
        for (selection += 1; selection < GAME_COUNT; ++selection)
            if (fs_games[selection].present) break;
        if (selection >= GAME_COUNT) selection = prev_selection;
    }
    else if (IN_ButtonPressed(B_DUP))
    {
        for (selection -= 1; selection >= 0; --selection)
            if (fs_games[selection].present) break;
        if (selection < 0) selection = prev_selection;
    }

    if (IN_ButtonPressed(B_SQUARE))
    {
        UI_PushMenu(MENU_PWADS, selection);
        return;
    }

    if (IN_ButtonPressed(B_CROSS))
        FS_ExecGame(selection);
}

void UI_MenuMain_Draw(void)
{
    int y = 128;
    for (int i = 0; i < GAME_COUNT; ++i)
    {
        if (!fs_games[i].present) continue;

        unsigned c = (selection == i) ? C_GREEN : C_LTGREY;
        R_Print(P_XCENTER, SCR_CX, y, c, fs_games[i].name);

        y += 24;
    }
}
