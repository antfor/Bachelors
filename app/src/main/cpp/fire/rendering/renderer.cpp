#include "renderer.h"

#include <jni.h>
#include <string>

#include <time.h>
#include <math.h>
#include <chrono>

#include <stdlib.h>
#include <utility>

#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

/*
 Water. Earth. Fire. Air.
 Long ago, the four nations lived together in harmony.
 Then everything changed when the Fire Nation attacked.
 Only the Avatar, master of all four elements,
 could stop them. But when the world needed him most,
 he vanished. A hundred years passed and my brother and
 I discovered the new Avatar, an airbender named Aang,
 and although his airbending skills are great, he still
 has a lot to learn before he's ready to save anyone.
 But I believe Aang can save the world.
*/

int Renderer::init(Settings* settings) {
    rayRenderer = new RayRenderer;
    return rayRenderer->init(settings);
}

int Renderer::changeSettings(Settings* settings) {
    return rayRenderer->changeSettings(settings);
}

void Renderer::resize(int width, int height){
    rayRenderer->resize(width, height);
}

void Renderer::update(GLuint density, GLuint temperature, ivec3 size) {
    rayRenderer->step(density, temperature, size);
}

void Renderer::scale(float scaleFactor, double scaleX, double scaleY){
    // TODO implement
}

void Renderer::touch(double dx, double dy){
    // TODO implement
    rayRenderer->touch(dx, dy);
}
