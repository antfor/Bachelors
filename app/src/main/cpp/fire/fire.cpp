//
// Created by Kalle on 2020-03-04.
//

#include "fire.h"
#include "util/file_loader.h"
#include "settings.h"
#include <android/asset_manager_jni.h>

#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>

#define LOG_TAG "FIRE"
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

Fire::Fire(JNIEnv* javaEnvironment, AAssetManager* assetManager, int width, int height)
    : javaEnvironment(javaEnvironment), assetManager(assetManager),
      screen_width(width), screen_height(height), shouldUpdateSettings(false), shouldRegenFields(false) {
    initFileLoader(assetManager);

    settings = new Settings();

    renderer = new Renderer();
    simulator = new Simulator();

}

int Fire::init() {

    settings->withSize(ivec3(1, 4, 1), 12, 60, 24.0f)->withDeltaTime(1 / 30.0f)
            ->withSourceMode(SourceMode::set)->withSourceType(SourceType::singleSphere)->withSourceTemperature(3500.0f)
            ->withSourceDensity(0.4f)->withSourceRadius(8.0f)->withVelDiffusion(0.0f, 0)->withVorticityScale(8.0f)->withProjectIterations(20)
            ->withBuoyancyScale(0.15f)->withSmokeDissipation(0.0f)->withSmokeDiffusion(0.0f, 0)->withWindStrength(0.0f)
            ->withTempDiffusion(0.0f, 0)->withBackgroundColor(vec3(0.0f, 0.0f, 0.0f))->withFilterColor(vec3(1.0f, 1.0f, 1.0f))
            ->withColorSpace(vec3(1.8f, 2.2f, 2.2f))->withName("Default")->withMinBand(2.0f)->withMaxBand(8.0f);

    settings->printInfo("FIRE");

    return renderer->init(settings) && simulator->init(settings);
}

void Fire::resize(int width, int height){
    renderer->resize(width, height);
}

void Fire::update(){
    GLuint density, temperature;
    ivec3 size;

    simulator->setRotation(-renderer->getRotation());

    if(shouldUpdateSettings){

        LOG_INFO("WOW!!!!!");

        renderer->changeSettings(settings);
        simulator->changeSettings(settings, shouldRegenFields);

        shouldUpdateSettings = false;
        shouldRegenFields = false;
    }

    simulator->update(density, temperature, size);
    renderer->update(density, temperature, size);
}

void Fire::touch(double x, double y, double dx, double dy){
    renderer->touch(dx, dy);

    if(!settings->getTouchMode()) {

        mat4 inverseMVP = renderer->getInverseMVP();

        vec3 normalPos = 2.0f * vec3(x/screen_width, 1.0f - y/screen_height, 0.5f) - vec3(1.0f);
        LOG_INFO("ROSITION: %f, %f, %f", normalPos.x, normalPos.y, normalPos.z);
        vec3 gridPos = vec3(inverseMVP * vec4(normalPos, 0.0f)) + vec3(0.5f) - renderer->getOffset();

        vec3 normalVec = vec3(dx, -dy, 0.0f);
        vec3 gridVec = inverseMVP * vec4(normalVec, 0.0f);

        simulator->addExternalForce(gridPos, gridVec, settings);
    }
}

void Fire::scale(float scaleFactor, double scaleX, double scaleY){
    renderer->scale(scaleFactor, scaleX, scaleY);
}

void Fire::onClick() {
    //Settings newSettings = nextSettings();
    //std::string name = newSettings.getName();
    //LOG_INFO("Changing settings to %s", name.data());
    //*settings = nextSettings();
    //shouldUpdateSettings = true;
    //simulator.changeSettings(newSettings);
}

void Fire::rotationSensor(float *rotationMatrix) {
    simulator->updateDeviceRotationMatrix(rotationMatrix);
}

void Fire::setTouchMode(bool touchMode){
    LOG_INFO("TouchModeSetting, %s", touchMode ? "true" : "false");
    settings->withTouchMode(touchMode);
    shouldUpdateSettings = true;
}

void Fire::setOrientation(bool orientationMode){
    LOG_INFO("OrientationSetting, %s", orientationMode ? "true" : "false");
    settings->withOrientationMode(orientationMode);
    shouldUpdateSettings = true;
}

void Fire::updateResolution(int lowerRes) {
    LOG_INFO("ResolutionUpdate, %d", lowerRes);
    settings->withSize(settings->getSizeRatio(), lowerRes, settings->getResScale()*lowerRes, settings->getSimulationScale());
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateResolutionScale(float scale) {
    LOG_INFO("ResolutionScaleUpdate, %f", scale);
    settings->withSize(settings->getSizeRatio(), settings->getVelocityScale(), settings->getVelocityScale()*scale, settings->getSimulationScale());
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateSimulationScale(float scale) {
    LOG_INFO("SimulationScaleUpdate, %f", scale);
    settings->withSize(settings->getSizeRatio(), settings->getVelocityScale(), settings->getSubstanceScale(), scale);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateTimeStep(float timestep) {
    LOG_INFO("TimeStepUpdate, %f", timestep);
    settings->withDeltaTime(1.0f/timestep);
    shouldUpdateSettings = true;
}

void Fire::updateBackgroundColor(float red, float green, float blue) {
    LOG_INFO("BackgroundColorUpdate, %f, %f, %f", red, green, blue);
    settings->withBackgroundColor(vec3(red, green, blue));
    shouldUpdateSettings = true;
}

void Fire::updateFilterColor(float red, float green, float blue) {
    LOG_INFO("BackgroundColorUpdate, %f, %f, %f", red, green, blue);
    settings->withFilterColor(vec3(red, green, blue));
    shouldUpdateSettings = true;
}

void Fire::updateColorSpace(float X, float Y, float Z) {
    LOG_INFO("ColorSpaceUpdate, %f, %f, %f", X, Y, Z);
    settings->withColorSpace(vec3(X, Y, Z));
    shouldUpdateSettings = true;
}

void Fire::updateObjectType(std::string type) {
    LOG_INFO("ObjectTypeUpdate, %s", type.c_str());
    if(type == "SPHERE")
        settings->withSourceType(SourceType::singleSphere);
    else if(type == "CUBE")
        settings->withSourceType(SourceType::cube);
    else if(type == "PYRAMID")
        settings->withSourceType(SourceType::pyramid);
    else if(type == "CYLINDER")
        settings->withSourceType(SourceType::cylinder);
    else if(type == "CONE")
        settings->withSourceType(SourceType::cone);
    else if(type == "FLOOR")
        settings->withSourceType(SourceType::floor);
    else if(type == "WALL")
        settings->withSourceType(SourceType::wall);
    else if(type == "DUALSPHERES")
        settings->withSourceType(SourceType::dualSpheres);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateObjectRadius(float radius) {
    LOG_INFO("ObjectRadiusUpdate, %f", radius);
    settings->withSourceRadius(radius);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateObjectTemperature(float temperature) {
    LOG_INFO("ObjectTemperatureUpdate, %f", temperature);
    settings->withSourceTemperature(temperature);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateObjectDensity(float density) {
    LOG_INFO("ObjectDensityUpdate, %f", density);
    settings->withSourceDensity(density);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateObjectVelocity(float velocity) {
    LOG_INFO("ObjectVelocityUpdate, %f", velocity);
    settings->withSourceVelocity(velocity);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateWindStrength(float strength) {
    LOG_INFO("WindStrengthUpdate, %f", strength);
    settings->withWindStrength(strength);
    shouldUpdateSettings = true;
}

void Fire::setWindAngle(bool custom) {
    LOG_INFO("WindAngleSetting, %s", custom ? "true" : "false");
    settings->withRotatingWindAngle(custom);
    shouldUpdateSettings = true;
}

void Fire::updateWindAngle(float angle) {
    LOG_INFO("WindAngleUpdate, %f", angle);
    settings->withWindAngle(angle);
    shouldUpdateSettings = true;
}

void Fire::updateVorticity(float vorticityScale) {
    LOG_INFO("VorticityUpdate, %f", vorticityScale);
    settings->withVorticityScale(vorticityScale);
    shouldUpdateSettings = true;
}

void Fire::updateBuoyancy(float buoyancyScale) {
    LOG_INFO("BuoyancyUpdate, %f", buoyancyScale);
    settings->withBuoyancyScale(buoyancyScale);
    shouldUpdateSettings = true;
}

void Fire::updateSmokeDissipation(float dissipation) {
    LOG_INFO("SmokeDissipationUpdate, %f", dissipation);
    settings->withSmokeDissipation(dissipation);
    shouldUpdateSettings = true;
}

void Fire::updateTemperatureViscosity(float viscosity) {
    LOG_INFO("TmeperatureViscosityUpdate, %f", viscosity);
    settings->withTempDiffusion(viscosity, settings->getTempDiffusionIterations());
    shouldUpdateSettings = true;
}

void Fire::updateSmokeViscosity(float viscosity) {
    LOG_INFO("SmokeViscosityUpdate, %f", viscosity);
    settings->withSmokeDiffusion(viscosity, settings->getSmokeDiffusionIterations());
    shouldUpdateSettings = true;
}

void Fire::updateVelocityViscosity(float viscosity) {
    LOG_INFO("VelocityViscosityUpdate, %f", viscosity);
    settings->withVelDiffusion(viscosity, settings->getVelDiffusionIterations());
    shouldUpdateSettings = true;
}

void Fire::setMinNoiseBand(bool custom) {
    LOG_INFO("MinNoiseBandSetting, %s", custom ? "true" : "false");
    settings->withCustomMinBand(custom);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateMinNoiseBand(float band) {
    LOG_INFO("MinNoiseUpdate, %f", band);
    settings->withMinBand(band);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::setMaxNoiseBand(bool custom) {
    LOG_INFO("MaxNoiseBandSetting, %s", custom ? "true" : "false");
    settings->withCustomMaxBand(custom);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateMaxNoiseBand(float band) {
    LOG_INFO("MaxNoiseUpdate, %f", band);
    settings->withMaxBand(band);
    shouldUpdateSettings = true;
    shouldRegenFields = true;
}

void Fire::updateDensityDiffusionIterations(int iterations) {
    LOG_INFO("DensityDiffusionIterationsUpdate, %d", iterations);
    settings->withSmokeDiffusion(settings->getSmokeKinematicViscosity(), iterations);
    shouldUpdateSettings = true;
}

void Fire::updateVelocityDiffusionIterations(int iterations) {
    LOG_INFO("VelocityDiffusionIterationsUpdate, %d", iterations);
    settings->withVelDiffusion(settings->getVelKinematicViscosity(), iterations);
    shouldUpdateSettings = true;
}

void Fire::updateProjectionIterations(int iterations) {
    LOG_INFO("ProjectionIterationsUpdate, %d", iterations);
    settings->withProjectIterations(iterations);
    shouldUpdateSettings = true;
}

void Fire::updateBoundaries(std::string mode) {
    LOG_INFO("BoundariesUpdate, %s", mode.c_str());
    if (mode == "NONE")
        settings->withBoundaryType(BoundaryType::none);
    else if (mode == "SOME")
        settings->withBoundaryType(BoundaryType::some);
    shouldUpdateSettings = true;
}

bool Fire::changedSettings() {
    return shouldUpdateSettings;
}

AAssetManager* loadAssetManager(JNIEnv *env, jobject assetManager) {
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        LOG_ERROR("error loading asset manger");
    } else {
        LOG_INFO("loaded asset manager");
    }
    return mgr;
}


// FireActivity
JC(void) Java_com_pbf_FireActivity_init(JNIEnv* env, jobject , jobject mgr, jint width, jint height){
    fire = new Fire(env, loadAssetManager(env, mgr), width, height);
}
// FireRenderer
JC(jint) Java_com_pbf_FireRenderer_init(JCT){
    return fire->init();
}
JC(void) Java_com_pbf_FireRenderer_resize(JCT, jint width, jint height){
    fire->resize(width, height);
}
JC(void) Java_com_pbf_FireRenderer_update(JCT){
    fire->update();
}
// FireListener
JC(void) Java_com_pbf_FireListener_touch(JCT, jdouble x, jdouble y, jdouble dx, jdouble dy){
    fire->touch(x, y, dx, dy);
}
JC(void) Java_com_pbf_FireListener_scale(JCT, jfloat scaleFactor, jdouble scaleX, jdouble scaleY){
    fire->scale(scaleFactor, scaleX, scaleY);
}
JC(void) Java_com_pbf_FireListener_onClick(JCT){
    fire->onClick();
}

JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateResolutionScale(JCT, jfloat scale){
    fire->updateResolutionScale(scale);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateSimulationScale(JCT, jfloat scale){
    fire->updateSimulationScale(scale);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateTimeStep(JCT, jfloat timeStep){
    fire->updateTimeStep(timeStep);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectRadius(JCT, jfloat radius){
    fire->updateObjectRadius(radius);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectTemperature(JCT, jfloat temperature){
    fire->updateObjectTemperature(temperature);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectDensity(JCT, jfloat density){
    fire->updateObjectDensity(density);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateObjectVelocity(JCT, jfloat velocity){
    fire->updateObjectVelocity(velocity);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateWindStrength(JCT, jfloat strength){
    fire->updateWindStrength(strength);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateWindAngle(JCT, jfloat angle){
    fire->updateWindAngle(angle);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateVorticity(JCT, jfloat vorticityScale){
    fire->updateVorticity(vorticityScale);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateBuoyancy(JCT, jfloat buoyancyScale){
    fire->updateBuoyancy(buoyancyScale);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateSmokeDissipation(JCT, jfloat dissipation){
    fire->updateSmokeDissipation(dissipation);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateTemperatureViscosity(JCT, jfloat viscosity){
    fire->updateTemperatureViscosity(viscosity);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateSmokeViscosity(JCT, jfloat viscosity){
    fire->updateSmokeViscosity(viscosity);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateVelocityViscosity(JCT, jfloat viscosity){
    fire->updateVelocityViscosity(viscosity);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateMinNoiseBand(JCT, jfloat band){
    fire->updateMinNoiseBand(band);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateMaxNoiseBand(JCT, jfloat band){
    fire->updateMaxNoiseBand(band);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateDensityDiffusionIterations(JCT, jint iterations){
    fire->updateDensityDiffusionIterations(iterations);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateVelocityDiffusionIterations(JCT, jint iterations){
    fire->updateVelocityDiffusionIterations(iterations);
}
JC(void) Java_com_pbf_SettingsFragment_00024SliderBarListener_updateProjectionIterations(JCT, jint iterations){
    fire->updateProjectionIterations(iterations);
}

JC(void) Java_com_pbf_FireListener_rotationSensor(JCT, jfloatArray rotationMatrix) {
    float *data = env->GetFloatArrayElements(rotationMatrix, NULL);
    fire->rotationSensor(data);
    env->ReleaseFloatArrayElements(rotationMatrix, data, 0);
}

JC(void) Java_com_pbf_SettingsFragment_setTouchMode(JCT, jboolean touchMode){
    fire->setTouchMode(touchMode);
}
JC(void) Java_com_pbf_SettingsFragment_setOrientation(JCT, jboolean orientationMode){
    fire->setOrientation(orientationMode);
}
JC(void) Java_com_pbf_SettingsFragment_updateResolution(JCT, jint lowerRes){
    fire->updateResolution(lowerRes);
}
JC(void) Java_com_pbf_SettingsFragment_updateBackgroundColor(JCT, jfloat red, jfloat green, jfloat blue){
    fire->updateBackgroundColor(red, green, blue);
}
JC(void) Java_com_pbf_SettingsFragment_updateFilterColor(JCT, jfloat red, jfloat green, jfloat blue){
    fire->updateFilterColor(red, green, blue);
}
JC(void) Java_com_pbf_SettingsFragment_updateColorSpace(JCT, jfloat X, jfloat Y, jfloat Z){
    fire->updateColorSpace(X, Y, Z);
}
JC(void) Java_com_pbf_SettingsFragment_updateObjectType(JNIEnv* env, jobject, jstring type){
    jboolean isCopy;
    fire->updateObjectType(env->GetStringUTFChars(type, &isCopy));
}
JC(void) Java_com_pbf_SettingsFragment_setWindAngle(JCT, jboolean custom){
    fire->setWindAngle(custom);
}
JC(void) Java_com_pbf_SettingsFragment_setMinNoiseBand(JCT, jboolean custom){
    fire->setMinNoiseBand(custom);
}
JC(void) Java_com_pbf_SettingsFragment_setMaxNoiseBand(JCT, jboolean custom){
    fire->setMaxNoiseBand(custom);
}
JC(void) Java_com_pbf_SettingsFragment_updateBoundaries(JNIEnv* env, jobject, jstring mode){
    jboolean isCopy;
    fire->updateBoundaries(env->GetStringUTFChars(mode, &isCopy));
}

JC(jboolean) Java_com_pbf_FireRenderer_changedSettings(JCT){
    return fire->changedSettings();
}
