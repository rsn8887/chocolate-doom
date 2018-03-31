#include "config.h"
#include "utils.h"
#include "input.h"

static SceCtrlData pad, pad_old;

int IN_Init(void)
{
    sceCtrlPeekBufferPositive(0, &pad, 1);
    return 0;
}

void IN_Update(void)
{
    pad_old = pad;
    sceCtrlPeekBufferPositive(0, &pad, 1);
}

void IN_Free(void)
{
}

static inline int TranslateButton(int btn)
{
    static const int bmap[16] =
    {
        SCE_CTRL_TRIANGLE,
        SCE_CTRL_CIRCLE,
        SCE_CTRL_CROSS,
        SCE_CTRL_SQUARE,
        SCE_CTRL_LTRIGGER,
        SCE_CTRL_RTRIGGER,
        SCE_CTRL_DOWN,
        SCE_CTRL_LEFT,
        SCE_CTRL_UP,
        SCE_CTRL_RIGHT,
        SCE_CTRL_SELECT,
        SCE_CTRL_START,
    };

   if (btn < 0 || btn > B_START) return -1;
   return bmap[btn];
}

int IN_ButtonPressed(int btn)
{
    btn = TranslateButton(btn);
    return (pad.buttons & btn) && !(pad_old.buttons & btn);
}

void IN_WaitForButton(int btn)
{
    do
    {
        IN_Update();
    }
    while (!IN_ButtonPressed(btn));
}

int IN_GetFirstButton(void)
{
    while (1)
    {
        IN_Update();
        for (int b = B_TRIANGLE; b < B_START; ++b)
        {
            if (IN_ButtonPressed(b))
                return b;
        }
    }

    return -1;
}
