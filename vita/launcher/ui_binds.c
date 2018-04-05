#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuBinds_Init(void);
void UI_MenuBinds_Update(void);
void UI_MenuBinds_Draw(void);
void UI_MenuBinds_Reload(void);

static struct Option bind_opts[] =
{
    { OPT_BUTTON, "Fire", "key_fire", NULL, .button = B_RTRIGGER },
    { OPT_BUTTON, "Use", "key_use", NULL, .button = B_SQUARE },
    { OPT_BUTTON, "Run", "key_speed", NULL, .button = B_LTRIGGER },
    { OPT_BUTTON, "Next weapon", "key_nextweapon", NULL, .button = B_TRIANGLE },
    { OPT_BUTTON, "Prev weapon", "key_prevweapon", NULL, .button = B_CIRCLE },
    { OPT_BUTTON, "Strafe left", "key_strafeleft", NULL, .button = -1 },
    { OPT_BUTTON, "Strafe right", "key_straferight", NULL, .button = -1 },
    { OPT_BUTTON, "Show map", "key_map_toggle", NULL, .button = B_TOUCH1 },
    // heretic/hexen
    { OPT_BUTTON, "Look up", "key_lookup", NULL, .button = B_DUP },
    { OPT_BUTTON, "Look down", "key_lookdown", NULL, .button = B_DDOWN },
    { OPT_BUTTON, "Look center", "key_lookcenter", NULL, .button = B_SQUARE },
    { OPT_BUTTON, "Fly up", "key_flyup", NULL, .button = B_DLEFT },
    { OPT_BUTTON, "Fly down", "key_flydown", NULL, .button = B_DRIGHT },
    { OPT_BUTTON, "Fly center", "key_flycenter", NULL, .button = -1 },
    { OPT_BUTTON, "Inventory left", "key_invleft", NULL, .button = -1 },
    { OPT_BUTTON, "Inventory right", "key_invright", NULL, .button = B_SELECT },
    { OPT_BUTTON, "Use inventory item", "key_useartifact", NULL, .button = B_CROSS },
    // hexen
    { OPT_BUTTON, "Jump", "key_jump", NULL, .button = B_CIRCLE },
};

static struct Option bind_opts_strife[] =
{
    { OPT_BUTTON, "Fire", "key_fire", NULL, .button = B_RTRIGGER },
    { OPT_BUTTON, "Use", "key_use", NULL, .button = B_SQUARE },
    { OPT_BUTTON, "Run", "key_speed", NULL, .button = B_LTRIGGER },
    { OPT_BUTTON, "Next weapon", "key_nextweapon", NULL, .button = B_TRIANGLE },
    { OPT_BUTTON, "Prev weapon", "key_prevweapon", NULL, .button = B_CIRCLE },
    { OPT_BUTTON, "Strafe left", "key_strafeleft", NULL, .button = -1 },
    { OPT_BUTTON, "Strafe right", "key_straferight", NULL, .button = -1 },
    { OPT_BUTTON, "Show map", "key_map_toggle", NULL, .button = B_TOUCH1 },
    { OPT_BUTTON, "Jump", "key_jump", NULL, .button = B_CIRCLE },
    { OPT_BUTTON, "Look up", "key_lookUp", NULL, .button = B_DUP },
    { OPT_BUTTON, "Look down", "key_lookDown", NULL, .button = B_DDOWN },
    { OPT_BUTTON, "Inventory left", "key_invLeft", NULL, .button = -1 },
    { OPT_BUTTON, "Inventory right", "key_invRight", NULL, .button = B_SELECT },
    { OPT_BUTTON, "Use inventory item", "key_invUse", NULL, .button = B_CROSS },
    { OPT_BUTTON, "Use health", "key_useHealth", NULL, .button = -1 },
    { OPT_BUTTON, "Show mission list", "key_mission", NULL, .button = -1 },
};

struct Menu ui_menu_binds =
{
    MENU_BINDS,
    "Buttons",
    "Change button bindings",
    bind_opts, 8, 0, 0,
    UI_MenuBinds_Init,
    UI_MenuBinds_Update,
    UI_MenuBinds_Draw,
    UI_MenuBinds_Reload,
};

static struct Menu *self = &ui_menu_binds;

void UI_MenuBinds_Init(void)
{

}

void UI_MenuBinds_Update(void)
{

}

void UI_MenuBinds_Draw(void)
{

}

void UI_MenuBinds_Reload(void)
{
    switch (ui_game)
    {
        case GAME_HERETIC_SW:
        case GAME_HERETIC:
            self->opts = bind_opts;
            self->numopts = 8 + 9;
            break;

        case GAME_HEXEN:
            self->opts = bind_opts;
            self->numopts = 8 + 9 + 1;
            break;

        case GAME_STRIFE:
            self->opts = bind_opts_strife;
            self->numopts = 16;
            break;

        default: // doom
            self->opts = bind_opts;
            self->numopts = 8;
            break;
    }
}
