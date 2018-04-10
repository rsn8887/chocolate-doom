#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "configs.h"
#include "ui.h"

void UI_MenuNet_Init(void);
void UI_MenuNet_Update(void);
void UI_MenuNet_Draw(void);
void UI_MenuNet_Reload(void);

static void DoJoinNet(int);
static void DoJoinLocal(int);
static void DoJoinAuto(int);
static void DoHost(int);

static const char *mode_labels[] = 
{
    "Cooperative",
    "Deathmatch",
    "Deathmatch 2.0",
};

static const char *mode_values[] = 
{
    "", "deathmatch", "altdeath",
};

static struct Option net_opts[] =
{
    { OPT_STRING, "Player name", "player_name" },
    { OPT_STRING, "Server name" },
    {
        OPT_CHOICE,
        "Game mode",
        NULL, NULL,
        .choice =
        {
            mode_labels, mode_values,
            3, 0,
        },
    },
    { OPT_CALLBACK, "Join Internet game", .cb = DoJoinNet },
    { OPT_CALLBACK, "Join local game", .cb = DoJoinLocal },
    { OPT_CALLBACK, "Autojoin local game", .cb = DoJoinAuto },
    { OPT_CALLBACK, "Host game", .cb = DoHost },
};

struct Menu ui_menu_net =
{
    MENU_PWADS,
    "Net",
    "Network game settings",
    NULL, 0, 0, 0,
    UI_MenuNet_Init,
    UI_MenuNet_Update,
    UI_MenuNet_Draw,
    UI_MenuNet_Reload,
};

static struct Menu *self = &ui_menu_net;

void UI_MenuNet_Init(void)
{
    UI_MenuNet_Reload();
}

void UI_MenuNet_Update(void)
{

}

void UI_MenuNet_Draw(void)
{

}

void UI_MenuNet_Reload(void)
{
    net_opts[1].codevar = fs_games[ui_game].servername;
    net_opts[2].codevar = fs_games[ui_game].gmode;

    self->opts = net_opts;
    self->numopts = 7;
}

static void NetStart(const char *mode)
{
    strncpy(fs_games[ui_game].netmode, mode, 32);
    UI_SaveOptions();
    CFG_SaveAll();
    FS_ExecGame(ui_game);
}

static void DoJoinNet(int arg)
{
    NetStart("search");
}

static void DoJoinLocal(int arg)
{
    NetStart("localsearch");
}

static void DoJoinAuto(int arg)
{
    NetStart("autojoin");
}

static void DoHost(int arg)
{
    NetStart("server");
}
