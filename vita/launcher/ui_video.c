#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuVideo_Init(void);
void UI_MenuVideo_Update(void);
void UI_MenuVideo_Draw(void);

static const char *aspect_labels[] = { "8:5", "4:3", "Fit to screen" };
static const char *aspect_values[] = { "0", "1", "2" };

static const char *filter_labels[] = 
{ 
    "None",
    "Bilinear",
    "Sharp bilinear",
    "Scale2x",
};
static const char *filter_values[] =
{ 
    "nearest",
    "linear",
    "sharp",
    "scale2x",
};

static struct Option video_opts[] =
{
    {
        OPT_CHOICE,
        "Aspect ratio",
        "aspect_ratio_correct", NULL,
        .choice =
        {
            aspect_labels, aspect_values,
            3, 0,
        },
    },
    {
        OPT_BOOLEAN,
        "Integer scaling",
        "integer_scaling", NULL,
        .boolean = 0,
    },
    {
        OPT_CHOICE,
        "Scaling filter",
        "scaling_filter", NULL,
        .choice =
        {
            filter_labels, filter_values,
            4, 2,
        },
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
    video_opts, 4, 0, 0,
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
