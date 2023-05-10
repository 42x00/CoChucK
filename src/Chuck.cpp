//
// Created by Yikai Li on 5/8/23.
//

#include <cstring>
#include "Chuck.h"
#include "example.h"


Chuck::Chuck(unsigned int sampleRate, unsigned int bufferSize, unsigned int inChannels, unsigned int outChannels) {
    this->sampleRate = sampleRate;
    this->bufferSize = bufferSize;
    this->inChannels = inChannels;
    this->outChannels = outChannels;
    globalShred = new Shred;
    currentShred = globalShred;
    adc = new ADC(this);
    dac = new DAC(this);
    blackhole = new Blackhole(this);
    now = new ChuckTime(0);
    samp = new ChuckDur(1);
    second = new ChuckDur(sampleRate);
    ms = new ChuckDur((double) sampleRate / 1000);
    minute = new ChuckDur(sampleRate * 60);
    hour = new ChuckDur(sampleRate * 60 * 60);
    day = new ChuckDur(sampleRate * 60 * 60 * 24);
    week = new ChuckDur(sampleRate * 60 * 60 * 24 * 7);
    queue.push(new Generator<ChuckDur>(
            example(this, *adc, *dac, *blackhole, *now, *samp,
                    *second, *ms, *minute, *hour, *day, *week)));
}


void Chuck::forward(double *inData, double *outData, unsigned int length) {
    adc->set(inData, length);

    while (!queue.empty()) {
        new Shred(queue.pop(), this);
    }

    unsigned int samplesLeft = length, samples;
    while (samplesLeft > 0) {
        samples = samplesLeft;
        for (auto shred: shreds) {
            samples = std::min(samples, shred->samplesLeft);
        }

        compute(samples);

        samplesLeft -= samples;
        now->sample += samples;
        for (auto shred: shreds) {
            shred->update(samples, this);
        }
    }

    dac->get(outData, length);
}

void Chuck::compute(unsigned int samples) {
    for (auto shred: shreds) {
        for (auto ugen: shred->ugens) {
            ugen->computed = false;
        }
    }
    for (auto ugen: globalShred->ugens) {
        ugen->computed = false;
    }
    blackhole->compute_samples(samples);
    dac->compute_samples(samples);
}








