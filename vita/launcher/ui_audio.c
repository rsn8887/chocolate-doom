#include "config.h"
#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuAudio_Init(void);
void UI_MenuAudio_Update(void);
void UI_MenuAudio_Draw(void);

static const char *sample_rates[] = { "11025", "22050", "44100", "48000" };

static const char *music_type_labels[] = { "None", "OPL3", "Digital" };
static const char *music_type_values[] = { "0", "3", "8" };

static const char *sound_type_labels[] = { "None", "PC Speaker", "Digital" };
static const char *sound_type_values[] = { "0", "1", "3" };

static struct Option audio_opts[] =
{
    {
        OPT_CHOICE,
        "Sound type",
        "snd_sfxdevice", NULL,
        .choice =
        {
            sound_type_labels, sound_type_values,
            3, 2,
        },
    },
    {
        OPT_CHOICE,
        "Music type",
        "snd_musicdevice", NULL,
        .choice =
        {
            music_type_labels, music_type_values,
            3, 1,
        },
    },
    {
        OPT_CHOICE,
        "Sample rate",
        "snd_samplerate", NULL,
        .choice =
        {
            sample_rates, sample_rates,
            4, 2,
        },
    },
    {
        OPT_INTEGER,
        "Sound channels",
        "snd_channels", NULL,
        .inum = { 1, 32, 1, 8 },
    },
    {
        OPT_BOOLEAN,
        "Random pitch shifting",
        "snd_pitchshift", NULL,
        .boolean = 0,
    },
};

struct Menu ui_menu_audio =
{
    MENU_AUDIO,
    "Audio",
    "Audio settings",
    audio_opts, 5, 0, 0,
    UI_MenuAudio_Init,
    UI_MenuAudio_Update,
    UI_MenuAudio_Draw,
};

static struct Menu *self = &ui_menu_audio;

void UI_MenuAudio_Init(void)
{

}

void UI_MenuAudio_Update(void)
{

}

void UI_MenuAudio_Draw(void)
{

}
