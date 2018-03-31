#pragma once

enum Menus
{
    MENU_MAIN,
    MENU_PWADS,

    MENU_COUNT
};

struct Menu
{
    int id;
    const char *title;

    int parent;
    int arg;

    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
};

extern int ui_current_menu;

int UI_Init(void);
int UI_Update(void);
void UI_Draw(void);
void UI_Free(void);

struct Menu *UI_ParentMenu(int menu);
void UI_PushMenu(int menu, int arg);
void UI_PopMenu(void);
