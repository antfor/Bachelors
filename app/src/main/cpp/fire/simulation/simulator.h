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
    SlabOperation* slab;
    SimulationOperations* operations;
    WaveletTurbulence* wavelet;

    DataTexturePair* smokeDensity;
    DataTexturePair* temperature;
    DataTexturePair* lowerVelocity;
    DataTexturePair* higherVelocity;

    //Textures for sources
    GLuint densitySource, temperatureSource, velocitySource;

    float dt;

    float buoyancyScale;

    float velKinematicViscosity;
    int velDiffusionIterations;

    float vorticityScale;

    int projectionIterations;

    SourceMode sourceMode;

    float tempKinematicViscosity;
    int tempDiffusionIterations;

    float smokeKinematicViscosity;
    int smokeDiffusionIterations;

    float smokeDissipation;

    float windScale;
    float windAngle = 3.14f;

    // Time
    time_point<system_clock> start_time, last_time;

public:

    int init(Settings* settings);

    int changeSettings(Settings* settings, bool shouldRegenFields);

    void update(GLuint& densityData, GLuint& temperatureData, ivec3& size);

private:

    void initData(Settings* settings);

    void clearData();

    void getData(GLuint& densityData, GLuint& temperatureData, ivec3& size);

    // Performs one fire.simulation step for velocity
    void velocityStep(float dt);

    void updateAndApplyWind(float scale, float dt);

    void temperatureStep( float dt);

    void smokeDensityStep(float dt);
};

#endif //DATX02_20_21_SIMULATOR_H
