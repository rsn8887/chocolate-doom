//
// Adapted for vanilla c by rsn8887 on 11/13/2017
// Created by cpasjuste on 18/12/16.
//

// use https://github.com/frangarcj/vita2dlib/tree/fbo
// and https://github.com/frangarcj/vita-shader-collection/releases

#ifndef PSP2_SHADER_H
#define PSP2_SHADER_H

#include <vita2d.h>

typedef enum VitaShader
{
    VSH_NONE = 0,
    VSH_LCD3X,
    VSH_SCALE2X,
    VSH_AAA,
    VSH_SHARP_BILINEAR,
    VSH_SHARP_BILINEAR_SIMPLE,
    VSH_FXAA,
} VitaShader;

void Vita_ClearShader(vita2d_shader *shader);
vita2d_shader *Vita_SetShader(VitaShader shaderType);

#endif //PSP2_SHADER_H
