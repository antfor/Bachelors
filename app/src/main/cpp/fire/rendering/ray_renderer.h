//
// Created by Anton Forsberg on 22/02/2020.
//

#ifndef DATX02_20_21_RAY_RENDERER_H
#define DATX02_20_21_RAY_RENDERER_H

#include <jni.h>
#include <GLES3/gl31.h>
#include <glm/glm.hpp>
#include <chrono>

#include <android/asset_manager.h>
#include <fire/settings.h>

#include "fire/util/shader.h"
#include "fire/util/framebuffer.h"

using namespace glm;

using std::chrono::time_point;
using std::chrono::system_clock;

class RayRenderer {
    int window_width, window_height;
    int sim_width, sim_height, max_sim_res;

    int texture_width, texture_height, texture_depth;
    GLint threads;

    Settings* settings;

    // Framebuffers
    Framebuffer *back_FBO;
    Framebuffer *front_FBO;

    GLuint ssbo;

    // Cube Buffers
    GLuint VAO;      // Vertex Array Object
    GLuint VBO, EBO; // Vertex Buffer Object && Element Buffer Object
    vec3 boundingScale;

    //QuadBuffer
    GLuint quad_VAO;      // Vertex Array Object
    GLuint quad_VBO, quad_EBO; // Vertex Buffer Object && Element Buffer Object

    // 3D texture
    GLuint temperatureTexID;
    GLuint densityTexID;

    // texture
    GLuint maxTexID;

    // rotation
    double rx;
    double ry;

    // Shaders
    Shader frontFaceShader, backFaceShader, quadShader, maxCompShader;

    // Time
    time_point<system_clock> start_time, last_time;

    vec3 worldUp = {0.0f, 1.0f, 0.0f};
public:
    int init(Settings* settings);

    int changeSettings(Settings* settings);

    void resize(int width, int height);

    void step(GLuint density, GLuint temperature, ivec3 size);

    void touch(double dx, double dy);

private:

    void initSSBO();

    Framebuffer* updateFBO(Framebuffer *FBO, int window_width, int window_height, GLuint internal_format,
                           GLuint type);
    void resizeMaxTexture();

    void initCube(GLuint &VAO, GLuint &VBO, GLuint &EBO);

    void initQuad(GLuint &VAO, GLuint &VBO, GLuint &EBO);

    int initProgram();

    void setData(GLuint density, GLuint temperature, ivec3 size);

    void simScale();

    void resizeSim();

    void loadMVP(Shader shader, float current_time);

};

#endif //DATX02_20_21_RAY_RENDERER_H
