//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2018 fgsfds
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL on the Vita.
//


#include "SDL.h"
#include "SDL_opengl.h"
#include "psp2_shader.h"
#include <vita2d.h>

#include "config.h"
#include "d_loop.h"
#include "deh_str.h"
#include "doomtype.h"
#include "i_input.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_timer.h"
#include "i_video.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "tables.h"
#include "v_diskicon.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#define VITA_SCR_W 960
#define VITA_SCR_H 544

// These are (1) the window (or the full screen) that our game is rendered to
// and (2) the renderer that scales the texture (see below) into this window.

static SDL_Window *screen;
static SDL_Renderer *renderer;

// Current vita2d shader, used for sharp bilinear filtering

static vita2d_shader *shader = NULL;

// Window title

static char *window_title = "";

// These are (1) the 320x200x8 paletted buffer that we draw to (i.e. the one
// that holds I_VideoBuffer), (2) the 320x200x32 RGBA intermediate buffer that
// we blit the former buffer to, (3) the intermediate 320x200 texture that we
// load the RGBA buffer to and that we render onto the screen.

static SDL_Surface *screenbuffer = NULL;
static SDL_Surface *argbbuffer = NULL;
static SDL_Texture *texture = NULL;

static SDL_Rect blit_rect = {
    0,
    0,
    SCREENWIDTH,
    SCREENHEIGHT
};

static SDL_Rect target_rect = {
    0,
    0,
    VITA_SCR_W,
    VITA_SCR_H
};

static uint32_t pixel_format;

// palette

static SDL_Color palette[256];
static boolean palette_to_set;

// display has been set up?

static boolean initialized = false;

// disable mouse?

static boolean nomouse = false;
int usemouse = 1;

// Save screenshots in PNG format.

int png_screenshots = 0;

// SDL video driver name

char *video_driver = "";

// Window position:

char *window_position = "center";

// Scaling filter applied

char *scaling_filter = "nearest";

// SDL display number on which to run.

int video_display = 0;

// Screen width and height, from configuration file.

int window_width = SCREENWIDTH * 2;
int window_height = SCREENHEIGHT_4_3 * 2;

// Fullscreen mode, 0x0 for SDL_WINDOW_FULLSCREEN_DESKTOP.

int fullscreen_width = 0, fullscreen_height = 0;

// Maximum number of pixels to use for intermediate scale buffer.

static int max_scaling_buffer_pixels = 16000000;

// Run in full screen mode?  (int type for config code)

int fullscreen = true;

// Aspect ratio correction mode

int aspect_ratio_correct = true;
static int actualheight;

// Force integer scales for resolution-independent rendering

int integer_scaling = false;

// VGA Porch palette change emulation

int vga_porch_flash = false;

// Force software rendering, for systems which lack effective hardware
// acceleration

int force_software_renderer = false;

// Time to wait for the screen to settle on startup before starting the
// game (ms)

static int startup_delay = 1000;

// Grab the mouse? (int type for config code). nograbmouse_override allows
// this to be temporarily disabled via the command line.

static int grabmouse = true;
static boolean nograbmouse_override = false;

// The screen buffer; this is modified to draw things to the screen

pixel_t *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

boolean screensaver_mode = false;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen

boolean screenvisible = true;

// If true, we display dots at the bottom of the screen to 
// indicate FPS.

static boolean display_fps_dots;

// If this is true, the screen is rendered but not blitted to the
// video buffer.

static boolean noblit;

// Callback function to invoke to determine whether to grab the 
// mouse pointer.

static grabmouse_callback_t grabmouse_callback = NULL;

// Does the window currently have focus?

static boolean window_focused = true;

// Window resize state.

static boolean need_resize = false;
static unsigned int last_resize_time;
#define RESIZE_DELAY 500

// Gamma correction level to use

int usegamma = 0;

// Joystick/gamepad hysteresis
unsigned int joywait = 0;

void I_SetGrabMouseCallback(grabmouse_callback_t func)
{
    grabmouse_callback = func;
}

// Set the variable controlling FPS dots.

void I_DisplayFPSDots(boolean dots_on)
{
    display_fps_dots = dots_on;
}

static void SetShowCursor(boolean show)
{
    if (!screensaver_mode)
    {
        // When the cursor is hidden, grab the input.
        // Relative mode implicitly hides the cursor.
        SDL_SetRelativeMouseMode(!show);
        SDL_GetRelativeMouseState(NULL, NULL);
    }
}

void I_ShutdownGraphics(void)
{
    if (initialized)
    {
        SetShowCursor(true);

        SDL_QuitSubSystem(SDL_INIT_VIDEO);

        initialized = false;
    }
}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}

static void HandleWindowEvent(SDL_WindowEvent *event)
{
    int i;

    switch (event->event)
    {
#if 0 // SDL2-TODO
        case SDL_ACTIVEEVENT:
            // need to update our focus state
            UpdateFocus();
            break;
#endif
        case SDL_WINDOWEVENT_EXPOSED:
            palette_to_set = true;
            break;

        case SDL_WINDOWEVENT_RESIZED:
            need_resize = true;
            last_resize_time = SDL_GetTicks();
            break;

        // Don't render the screen when the window is minimized:

        case SDL_WINDOWEVENT_MINIMIZED:
            screenvisible = false;
            break;

        case SDL_WINDOWEVENT_MAXIMIZED:
        case SDL_WINDOWEVENT_RESTORED:
            screenvisible = true;
            break;

        // Update the value of window_focused when we get a focus event
        //
        // We try to make ourselves be well-behaved: the grab on the mouse
        // is removed if we lose focus (such as a popup window appearing),
        // and we dont move the mouse around if we aren't focused either.

        case SDL_WINDOWEVENT_FOCUS_GAINED:
            window_focused = true;
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            window_focused = false;
            break;

        // We want to save the user's preferred monitor to use for running the
        // game, so that next time we're run we start on the same display. So
        // every time the window is moved, find which display we're now on and
        // update the video_display config variable.

        case SDL_WINDOWEVENT_MOVED:
            i = SDL_GetWindowDisplayIndex(screen);
            if (i >= 0)
            {
                video_display = i;
            }
            break;

        default:
            break;
    }
}

extern void I_HandleKeyboardEvent(SDL_Event *sdlevent);
extern void I_HandleMouseEvent(SDL_Event *sdlevent);

// No built-in touch support, so we translate mouse events to CTRL keypresses
// as actual mouse control is not really needed, since we got analogs and dpad

static void TranslateTouchEvent(SDL_Event *ev)
{
    SDL_Event ev_new;

    memset(&ev_new, 0, sizeof(SDL_Event));

    if (ev->tfinger.touchId == 0)
    {
        // front touch
        ev_new.key.keysym.sym = SDLK_TAB;
        ev_new.key.keysym.scancode = SDL_SCANCODE_TAB;
    }
    else
    {
        // back touch
        ev_new.key.keysym.sym = SDLK_PAGEUP;
        ev_new.key.keysym.scancode = SDL_SCANCODE_PAGEUP;
    }

    if (ev->type == SDL_FINGERDOWN)
    {
        ev_new.type = ev_new.key.type = SDL_KEYDOWN;
        ev_new.key.state = SDL_PRESSED;
    }
    else if (ev->type == SDL_FINGERUP)
    {
        ev_new.type = ev_new.key.type = SDL_KEYUP;
        ev_new.key.state = SDL_RELEASED;
    }

    I_HandleKeyboardEvent(&ev_new);
}

// A lot of actions can't be bound to joystick buttons in Chocolate, and in
// Heretic or Hexen joystick doesn't control the menus, so we have to translate
// joystick events to keys instead.

// HACK: I'm too fucking stupid to write a proper makefile that will recompile
//       this with -DHERETIC -DHEXEN or something, so we do this shit instead
extern boolean MenuActive __attribute__((weak));
extern boolean askforquit __attribute__((weak));
extern int messageToPrint __attribute__((weak));

static void TranslateControllerEvent(SDL_Event *ev)
{
    int btn;
    SDL_Event ev_new;
    boolean in_prompt;
    static const struct 
    {
        SDL_Keycode sym;
        SDL_Scancode scan;
    } v_keymap[] = 
    {
        { SDLK_LALT, SDL_SCANCODE_LALT },           // Triangle
        { SDLK_BACKSPACE, SDL_SCANCODE_BACKSPACE }, // Circle
        { SDLK_RETURN, SDL_SCANCODE_RETURN },       // Cross
        { SDLK_SPACE, SDL_SCANCODE_SPACE },         // Square
        { SDLK_LSHIFT, SDL_SCANCODE_LSHIFT },       // L Trigger
        { SDLK_LCTRL, SDL_SCANCODE_LCTRL },         // R Trigger
        { SDLK_DOWN, SDL_SCANCODE_DOWN },           // D-Down
        { SDLK_LEFT, SDL_SCANCODE_LEFT },           // D-Left
        { SDLK_UP, SDL_SCANCODE_UP },               // D-Up
        { SDLK_RIGHT, SDL_SCANCODE_RIGHT },         // D-Right
        { SDLK_DELETE, SDL_SCANCODE_DELETE },       // Select
        { SDLK_ESCAPE, SDL_SCANCODE_ESCAPE },       // Start
    };
    
    memset(&ev_new, 0, sizeof(SDL_Event));

    btn = ev->jbutton.button;
    in_prompt = (&askforquit && askforquit) ||
        (&messageToPrint && messageToPrint);

    if (in_prompt)
    {
        if (btn == 1 || btn == 10)
        {
            ev_new.key.keysym.sym = SDLK_n;
            ev_new.key.keysym.scancode = SDL_SCANCODE_N;
        }
        else if (btn == 2 || btn == 11)
        {
            ev_new.key.keysym.sym = SDLK_y;
            ev_new.key.keysym.scancode = SDL_SCANCODE_Y;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (btn < 0 || btn > 11)
            return;
        ev_new.key.keysym.sym = v_keymap[btn].sym;
        ev_new.key.keysym.scancode = v_keymap[btn].scan;
    }

    if (ev->type == SDL_JOYBUTTONDOWN)
    {
        ev_new.type = ev_new.key.type = SDL_KEYDOWN;
        ev_new.key.state = SDL_PRESSED;
    }
    else if (ev->type == SDL_JOYBUTTONUP)
    {
        ev_new.type = ev_new.key.type = SDL_KEYUP;
        ev_new.key.state = SDL_RELEASED;
    }

    I_HandleKeyboardEvent(&ev_new);
}

void I_GetEvent(void)
{
    SDL_Event sdlevent;

    SDL_PumpEvents();

    while (SDL_PollEvent(&sdlevent))
    {
        switch (sdlevent.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                I_HandleKeyboardEvent(&sdlevent);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL:
                if (usemouse && !nomouse)
                {
                    I_HandleMouseEvent(&sdlevent);
                }
                break;

            // Translate Vita front screen touches into keypresses
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
                TranslateTouchEvent(&sdlevent);
                break;

            // Translate some gamepad events to keys to allow menu navigation
            // in games that don't support joystick menu controls (Heretic, Hexen)
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                TranslateControllerEvent(&sdlevent);
                break;

            case SDL_QUIT:
                if (screensaver_mode)
                {
                    I_Quit();
                }
                else
                {
                    event_t event;
                    event.type = ev_quit;
                    D_PostEvent(&event);
                }
                break;

            case SDL_WINDOWEVENT:
                if (sdlevent.window.windowID == SDL_GetWindowID(screen))
                {
                    HandleWindowEvent(&sdlevent.window);
                }
                break;

            default:
                break;
        }
    }
}

//
// I_StartTic
//
void I_StartTic (void)
{
    if (!initialized)
    {
        return;
    }

    I_GetEvent();

    if (usemouse && !nomouse)
    {
        I_ReadMouse();
    }

    if (joywait < I_GetTime())
    {
        I_UpdateJoystick();
    }
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

// TODO: accelerate this
static inline void BlitBuffer(void)
{
  SDL_Color c;
  uint8_t *src, *dst, *end;

  src = screenbuffer->pixels;
  dst = argbbuffer->pixels;
  end = src + SCREENWIDTH * SCREENHEIGHT;
  while (src != end)
  {
    c = palette[*(src++)];
    // ABGR
    *(dst++) = c.r;
    *(dst++) = c.g;
    *(dst++) = c.b;
    *(dst++) = 255;
  }
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
    static int lasttic;
    int tics;
    int i;

    if (!initialized)
        return;

    if (noblit)
        return;

    // draws little dots on the bottom of the screen

    if (display_fps_dots)
    {
        i = I_GetTime();
        tics = i - lasttic;
        lasttic = i;
        if (tics > 20) tics = 20;

        for (i=0 ; i<tics*4 ; i+=4)
            I_VideoBuffer[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
        for ( ; i<20*4 ; i+=4)
            I_VideoBuffer[ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
    }

    // Draw disk icon before blit, if necessary.
    V_DrawDiskIcon();

    if (palette_to_set)
    {
        SDL_SetPaletteColors(screenbuffer->format->palette, palette, 0, 256);
        palette_to_set = false;

        if (vga_porch_flash)
        {
            // "flash" the pillars/letterboxes with palette changes, emulating
            // VGA "porch" behaviour (GitHub issue #832)
            SDL_SetRenderDrawColor(renderer, palette[0].r, palette[0].g,
                palette[0].b, SDL_ALPHA_OPAQUE);
        }
    }

    // Blit from the paletted 8-bit screen buffer to the intermediate
    // 32-bit RGBA buffer that we can load into the texture.

    // Have to blit manually here because it seems LowerBlit is bugged on Vita.
    BlitBuffer();

    // Update the intermediate texture with the contents of the RGBA buffer.

    SDL_UpdateTexture(texture, NULL, argbbuffer->pixels, argbbuffer->pitch);

    // Make sure the pillarboxes are kept clear each frame.

    SDL_RenderClear(renderer);

    // Just render the intermediate texture onto the screen, upscaling it.

    SDL_RenderCopy(renderer, texture, &blit_rect, &target_rect);

    // Draw!

    SDL_RenderPresent(renderer);

    // Restore background and undo the disk indicator, if it was drawn.
    V_RestoreDiskBackground();
}


//
// I_ReadScreen
//
void I_ReadScreen (pixel_t* scr)
{
    memcpy(scr, I_VideoBuffer, SCREENWIDTH*SCREENHEIGHT*sizeof(*scr));
}


//
// I_SetPalette
//
void I_SetPalette (byte *doompalette)
{
    int i;

    for (i=0; i<256; ++i)
    {
        // Zero out the bottom two bits of each channel - the PC VGA
        // controller only supports 6 bits of accuracy.

        palette[i].r = gammatable[usegamma][*doompalette++] & ~3;
        palette[i].g = gammatable[usegamma][*doompalette++] & ~3;
        palette[i].b = gammatable[usegamma][*doompalette++] & ~3;
    }

    palette_to_set = true;
}

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex(int r, int g, int b)
{
    int best, best_diff, diff;
    int i;

    best = 0; best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
        diff = (r - palette[i].r) * (r - palette[i].r)
             + (g - palette[i].g) * (g - palette[i].g)
             + (b - palette[i].b) * (b - palette[i].b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

// 
// Set the window title
//

void I_SetWindowTitle(char *title)
{
    window_title = title;
}

//
// Call the SDL function to set the window title, based on 
// the title set with I_SetWindowTitle.
//

void I_InitWindowTitle(void)
{

}

// Set the application icon

void I_InitWindowIcon(void)
{

}

static void CalculateTargetRect(void)
{
    double aspect;
    int scale;

    if (integer_scaling)
    {
        scale = VITA_SCR_H / actualheight;
        target_rect.w = SCREENWIDTH * scale;
        target_rect.h = actualheight * scale;
        target_rect.y = VITA_SCR_H / 2 - target_rect.h / 2;
    }
    else
    {
        aspect = (double)SCREENWIDTH / (double)actualheight;
        target_rect.h = VITA_SCR_H;
        target_rect.w = VITA_SCR_H * aspect;
        target_rect.y = 0;
    }

    target_rect.x = VITA_SCR_W / 2 - target_rect.w / 2;
}

void I_GetWindowPosition(int *x, int *y, int w, int h)
{
    *x = *y = 0;
}

void I_GraphicsCheckCommandLine(void)
{
    //!
    // @category video
    // @vanilla
    //
    // Disable blitting the screen.
    //

    noblit = M_CheckParm ("-noblit");

    //!
    // @category video 
    //
    // Don't grab the mouse when running in windowed mode.
    //

    nograbmouse_override = M_ParmExists("-nograbmouse");

    //!
    // @category video 
    //
    // Disable the mouse.
    //

    nomouse = M_CheckParm("-nomouse") > 0;
}

// Check if we have been invoked as a screensaver by xscreensaver.

void I_CheckIsScreensaver(void)
{
    screensaver_mode = false;
}

static void SetVideoMode(void)
{
    int w, h;
    int x, y;
    Uint32 rmask, gmask, bmask, amask;
    int unused_bpp;
    int window_flags = 0, renderer_flags = 0;
    SDL_DisplayMode mode;

    w = window_width = VITA_SCR_W;
    h = window_height = VITA_SCR_H;

    window_flags = SDL_WINDOW_SHOWN;
    fullscreen = true;

    x = SDL_WINDOWPOS_UNDEFINED;
    y = SDL_WINDOWPOS_UNDEFINED;

    if (screen == NULL)
    {
        screen = SDL_CreateWindow(NULL, x, y, w, h, window_flags);

        if (screen == NULL)
        {
            I_Error("Error creating window for video startup: %s",
            SDL_GetError());
        }

        pixel_format = SDL_PIXELFORMAT_ABGR8888;
    }

    // We don't use render-to-texture on the vita
    renderer_flags = 0;

    if (SDL_GetCurrentDisplayMode(video_display, &mode) != 0)
    {
        I_Error("Could not get display mode for video display #%d: %s",
        video_display, SDL_GetError());
    }

    // Turn on vsync if we aren't in a -timedemo
    if (!singletics && mode.refresh_rate > 0)
    {
        renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    if (force_software_renderer)
    {
        renderer_flags |= SDL_RENDERER_SOFTWARE;
        renderer_flags &= ~SDL_RENDERER_PRESENTVSYNC;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
    }

    renderer = SDL_CreateRenderer(screen, -1, renderer_flags);

    if (renderer == NULL)
    {
        I_Error("Error creating renderer for screen window: %s",
                SDL_GetError());
    }

    // Set the scaling quality for rendering and immediate texture.
    // Defaults to "nearest", which is gritty and pixelated and resembles
    // software scaling pretty well.  "linear" can be set as an alternative,
    // which may give better results at low resolutions.

    if (!strcmp(scaling_filter, "linear"))
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    }
    else if (!strcmp(scaling_filter, "sharp"))
    {
        // For the sharp_bilinear_simple shader to work, linear filtering has to be enabled.
        // This is done by a simple command here, supported by SDL2 for Vita since 2017/12/24.
        // This affects all textures created after this command.
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        // Enable sharp-bilinear-simple shader for sharp pixels without distortion.
        // This has to be done after the SDL renderer is created because that inits vita2d.
        shader = setPSP2Shader(SHARP_BILINEAR_SIMPLE);
    }
    else
    {
        scaling_filter = "nearest";
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    }

    // Blank out the full screen area in case there is any junk in
    // the borders that won't otherwise be overwritten.

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Create the 8-bit paletted and the 32-bit RGBA screenbuffer surfaces.

    if (screenbuffer == NULL)
    {
        screenbuffer = SDL_CreateRGBSurface(0,
                                            SCREENWIDTH, SCREENHEIGHT, 8,
                                            0, 0, 0, 0);
        SDL_FillRect(screenbuffer, NULL, 0);
    }

    // Format of argbbuffer must match the screen pixel format because we
    // import the surface data into the texture.
    if (argbbuffer == NULL)
    {
        SDL_PixelFormatEnumToMasks(pixel_format, &unused_bpp,
                                   &rmask, &gmask, &bmask, &amask);
        argbbuffer = SDL_CreateRGBSurface(0,
                                          SCREENWIDTH, SCREENHEIGHT, 32,
                                          rmask, gmask, bmask, amask);
        SDL_FillRect(argbbuffer, NULL, 0);
    }

    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
    }

    // Create the intermediate texture that the RGBA surface gets loaded into.
    // The SDL_TEXTUREACCESS_STREAMING flag means that this texture's content
    // is going to change frequently.

    texture = SDL_CreateTexture(renderer,
                                pixel_format,
                                SDL_TEXTUREACCESS_STREAMING,
                                SCREENWIDTH, SCREENHEIGHT);

    if (!texture) 
        I_Error("Error creating screen texture: %s", SDL_GetError());

    CalculateTargetRect();
}

static const char *hw_emu_warning = 
"===========================================================================\n"
"WARNING: it looks like you are using a software GL implementation.\n"
"To improve performance, try setting force_software_renderer in your\n"
"configuration file.\n"
"===========================================================================\n";

static void CheckGLVersion(void)
{
    const char * version;
    typedef const GLubyte* (APIENTRY * glStringFn_t)(GLenum);
    glStringFn_t glfp = (glStringFn_t)SDL_GL_GetProcAddress("glGetString");

    if (glfp)
    {
        version = (const char *)glfp(GL_VERSION);

        if (version && strstr(version, "Mesa"))
        {
            printf("%s", hw_emu_warning);
        }
    }
}

void I_InitGraphics(void)
{
    SDL_Event dummy;
    byte *doompal;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        I_Error("Failed to initialize video: %s", SDL_GetError());
    }

    fullscreen = true;

    if (aspect_ratio_correct)
    {
        actualheight = SCREENHEIGHT_4_3;
    }
    else
    {
        actualheight = SCREENHEIGHT;
    }

    // Create the game window; this may switch graphic modes depending
    // on configuration.
    SetVideoMode();

    // We might have poor performance if we are using an emulated
    // HW accelerator. Check for Mesa and warn if we're using it.
    CheckGLVersion();

    // Start with a clear black screen
    // (screen will be flipped after we set the palette)

    SDL_FillRect(screenbuffer, NULL, 0);

    // Set the palette

    doompal = W_CacheLumpName(DEH_String("PLAYPAL"), PU_CACHE);
    I_SetPalette(doompal);
    SDL_SetPaletteColors(screenbuffer->format->palette, palette, 0, 256);

    // On some systems, it takes a second or so for the screen to settle
    // after changing modes.  We include the option to add a delay when
    // setting the screen mode, so that the game doesn't start immediately
    // with the player unable to see anything.

    if (fullscreen && !screensaver_mode)
    {
        SDL_Delay(startup_delay);
    }

    // The actual 320x200 canvas that we draw to. This is the pixel buffer of
    // the 8-bit paletted screen buffer that gets blit on an intermediate
    // 32-bit RGBA screen buffer that gets loaded into a texture that gets
    // finally rendered into our window or full screen in I_FinishUpdate().

    I_VideoBuffer = screenbuffer->pixels;
    V_RestoreBuffer();

    // Clear the screen to black.

    memset(I_VideoBuffer, 0, SCREENWIDTH * SCREENHEIGHT);

    // clear out any events waiting at the start and center the mouse
  
    while (SDL_PollEvent(&dummy));

    initialized = true;

    // Call I_ShutdownGraphics on quit

    I_AtExit(I_ShutdownGraphics, true);
}

// Bind all variables controlling video options into the configuration
// file system.
void I_BindVideoVariables(void)
{
    M_BindIntVariable("use_mouse",                 &usemouse);
    M_BindIntVariable("fullscreen",                &fullscreen);
    M_BindIntVariable("video_display",             &video_display);
    M_BindIntVariable("aspect_ratio_correct",      &aspect_ratio_correct);
    M_BindIntVariable("integer_scaling",           &integer_scaling);
    M_BindIntVariable("vga_porch_flash",           &vga_porch_flash);
    M_BindIntVariable("startup_delay",             &startup_delay);
    M_BindIntVariable("fullscreen_width",          &fullscreen_width);
    M_BindIntVariable("fullscreen_height",         &fullscreen_height);
    M_BindIntVariable("force_software_renderer",   &force_software_renderer);
    M_BindIntVariable("max_scaling_buffer_pixels", &max_scaling_buffer_pixels);
    M_BindIntVariable("window_width",              &window_width);
    M_BindIntVariable("window_height",             &window_height);
    M_BindIntVariable("grabmouse",                 &grabmouse);
    M_BindStringVariable("video_driver",           &video_driver);
    M_BindStringVariable("window_position",        &window_position);
    M_BindIntVariable("usegamma",                  &usegamma);
    M_BindIntVariable("png_screenshots",           &png_screenshots);
    M_BindStringVariable("scaling_filter",         &scaling_filter);
}
