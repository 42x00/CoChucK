//
// Created by Yikai Li on 5/8/23.
//

#ifndef COCHUCK_UGEN_H
#define COCHUCK_UGEN_H

#include <vector>
#include <SineWave.h>

class Chuck;

class UGen {
public:
    UGen(Chuck *VM);

    ~UGen();

    UGen &operator>>(UGen &ugen);

    UGen &operator<<(UGen &ugen);

    void remove();

    void aggregate_inputs(unsigned int samples);

    virtual double *compute_samples(unsigned int samples);

    virtual void compute(unsigned int samples);

    virtual double tick(double input);

    float gain;
    double last;
    unsigned int op;
    unsigned int sampleRate;
    unsigned int bufferSize;
    double *inBuffer;
    double *buffer;
    bool computed;
    std::vector<UGen *> inUGens;
    std::vector<UGen *> outUGens;
};

class ADC : public UGen {
public:
    ADC(Chuck *VM) : UGen(VM) {};

    void set(double *inData, unsigned int length);;

    double *compute_samples(unsigned int samples) override;

    unsigned int index;
};

class DAC : public UGen {
public:
    DAC(Chuck *VM) : UGen(VM) {};

    void get(double *outData, unsigned int length);;

    double *compute_samples(unsigned int samples) override;

    unsigned int index;
};

class Blackhole : public UGen {
public:
    Blackhole(Chuck *VM) : UGen(VM) {};

    double *compute_samples(unsigned int samples) override;
};

class SinOsc : public UGen {
public:
    SinOsc(Chuck *VM) : UGen(VM) {
        obj = new stk::SineWave();
        obj->setRate(sampleRate);
        obj->setFrequency(440);
    };

    double tick(double input) override {
        return obj->tick();
    };

    void setFrequency(double frequency) {
        obj->setFrequency(frequency);
    }

    stk::SineWave *obj;
};

#endif //COCHUCK_UGEN_H
