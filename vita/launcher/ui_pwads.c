#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuFiles_Init(void);
void UI_MenuFiles_Update(void);
void UI_MenuFiles_Draw(void);

struct Menu ui_menu_pwads =
{
    MENU_PWADS,
    "Select PWADs",
    MENU_MAIN, -1,
    UI_MenuFiles_Init,
    UI_MenuFiles_Update,
    UI_MenuFiles_Draw,
};

static struct Menu *self = &ui_menu_pwads;
static struct Menu *parent = NULL;

void UI_MenuFiles_Init(void)
{
    parent = UI_ParentMenu(MENU_PWADS);
}

void UI_MenuFiles_Update(void)
{

}

void UI_MenuFiles_Draw(void)
{

}
