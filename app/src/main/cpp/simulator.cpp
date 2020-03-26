//
// Created by Kalle on 2020-03-04.
//
#include "simulator.h"

#include <jni.h>
#include <GLES3/gl32.h>

void Simulator::init(){
    slab.init();
}

void Simulator::resize(int width, int height, int depth) {
    slab.resize(width, height, depth);
}

void Simulator::update(){
    slab.update();
}

void Simulator::getData(GLuint& data, int& width, int& height, int& depth){
    slab.getData(data, width, height, depth);
}