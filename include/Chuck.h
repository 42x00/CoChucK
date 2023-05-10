//
// Created by Yikai Li on 5/8/23.
//

#ifndef COCHUCK_CHUCK_H
#define COCHUCK_CHUCK_H


#include <vector>
#include "utils.h"
#include "Shred.h"
#include "UGen.h"


class Chuck {
public:
    Chuck(unsigned int sampleRate = 44100, unsigned int bufferSize = 256,
          unsigned int inChannels = 1, unsigned int outChannels = 2);

    void addShred(std::string code);

    void forward(double *inData, double *outData, unsigned int length);

    void compute(unsigned int samples);

    unsigned int sampleRate;
    unsigned int bufferSize;
    unsigned int inChannels;
    unsigned int outChannels;
    std::vector<Shred *> shreds;
    Shred *globalShred;
    Shred *currentShred;
    TSQueue<Generator<ChuckDur> *> queue;
    ADC *adc;
    DAC *dac;
    Blackhole *blackhole;
    ChuckTime *now;
    ChuckDur *samp;
    ChuckDur *ms;
    ChuckDur *second;
    ChuckDur *minute;
    ChuckDur *hour;
    ChuckDur *day;
    ChuckDur *week;
};


#endif //COCHUCK_CHUCK_H
