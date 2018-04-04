#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuVideo_Init(void);
void UI_MenuVideo_Update(void);
void UI_MenuVideo_Draw(void);

static struct Option video_opts[] =
{
    {
        OPT_BOOLEAN,
        "4:3 aspect ratio",
        "aspect_ratio_correct", NULL,
        .boolean = 1,
    },
    {
        OPT_BOOLEAN,
        "Integer scaling",
        "integer_scaling", NULL,
        .boolean = 0,
    },
    {
        OPT_BOOLEAN,
        "VGA porch flash",
        "vga_porch_flash", NULL,
        .boolean = 0,
    },
};

struct Menu ui_menu_video =
{
    MENU_VIDEO,
    "Video",
    "Video settings",
    video_opts, 3, 0, 0,
    UI_MenuVideo_Init,
    UI_MenuVideo_Update,
    UI_MenuVideo_Draw,
};

static struct Menu *self = &ui_menu_video;

void UI_MenuVideo_Init(void)
{

}

void UI_MenuVideo_Update(void)
{

}

void UI_MenuVideo_Draw(void)
{

}
