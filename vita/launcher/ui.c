#include "config.h"
#include "utils.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

extern struct Menu ui_menu_main;
extern struct Menu ui_menu_pwads;

static struct Menu *ui_menus[MENU_COUNT] =
{
    &ui_menu_main,
    &ui_menu_pwads,
};

int ui_current_menu = MENU_MAIN;

int UI_Init(void)
{
    for (int i = 0; i < MENU_COUNT; ++i)
        ui_menus[i]->init();

    return 0;
}

int UI_Update(void)
{
    struct Menu *menu = ui_menus[ui_current_menu];

    if (IN_ButtonPressed(B_CIRCLE))
    {
        if (menu->parent < MENU_MAIN) return 1;
        UI_PopMenu();
        return 0;
    }

    if (IN_ButtonPressed(B_START))
        return 1;

    menu->update();

    return 0;
}

void UI_Draw(void)
{
    struct Menu *menu = ui_menus[ui_current_menu];

    R_Print(P_XCENTER, SCR_CX, 32, C_WHITE, menu->title);
    menu->draw();
}

void UI_Free(void)
{

}

struct Menu *UI_ParentMenu(int mi)
{
    if (mi < 0 || mi >= MENU_COUNT) return NULL;
    struct Menu *menu = ui_menus[mi];
    if (menu->parent < 0) return NULL;
    return ui_menus[menu->parent];
}

void UI_PushMenu(int mi, int arg)
{
    if (mi < 0 || mi >= MENU_COUNT) return;
    struct Menu *menu = ui_menus[ui_current_menu];
    struct Menu *nextmenu = ui_menus[mi];
    nextmenu->arg = arg;
    ui_current_menu = mi;
}

void UI_PopMenu(void)
{
    struct Menu *menu = ui_menus[ui_current_menu];
    ui_current_menu = menu->parent;
}
