//
// Created by Anton Forsberg on 18/02/2020.
//

#include "helper.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <stdlib.h>
#include <stdio.h>

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "file_loader.h"

#define LOG_TAG "helper"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


using namespace glm;

bool checkGlError(const char *funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

char *loadFileToMemory(AAssetManager *mgr, const char *filename) {

    // Open your file
    AAsset *file = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
    // Get the file length
    off_t fileLength = AAsset_getLength(file);

    // Allocate memory to read your file
    char *fileContent = new char[fileLength];

    // Read your file
    float error = AAsset_read(file, fileContent, fileLength);

    if (error < fileLength || error == 0) {
        std::cout << "Failed to load image: " << filename << ".\n";
        ALOGE("Failed to load image: ");
    }

    return fileContent;
}

vec3 corners[8] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0},
                   {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

vec3 seed = vec3(rand()%10000, rand()%10000, rand()%10000);

#define PI 3.1415

int angle[256] = {
		163,	 81,	 50,	 50,	 92,	164,	154,	 19,	274,	221,	 16,	174,	266,	 59,	  4,	302,
		65,	171,	124,	 96,	306,	230,	  7,	314,	241,	247,	142,	 81,	193,	 92,	267,	227,
		1,	257,	315,	118,	108,	  3,	120,	211,	 36,	 44,	 29,	256,	179,	331,	78 ,	 31,
		317,	 29,	345,	288,	253,	187,	227,	 60,	 48,	 78,	208,	211,	 79,	239,	123,	127,
		3,	277,	238,	271,	  8,	194,	252,	263,	 32,	301,	249,	 98,	136,	 98,	208,	206,
		344,	170,	249,	250,	197,	342,	325,	228,	 55,	196,	339,	358,	205,	 59,	  4,	125,
		122,	247,	 50,	161,	  7,	 80,	349,	247,	 87,	278,	314,	302,	326,	204,	271,	139,
		77,	330,	143,	236,	275,	245,	131,	100,	 94,	 44,	122,	202,	353,	295,	121,	 22,
		0,	157,	 64,	255,	329,	 55,	332,	 52,	 61,	192,	125,	 43,	 56,	270,	356,	263,
		30,	 50,	236,	156,	135,	 17,	 39,	148,	 27,	 28,	348,	233,	313,	 52,	245,	126,
		282,	299,	305,	252,	258,	 99,	207,	 33,	201,	198,	141,	222,	115,	298,	114,	 38,
		327,	274,	 63,	 76,	 68,	308,	138,	326,	278,	338,	331,	308,	347,	281,	 31,	320,
		322,	168,	301,	228,	185,	 57,	297,	 35,	 94,	346,	177,	 75,	344,	255,	150,	 77,
		242,	  7,	 88,	 19,	 79,	206,	211,	188,	255,	279,	320,	133,	226,	344,	209,	273,
		75,	 67,	221,	209,	 25,	324,	 35,	 11,	  1,	184,	273,	102,	302,	268,	169,	290,
		241,	283,	 42,	305,	 18,	255,	317,	316,	241,	154,	 40,	258,	128,	187,	 45,	355
};

double fade(double x){
    return x * x * x * (x * (x * 6 - 15) + 10);
}
//linear interpolation with smoothstep
double lerp(double a, double b, double t){
    return (a + t * (b - a));
}

//Get perlin noise from 2d value
double perlin(double x, double y, double z){
    vec3 internal_position = vec3(x - floor(x), y - floor(y), z - floor(z)); 																		//Intern position (0.0, 0.0 to 1.0, 1.0)
    vec3 external_position = vec3(floor(x), floor(y), floor(z));     																	//Extern position (-infinity, -infinity to +infinity, +infinity)
    double dots[8] = {0};																					//Declare 4 dot products, one for each corner
    //Loop through all corners
    for (int c = 0; c < 8; c++){
        vec3 external_corner = external_position + corners[c] + seed;																			//Extern corner coordinate
		//Get gradient angle from sum of extern corner values (x, y)
		double yaw   = (angle[int(external_corner.x*external_corner.x+external_corner.y)%256]
					 +  angle[int(external_corner.y*external_corner.y+external_corner.x)%256]) / 180.0 * PI;
		double pitch = (angle[int(external_corner.x*external_corner.x+external_corner.z)%256]
					 +  angle[int(external_corner.z*external_corner.z+external_corner.x)%256]) / 180.0 * PI;
        dots[c] = dot(vec3(cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), cos(pitch)), internal_position - corners[c]);												//Get dot product from distance and gradient vector
    }
    //Interpolate dot product values with common y value
    //then interpolate the interpolated values and return
    return  lerp(   lerp(   lerp(dots[0], dots[1], fade(internal_position.x)),
                            lerp(dots[2], dots[3], fade(internal_position.x)),
                                                   fade(internal_position.y)),
                    lerp(   lerp(dots[4], dots[5], fade(internal_position.x)),
                            lerp(dots[6], dots[7], fade(internal_position.x)),
                                                   fade(internal_position.y)),
                                                   fade(internal_position.z));
}

vec4 noise(double x, double y, double z){
	double g = 0;
	double a = 0;
	for(int octave = 2; octave <= 5; octave++)
		g += 1.0/(octave+2) * perlin(octave*x+3213, octave*y+939, octave*z+2425);
	for(int octave = 1; octave <= 5; octave++)
		a += 1.0/(octave+1) * perlin(octave*x, octave*y, octave*z);
    return vec4(1.0f, (g+0.5f)/2, 0.0f, std::max(0.25f*round((a+0.5f)), 0.0));
}

void createScalar3DTexture(GLuint *id, int width, int height, int depth, float* data){

    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_3D, *id);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void createVector3DTexture(GLuint *id, int width, int height, int depth, vec3* data){

    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_3D, *id);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, width, height, depth, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void generate3DTexture(GLuint *textureID, GLsizei width, GLsizei height, GLsizei depth){
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_3D, *textureID);

    vec4* data = new vec4[width * height * depth];

    LOGE("Generating 3D texture");

    for(int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                data[z * height * width + y * width + x] = noise((double) x / width,
                                                                 (double) y / height,
                                                                 (double) z / depth);
                //data[z * height * width + y * width + x] = vec4(1.0f, 0.0f, 1.0f, 0.24f);
            }
        }
    }
    LOGE("GENERATION DONE");

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
}

void load3DTexture(AAssetManager *mgr, const char *filename, GLsizei width, GLsizei height,
                   GLsizei depth,GLuint *volumeTexID) {
   const char *fileContent = loadFileToMemory(mgr, filename);

    if (*volumeTexID == UINT32_MAX) {
        glGenTextures(1, volumeTexID);
    }

    glBindTexture(GL_TEXTURE_3D, *volumeTexID);

    glTexImage3D(GL_TEXTURE_3D,
                 0,
                 GL_R8,
                 width,
                 height,
                 depth,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 fileContent);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glGenerateMipmap( GL_TEXTURE_3D );

    // Free the memoery you allocated earlier
    delete[] fileContent;
}
