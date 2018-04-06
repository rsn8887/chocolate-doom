//
// Adapted for vanilla c by rsn8887 on 11/13/2017
// Created by cpasjuste on 18/12/16.
//

// use https://github.com/frangarcj/vita2dlib/tree/fbo
// and https://github.com/frangarcj/vita-shader-collection/releases

#include "shader.h"

#include "shaders/lcd3x_v.h"
#include "shaders/lcd3x_f.h"
#include "shaders/gtu_v.h"
#include "shaders/gtu_f.h"
#include "shaders/texture_v.h"
#include "shaders/texture_f.h"
#include "shaders/opaque_v.h"
#include "shaders/bicubic_f.h"
#include "shaders/xbr_2x_v.h"
#include "shaders/xbr_2x_f.h"
#include "shaders/xbr_2x_fast_v.h"
#include "shaders/xbr_2x_fast_f.h"
#include "shaders/advanced_aa_v.h"
#include "shaders/advanced_aa_f.h"
#include "shaders/scale2x_f.h"
#include "shaders/scale2x_v.h"
#include "shaders/sharp_bilinear_f.h"
#include "shaders/sharp_bilinear_v.h"
#include "shaders/sharp_bilinear_simple_f.h"
#include "shaders/sharp_bilinear_simple_v.h"
// #include "shaders/xbr_2x_noblend_f.h"
// #include "shaders/xbr_2x_noblend_v.h"
#include "shaders/fxaa_v.h"
#include "shaders/fxaa_f.h"
#include "shaders/crt_easymode_f.h"


vita2d_shader *Vita_SetShader(VitaShader shaderType)
{
    vita2d_shader *shader;
    switch (shaderType)
    {
        case VSH_LCD3X:
            shader = vita2d_create_shader((SceGxmProgram *) lcd3x_v, (SceGxmProgram *) lcd3x_f);
            break;
        case VSH_SCALE2X:
            shader = vita2d_create_shader((SceGxmProgram *) scale2x_v, (SceGxmProgram *) scale2x_f);
            break;
        case VSH_AAA:
            shader = vita2d_create_shader((SceGxmProgram *) advanced_aa_v, (SceGxmProgram *) advanced_aa_f);
            break;
        case VSH_SHARP_BILINEAR:
            shader = vita2d_create_shader((SceGxmProgram *) sharp_bilinear_v, (SceGxmProgram *) sharp_bilinear_f);
            break;
        case VSH_SHARP_BILINEAR_SIMPLE:
            shader = vita2d_create_shader((SceGxmProgram *) sharp_bilinear_simple_v, (SceGxmProgram *) sharp_bilinear_simple_f);
            break;
        case VSH_FXAA:
            shader = vita2d_create_shader((SceGxmProgram *) fxaa_v, (SceGxmProgram *) fxaa_f);
            break;
        default:
            shader = vita2d_create_shader((SceGxmProgram *) texture_v, (SceGxmProgram *) texture_f);
            break;
    }

    vita2d_texture_set_program(shader->vertexProgram, shader->fragmentProgram);
    vita2d_texture_set_wvp(shader->wvpParam);
    vita2d_texture_set_vertexInput(&shader->vertexInput);
    vita2d_texture_set_fragmentInput(&shader->fragmentInput);

    for(int i = 0; i < 3; i++)
    {
        vita2d_start_drawing();
        vita2d_clear_screen();
        vita2d_wait_rendering_done();
        vita2d_swap_buffers();
    }

    return shader;
}

void Vita_ClearShader(vita2d_shader *shader)
{
    if (shader != NULL)
    {
        vita2d_wait_rendering_done();
        vita2d_free_shader(shader);
    }
}
