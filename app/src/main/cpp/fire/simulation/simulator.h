//
// Created by Kalle on 2020-03-04.
//

#ifndef DATX02_20_21_SIMULATOR_H
#define DATX02_20_21_SIMULATOR_H

#include <jni.h>
#include <GLES3/gl31.h>
#include <chrono>
#include <fire/settings.h>

#include "simulation_operations.h"
#include "wavelet_turbulence.h"

using std::chrono::time_point;
using std::chrono::system_clock;

class Simulator {
    SlabOperator* slab;
    SimulationOperations* operations;
    WaveletTurbulence* wavelet;

    Settings settings;

    DataTexturePair* density;
    DataTexturePair* temperature;
    DataTexturePair* lowerVelocity;
    DataTexturePair* higherVelocity;

    //Textures for sources
    GLuint densitySource, temperatureSource, velocitySource;

    float windAngle = 3.14f;

    // Time
    time_point<system_clock> start_time, last_time;

public:

    int init(Settings settings);

    int changeSettings(Settings settings);

    void update();

    void getData(GLuint& densityData, GLuint& temperatureData, int& width, int& height, int& depth);

private:

    void initData();

    void clearData();

    // Performs one fire.simulation step for velocity
    void velocityStep(float dt);

    void waveletStep(float dt);

    void updateAndApplyWind(float scale, float dt);

    void temperatureStep( float dt);

    void densityStep(float dt);

    void clearField(float* field, float value, ivec3 gridSize);

    void clearField(vec3* field, vec3 value, ivec3 gridSize);

    // density is in unit/m^3
    void fillExtensive(float* field, float density, vec3 minPos, vec3 maxPos, Resolution res, Settings settings);

    // value is in unit
    void fillIntensive(float* field, float value, vec3 minPos, vec3 maxPos, Resolution res, Settings settings);

    void fillSphere(float* field, float value, vec3 center, float radius, Resolution res, Settings settings);
    void fillSphere(vec3* field, vec3 value, vec3 center, float radius, Resolution res, Settings settingse);

    // fills the field with vectors pointing outward from the center,
    // and that scale with the distance from the center
    // scale is unit/meter from center
    void fillOutgoingVector(vec3* field, float scale, vec3 minPos, vec3 maxPos, Resolution res, Settings settings);

    bool hasOverlap(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

    // might return negative values if there is no overlap
    float getOverlapVolume(vec3 min1, vec3 max1, vec3 min2, vec3 max2);

};

#endif //DATX02_20_21_SIMULATOR_H
