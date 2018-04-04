#pragma once

#define MAX_CHOICES 8
#define MAX_STROPT 100

enum Menus
{
    MENU_MAIN,
    MENU_VIDEO,
    MENU_AUDIO,
    MENU_INPUT,
    MENU_PWADS,

    MENU_COUNT
};

enum OptionTypes
{
    OPT_BOOLEAN,
    OPT_CHOICE,
    OPT_INTEGER,
    OPT_DOUBLE,
    OPT_STRING,
    OPT_BUTTON,
};

struct Option
{
    int type;
    const char *name;
    const char *cfgvar;
    void *codevar;

    union
    {
        int boolean;

        struct
        {
            const char **names;
            const char **values;
            int count;
            int val;
        } choice;

        struct
        {
            double min;
            double max;
            double step;
            double val;
        } dnum;

        struct
        {
            int min;
            int max;
            int step;
            int val;
        } inum;

        char string[MAX_STROPT];

        int button;
    };
};

struct Menu
{
    int id;
    const char *tabname;
    const char *title;

    struct Option *opts;
    int numopts;
    int sel;

    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
};

extern int ui_current_menu;
extern int ui_game;

int UI_Init(void);
int UI_Update(void);
void UI_Draw(void);
void UI_Redraw(void);
void UI_Free(void);

void UI_ReloadOptions(void);
void UI_SaveOptions(void);
