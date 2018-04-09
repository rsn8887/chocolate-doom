#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuFiles_Init(void);
void UI_MenuFiles_Update(void);
void UI_MenuFiles_Draw(void);
void UI_MenuFiles_Reload(void);

static struct Option pwads_opts[] =
{
    { OPT_FILE, "Custom file 1" },
    { OPT_FILE, "Custom file 2" },
    { OPT_FILE, "Custom file 3" },
    { OPT_FILE, "Custom file 4" },
    { OPT_FILE, "DEH file 1" },
    { OPT_FILE, "DEH file 2" },
    { OPT_FILE, "Merge file" },
    { OPT_FILE, "Demo" },
    { OPT_FILE, "Override response file" },
    { OPT_BOOLEAN, "Load DEHACKED lumps" },
};

struct Menu ui_menu_pwads =
{
    MENU_PWADS,
    "Files",
    "Custom content settings",
    NULL, 0, 0, 0,
    UI_MenuFiles_Init,
    UI_MenuFiles_Update,
    UI_MenuFiles_Draw,
    UI_MenuFiles_Reload,
};

static struct Menu *self = &ui_menu_pwads;

void UI_MenuFiles_Init(void)
{
    UI_MenuFiles_Reload();
}

void UI_MenuFiles_Update(void)
{

}

void UI_MenuFiles_Draw(void)
{

}

static const char *pwad_dirs[GAME_COUNT] =
{
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/doom",
    VITA_BASEDIR "/pwads/heretic",
    VITA_BASEDIR "/pwads/heretic",
    VITA_BASEDIR "/pwads/hexen",
    VITA_BASEDIR "/pwads/strife",
};

void UI_MenuFiles_Reload(void)
{
    for (int i = 0; i < 4; ++i)
    {
        pwads_opts[i].codevar = fs_games[ui_game].pwads[i];
        pwads_opts[i].file.dir = pwad_dirs[ui_game];
        pwads_opts[i].file.ext[0] = "wad";
        pwads_opts[i].file.ext[1] = "lmp";
        pwads_opts[i].file.ext[2] = NULL;
    }

    pwads_opts[4].codevar = fs_games[ui_game].dehs[0];
    pwads_opts[4].file.dir = pwad_dirs[ui_game];
    pwads_opts[4].file.ext[0] = "deh";
    pwads_opts[4].file.ext[1] = NULL;

    pwads_opts[5].codevar = fs_games[ui_game].dehs[1];
    pwads_opts[5].file.dir = pwad_dirs[ui_game];
    pwads_opts[5].file.ext[0] = "deh";
    pwads_opts[5].file.ext[1] = NULL;

    pwads_opts[6].codevar = fs_games[ui_game].merge;
    pwads_opts[6].file.dir = pwad_dirs[ui_game];
    pwads_opts[6].file.ext[0] = "wad";
    pwads_opts[6].file.ext[1] = NULL;

    pwads_opts[7].codevar = fs_games[ui_game].demo;
    pwads_opts[7].file.dir = pwad_dirs[ui_game];
    pwads_opts[7].file.ext[0] = "lmp";
    pwads_opts[7].file.ext[1] = NULL;

    pwads_opts[8].codevar = fs_games[ui_game].rsp;
    pwads_opts[8].file.dir = pwad_dirs[ui_game];
    pwads_opts[8].file.ext[0] = "rsp";
    pwads_opts[8].file.ext[1] = NULL;

    pwads_opts[9].codevar = &fs_games[ui_game].dehlump;

    self->opts = pwads_opts;
    self->numopts = 10;
}
