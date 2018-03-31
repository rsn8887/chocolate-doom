#include "config.h"
#include "utils.h"
#include "files.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

static char fs_error[4096];

struct Game fs_games[GAME_COUNT] =
{
    { "Doom (Shareware)", "doom", "doom1.wad" },
    { "Doom", "doom", "doom.wad" },
    { "Doom II", "doom", "doom2.wad" },
    { "Final Doom: TNT Evilution", "doom", "tnt.wad" },
    { "Final Doom: The Plutonia Experiment", "doom", "plutonia.wad" },
    { "Heretic (Shareware)", "heretic", "heretic1.wad" },
    { "Heretic", "heretic", "heretic.wad" },
    { "Hexen", "hexen", "hexen.wad" },
    { "Strife", "strife", "strife.wad" },
};

struct FileList fs_pwads[GAME_COUNT];

static void SetError(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(fs_error, sizeof(fs_error), fmt, argptr);
    va_end(argptr);
}

static int CheckForGame(int g)
{
    static char buf[512];
    snprintf(buf, sizeof(buf), VITA_BASEDIR "/%s", fs_games[g].iwad);
    return fexists(buf);
}

static void BuildPWADList(int g)
{
    struct Game *game = fs_games + g;
    struct FileList *flist = &(game->pwads);
    static char dir[512];

    snprintf(dir, sizeof(dir), VITA_BASEDIR "/pwads/%s", game->dir);

    flist->files = NULL;
    flist->numfiles = 0;

    FS_ListDir(flist, dir, "wad");
    FS_ListDir(flist, dir, "lmp");
}

int FS_Init(void)
{
    int numgames = 0;
    for (int i = 0; i < GAME_COUNT; ++i)
    {
        int present = CheckForGame(i);
        fs_games[i].present = present;
        if (present) BuildPWADList(i);
        numgames += present;
    }

    if (!numgames)
    {
        SetError("No supported games were found in the data directory.\n" \
                 "Make sure you have installed at least one game properly.");
        return -1;
    }

    return 0;
}

void FS_Free(void)
{
    for (int i = 0; i < GAME_COUNT; ++i)
        FS_FreeFileList(&(fs_games[i].pwads));
} 

char *FS_Error(void)
{
    return fs_error;
}

int FS_HaveGame(int game)
{
    if (game < 0 || game >= GAME_COUNT) return 0;
    return fs_games[game].present;
}

int FS_ListDir(struct FileList *flist, const char *path, const char *ext)
{
    static char buf[512];

    DIR *dp = opendir(path);
    if (!dp) return -1;

    struct dirent *ep;
    while (ep = readdir(dp))
    {
        if (!ep->d_name || ep->d_name[0] == '.')
            continue;

        char *fname = ep->d_name;
        snprintf(buf, sizeof(buf), "%s/%s", path, fname);

        if (!isdir(buf) && !strcasecmp(fext(fname), ext))
        {
            flist->files = realloc(flist->files, (flist->numfiles + 1) * sizeof(char*));
            if (!flist->files) return -2;
            flist->files[flist->numfiles] = strdup(fname);
            flist->numfiles++;
        }
    }

    closedir(dp);

    return flist->numfiles;
}

void FS_FreeFileList(struct FileList *flist)
{
    for (int i = 0; i < flist->numfiles; ++i)
      free(flist->files[i]);

    free(flist->files);
    flist->files = NULL;
    flist->numfiles = 0;
}

void FS_ExecGame(int game)
{
    static char exe[128];
    static char *argv[4];

    if (game < 0 || game >= GAME_COUNT) return;

    snprintf(exe, sizeof(exe), "app0:/%s.bin", fs_games[game].dir);

    argv[0] = exe;
    argv[1] = "-iwad";
    argv[2] = (char*)fs_games[game].iwad;
    argv[3] = NULL;

    sceAppMgrLoadExec(exe, argv, NULL);
}
