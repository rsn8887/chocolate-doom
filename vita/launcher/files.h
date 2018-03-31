#pragma once

enum Games
{
    GAME_DOOM_SW,
    GAME_DOOM,
    GAME_DOOM2,
    GAME_TNT,
    GAME_PLUTONIA,
    GAME_HERETIC_SW,
    GAME_HERETIC,
    GAME_HEXEN,
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
    struct FileList pwads;
    int present;
};

extern struct Game fs_games[GAME_COUNT];

int FS_Init(void);
void FS_Free(void);
char *FS_Error(void);

int FS_HaveGame(int game);

int FS_ListDir(struct FileList *flist, const char *path, const char *ext);
void FS_FreeFileList(struct FileList *flist);

void FS_ExecGame(int game);
