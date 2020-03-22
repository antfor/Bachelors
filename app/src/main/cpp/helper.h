//
// Created by Anton Forsberg on 18/02/2020.
//

#ifndef DATX02_20_21_HELPER_H
#define DATX02_20_21_HELPER_H

#include <GLES3/gl31.h>
#include <android/asset_manager.h>

#include <glm/glm.hpp>

using namespace glm;

bool checkGlError(const char *funcName);

double fade(double x);

double lerp(double a, double b, double t);

double perlin(double x, double y, double z);

vec4 noise(double x, double y, double z);

void generate3DTexture(GLuint *textureID, GLsizei width, GLsizei height, GLsizei depth);

void create3DTexture(GLuint *id, int width, int height, int depth, vec4* data);

void load3DTexture(AAssetManager *mgr, const char *filename, GLsizei width, GLsizei height,
                   GLsizei depth,GLuint *volumeTexID);

#endif //DATX02_20_21_HELPER_H
