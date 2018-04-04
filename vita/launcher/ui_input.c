#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuInput_Init(void);
void UI_MenuInput_Update(void);
void UI_MenuInput_Draw(void);

static const char *joy_axis_labels[] =
{
    "Disabled",
    "L Stick X",
    "L Stick Y",
    "R Stick X",
    "R Stick Y",
};

static const char *joy_axis_values[] = { "-1", "0", "1", "2", "3" };

static struct Option input_opts[] =
{
    {
        OPT_CHOICE,
        "Move axis",
        "joystick_y_axis", NULL,
        .choice =
        {
            joy_axis_labels, joy_axis_values,
            5, 2,
        },
    },
    {
        OPT_CHOICE,
        "Strafe axis",
        "joystick_strafe_axis", NULL,
        .choice =
        {
            joy_axis_labels, joy_axis_values,
            5, 1,
        },
    },
    {
        OPT_CHOICE,
        "Turn axis",
        "joystick_x_axis", NULL,
        .choice =
        {
            joy_axis_labels, joy_axis_values,
            5, 3,
        },
    },
    {
        OPT_CHOICE,
        "Look axis",
        "joystick_look_axis", NULL,
        .choice =
        {
            joy_axis_labels, joy_axis_values,
            5, 0,
        },
    },
    {
        OPT_BOOLEAN,
        "Invert look axis",
        "joystick_look_invert", NULL,
        .boolean = 0,
    },
};

struct Menu ui_menu_input =
{
    MENU_INPUT,
    "Input",
    "Input settings",
    input_opts, 5, 0,
    UI_MenuInput_Init,
    UI_MenuInput_Update,
    UI_MenuInput_Draw,
};

static struct Menu *self = &ui_menu_input;

void UI_MenuInput_Init(void)
{

}

void UI_MenuInput_Update(void)
{

}

void UI_MenuInput_Draw(void)
{

}
