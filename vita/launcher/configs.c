#include "config.h"
#include "utils.h"
#include "files.h"
#include "configs.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_CVARLEN 100
#define MAX_CVARNAME 80

static const char *std_vars[] =
{
    "mouse_sensitivity",
    "sfx_volume",
    "music_volume",
    "key_right",
    "key_left",
    "key_up",
    "key_down",
    "key_strafeleft",
    "key_straferight",
    "key_jump",
    "key_flyup",
    "key_flydown",
    "key_flycenter",
    "key_lookup",
    "key_lookdown",
    "key_lookcenter",
    "key_invleft",
    "key_invright",
    "key_useartifact",
    "key_fire",
    "key_use",
    "key_strafe",
    "key_speed",
    "snd_channels",
    "snd_musicdevice",
    "snd_sfxdevice",
    "usegamma",
    "savedir",
    "messageson",
    "chatmacro0",
    "chatmacro1",
    "chatmacro2",
    "chatmacro3",
    "chatmacro4",
    "chatmacro5",
    "chatmacro6",
    "chatmacro7",
    "chatmacro8",
    "chatmacro9",
};

// TODO: actually store types for known cvars somewhere and check them when
//       loading; maybe use m_config.c for this

struct ConfigVar
{
    int type;
    char name[MAX_CVARLEN];
    union
    {
        char sval[MAX_CVARLEN];
        int ival;
        double dval;
    };
};

struct Config
{
    int extended;
    int modified;
    const char *name;
    struct ConfigVar *cvars;
    int numcvars;
};

static struct Config cfgs[4][2] =
{
    {
        { 0, 0, "default.cfg", NULL, 0 },
        { 1, 0, "chocolate-doom.cfg", NULL, 0 },
    },
    {
        { 0, 0, "heretic.cfg", NULL, 0 },
        { 1, 0, "chocolate-heretic.cfg", NULL, 0 },
    },
    {
        { 0, 0, "hexen.cfg", NULL, 0 },
        { 1, 0, "chocolate-hexen.cfg", NULL, 0 },
    },
    {
        { 0, 0, "strife.cfg", NULL, 0 },
        { 1, 0, "chocolate-strife.cfg", NULL, 0 },
    },
};

static struct Config *game_to_cfg[GAME_COUNT][2] =
{
    // doom
    { &cfgs[0][0], &cfgs[0][1] },
    { &cfgs[0][0], &cfgs[0][1] },
    { &cfgs[0][0], &cfgs[0][1] },
    { &cfgs[0][0], &cfgs[0][1] },
    { &cfgs[0][0], &cfgs[0][1] },
    // heretic
    { &cfgs[1][0], &cfgs[1][1] },
    { &cfgs[1][0], &cfgs[1][1] },
    // hexen
    { &cfgs[2][0], &cfgs[2][1] },
    // strife
    { &cfgs[3][0], &cfgs[3][1] },
};

static int IsExtendedVar(const char *name)
{
    const int n = sizeof(std_vars) / sizeof(std_vars[0]);
    for (int i = 0; i < n; ++i)
        if (!strcmp(std_vars[i], name))
            return 0;
    return 1;
}

static int ConfigLoad(struct Config *cfg)
{
    static char buf[512];
    static char key[MAX_CVARNAME], value[MAX_CVARLEN];
    snprintf(buf, sizeof(buf), VITA_BASEDIR "/%s", cfg->name);

    FILE *f = fopen(buf, "r");
    if (!f) return 1;

    while (!feof(f))
    {
        if (fscanf(f, "%79s %99[^\n]\n", key, value) != 2)
            continue;

        cfg->cvars = realloc(cfg->cvars, sizeof(struct ConfigVar) * (cfg->numcvars + 1));
        if (!cfg->cvars) { fclose(f); return 1; }

        struct ConfigVar *cvar = cfg->cvars + cfg->numcvars++;

        while (strlen(value) > 0 && !isprint(value[strlen(value)-1]))
            value[strlen(value)-1] = '\0';

        strncpy(cvar->name, key, MAX_CVARNAME);

        if (value[0] == '"' && value[1])
        {
            cvar->type = CVAR_STRING;
            value[strlen(value)-1] = '\0';
            strncpy(cvar->sval, value + 1, MAX_CVARLEN);
        }
        else if (strchr(value, '.'))
        {
            cvar->type = CVAR_DOUBLE;
            sscanf(value, "%lf", &cvar->dval);
        }
        else
        {
            cvar->type = CVAR_INTEGER;
            if (value[0] == '0' && value[1] == 'x')
                sscanf(value + 2, "%x", &cvar->ival);
            else
                sscanf(value, "%d", &cvar->ival);
        }
    }

    cfg->modified = 0;
    fclose(f);
    return 0;
}

static int ConfigSave(struct Config *cfg)
{
    static char buf[512];
    snprintf(buf, sizeof(buf), VITA_BASEDIR "/%s", cfg->name);

    FILE *f = fopen(buf, "w");
    if (!f) return 1;

    for (int i = 0; i < cfg->numcvars; ++i)
    {
        struct ConfigVar *cvar = cfg->cvars + i;

        int wc = fprintf(f, "%s ", cvar->name);
        for (; wc < 30; ++wc) fprintf(f, " ");

        if (cvar->type == CVAR_STRING)
        {
            fprintf(f, "\"%s\"", cvar->sval);
        }
        else if (cvar->type == CVAR_DOUBLE)
        {
            fprintf(f, "%f", (float)cvar->dval);
        }
        else if (cvar->type == CVAR_INTEGER)
        {
            // HACK
            if (!strcmp(cvar->name, "opl_io_port"))
                fprintf(f, "0x%x", (unsigned)cvar->ival);
            else
                fprintf(f, "%d", cvar->ival);
        }

        fprintf(f, "\n");
    }

    cfg->modified = 0;
    fclose(f);
    return 0;
}

int CFG_LoadAll(void)
{
    int fail = 0;
    for (int i = 0; i < 4; i++)
    {
        fail += ConfigLoad(&cfgs[i][0]);
        fail += ConfigLoad(&cfgs[i][1]);
    }
    return fail;
}

int CFG_SaveAll(void)
{
    int fail = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!cfgs[i][0].modified) continue;
        fail += ConfigSave(&cfgs[i][0]);
        fail += ConfigSave(&cfgs[i][1]);
    }
    return fail;
}

void CFG_FreeAll(void)
{
    for (int i = 0; i < 4; i++)
    {
        if (cfgs[i][0].numcvars) free(cfgs[i][0].cvars);
        if (cfgs[i][1].numcvars) free(cfgs[i][1].cvars);
        cfgs[i][0].cvars = cfgs[i][1].cvars = NULL;
        cfgs[i][0].numcvars = cfgs[i][1].numcvars = 0;
    }
}

int CFG_Load(int game)
{
    return ConfigLoad(game_to_cfg[game][0]) +
        ConfigLoad(game_to_cfg[game][1]);
}

int CFG_Save(int game)
{
    return ConfigSave(game_to_cfg[game][0]) +
        ConfigSave(game_to_cfg[game][1]);
}

static struct ConfigVar *FindVar(int game, const char *name)
{
    int ext = IsExtendedVar(name);
    struct Config *cfg = game_to_cfg[game][ext];

    for (int i = 0; i < cfg->numcvars; ++i)
    {
        if (!strcmp(cfg->cvars[i].name, name))
            return cfg->cvars + i;
    }

    return NULL;
}

int CFG_ReadVar(int game, const char *name, void *dst)
{
    struct ConfigVar *cvar = FindVar(game, name);

    if (!cvar) return 1;

    switch (cvar->type)
    {
        case CVAR_INTEGER:
            *((int *)dst) = cvar->ival;
            break;
        case CVAR_DOUBLE:
            *((double *)dst) = cvar->dval;
            break;
        case CVAR_STRING:
            strncpy((char *)dst, cvar->sval, MAX_CVARLEN);
            break;
        default:
            return 2;
    }

    return 0;
}

int CFG_WriteVar(int game, const char *name, void *src)
{
    struct ConfigVar *cvar = FindVar(game, name);

    if (!cvar) return 1;

    int modified = 0;

    switch (cvar->type)
    {
        case CVAR_INTEGER:
            modified = cvar->ival != *((int *)src);
            cvar->ival = *((int *)src);
            break;
        case CVAR_DOUBLE:
            modified = cvar->dval != *((double *)src);
            cvar->dval = *((double *)src);
            break;
        case CVAR_STRING:
            modified = strcmp(cvar->sval, (char *)src);
            strncpy(cvar->sval, (char *)src, MAX_CVARLEN);
            break;
        default:
            return 2;
    }

    if (modified)
    {
        int ext = IsExtendedVar(name);
        struct Config *cfg = game_to_cfg[game][ext];
        cfg->modified = 1;
    }

    return 0;
}

int CFG_VarType(int game, const char *name)
{
    struct ConfigVar *cvar = FindVar(game, name);
    if (!cvar) return CVAR_UNKNOWN;
    return cvar->type;
}
