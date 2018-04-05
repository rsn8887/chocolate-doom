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
    { "Strife", "strife", "strife1.wad" },
};

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
    int res = fexists(buf);
    snprintf(buf, sizeof(buf), VITA_BASEDIR "/iwads/%s", fs_games[g].iwad);
    return res || fexists(buf);
}

int FS_Init(void)
{
    mkdir(VITA_BASEDIR, 0755);
    mkdir(VITA_TMPDIR, 0755);
    mkdir(VITA_BASEDIR "/pwads", 0755);

    int numgames = 0;
    for (int i = 0; i < GAME_COUNT; ++i)
    {
        int present = CheckForGame(i);
        fs_games[i].present = present;
        fs_games[i].monsters[0] = '0';
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
    if (game < 0 || game >= GAME_COUNT) return;

    struct Game *g = fs_games + game;

    FILE *f = fopen(VITA_TMPDIR "/chocolat.rsp", "w");
    if (!f) I_Error("Could not create file\n" VITA_TMPDIR "/chocolat.rsp");

    fprintf(f, "-iwad %s\n", g->iwad);

    int file = 0;
    for (int i = 0; i < MAX_PWADS; ++i)
        if (g->pwads[i][0]) { file = 1; break; }
    if (file)
    {
        fprintf(f, "-file");

        for (int i = 0; i < MAX_PWADS; ++i)
            if (g->pwads[i][0])
                fprintf(f, " %s", g->pwads[i]);

        if (g->demo[0])
            fprintf(f, " %s", g->demo);

        fprintf(f, "\n");
    }

    if (g->deh[0])
        fprintf(f, "-deh %s\n", g->deh);

    if (g->skill)
        fprintf(f, "-skill %d\n", g->skill);

    if (g->warp)
    {
        if (game < GAME_DOOM2 || game == GAME_HERETIC || game == GAME_HERETIC_SW)
        {
            int ep = g->warp / 10;
            int map = g->warp % 10;
            if (!ep) ep = 1;
            if (!map) map = 1;
            fprintf(f, "-warp %d %d\n", ep, map);
        }
        else
        {
            fprintf(f, "-warp %d\n", g->warp);
        }
    }

    if (g->charclass)
        fprintf(f, "-class %d\n", g->charclass - 1);

    if (g->monsters[0] == '1')
        fprintf(f, "-nomonsters\n");
    else if (g->monsters[0] == '2')
        fprintf(f, "-fast\n");
    else if (g->monsters[0] == '4')
        fprintf(f, "-respawn\n");
    else if (g->monsters[0] == '6')
        fprintf(f, "-fast\n-respawn\n");

    if (g->record)
    {
        fprintf(f, "-record %s/mydemo\n", VITA_TMPDIR);
    }
    else if (g->demo[0])
    {
        if (!file) fprintf(f, "-file %s\n", g->demo);
        char *dot = strrchr(g->demo, '.');
        if (dot) *dot = '\0'; // playdemo doesn't want extensions
        fprintf(f, "-playdemo %s\n", g->demo);
    }

    fclose(f);

    static char exe[128];
    snprintf(exe, sizeof(exe), "app0:/%s.bin", g->dir);

    static char *argv[3];
    argv[0] = exe;
    argv[1] = "@" VITA_TMPDIR "/chocolat.rsp";
    argv[2] = NULL;

    sceAppMgrLoadExec(exe, argv, NULL);
}
