//
// Created by Yikai Li on 5/8/23.
//

#include <fstream>
#include "UGen.h"
#include "Chuck.h"

UGen::UGen(Chuck *VM) {
    VM->currentShred->ugens.push_back(this);
    gain = 1.0;
    last = 0.0;
    op = 0;
    sampleRate = VM->sampleRate;
    bufferSize = VM->bufferSize;
    inBuffer = new double[bufferSize];
    buffer = new double[bufferSize];
    computed = false;
}

UGen::~UGen() {
    delete[] inBuffer;
    delete[] buffer;
}

UGen &UGen::operator>>(UGen &ugen) {
    ugen.inUGens.push_back(this);
    outUGens.push_back(&ugen);
    return ugen;
}

UGen &UGen::operator<<(UGen &ugen) {
    ugen.inUGens.erase(std::find(ugen.inUGens.begin(), ugen.inUGens.end(), this));
    outUGens.erase(std::find(outUGens.begin(), outUGens.end(), &ugen));
    return ugen;
}

void UGen::remove() {
    for (auto ugen: inUGens) {
        ugen->outUGens.erase(std::find(ugen->outUGens.begin(), ugen->outUGens.end(), this));
    }
    inUGens.clear();
    for (auto ugen: outUGens) {
        ugen->inUGens.erase(std::find(ugen->inUGens.begin(), ugen->inUGens.end(), this));
    }
    outUGens.clear();
}

void UGen::aggregate_inputs(unsigned int samples) {
    memset(inBuffer, 0, bufferSize * sizeof(double));
    for (auto ugen: inUGens) {
        double *inData = ugen->compute_samples(samples);
        for (unsigned int i = 0; i < samples; i++) {
            inBuffer[i] += inData[i];
        }
    }
}

double *UGen::compute_samples(unsigned int samples) {
    if (!computed) {
        computed = true;
        aggregate_inputs(samples);
        compute(samples);
        for (unsigned int i = 0; i < samples; i++) {
            buffer[i] *= gain;
        }
        last = buffer[samples - 1];
    }
    return buffer;
}

void UGen::compute(unsigned int samples) {
    for (unsigned int i = 0; i < samples; i++) {
        buffer[i] = tick(inBuffer[i]);
    }
}

double UGen::tick(double input) {
    return 0.0;
}


void ADC::set(double *inData, unsigned int length) {
    memcpy(buffer, inData, length * sizeof(double));
    index = 0;
}

double *ADC::compute_samples(unsigned int samples) {
    if (!computed) {
        computed = true;
        for (unsigned int i = 0; i < samples; i++) {
            buffer[index + i] *= gain;
        }
        index += samples;
        last = buffer[index - 1];
    }
    return &buffer[index - samples];
}

void DAC::get(double *outData, unsigned int length) {
    memcpy(outData, buffer, length * sizeof(double));
    index = 0;
}

double *DAC::compute_samples(unsigned int samples) {
    if (!computed) {
        computed = true;
        aggregate_inputs(samples);
        for (unsigned int i = 0; i < samples; i++) {
            buffer[index + i] = inBuffer[i] * gain;
        }
        index += samples;
        last = buffer[index - 1];
    }
    return &buffer[index - samples];
}

double *Blackhole::compute_samples(unsigned int samples) {
    if (!computed) {
        computed = true;
        aggregate_inputs(samples);
    }
    return inBuffer;
}
