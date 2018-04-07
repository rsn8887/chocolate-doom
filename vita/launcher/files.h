#pragma once

#define MAX_PWADS 4

enum Games
{
    // doom1-based
    GAME_DOOM_SW,
    GAME_DOOM,
    GAME_FREEDM,
    GAME_FREEDOOM,
    GAME_CHEX,
    // doom2-based
    GAME_DOOM2,
    GAME_TNT,
    GAME_PLUTONIA,
    GAME_FREEDOOM2,
    // heretic-based
    GAME_HERETIC_SW,
    GAME_HERETIC,
    // hexen-based
    GAME_HEXEN,
    // strife-based
    GAME_STRIFE,

    GAME_COUNT
};

struct FileList
{
    char **files;
    int numfiles;
};

struct Game
{
    const char *name;
    const char *dir;
    const char *iwad;
    int present;

    char pwads[MAX_PWADS][256];
    char deh[256];
    char demo[256];
    int skill;
    int warp;
    char monsters[2];
    int record;
    int charclass;
};

extern struct Game fs_games[GAME_COUNT];

int FS_Init(void);
void FS_Free(void);
char *FS_Error(void);

int FS_HaveGame(int game);

int FS_ListDir(struct FileList *flist, const char *path, const char *ext);
void FS_FreeFileList(struct FileList *flist);

void FS_ExecGame(int game);
