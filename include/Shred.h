//
// Created by Yikai Li on 5/8/23.
//

#ifndef COCHUCK_SHRED_H
#define COCHUCK_SHRED_H

#include "UGen.h"
#include "utils.h"

class Chuck;

class Shred {
public:
    Shred() {};

    Shred(Generator<ChuckDur> *generator, Chuck *VM);

    ~Shred();

    bool next(Chuck *VM);

    void update(unsigned int samples, Chuck *VM);

    void remove(Chuck *VM);

    Generator<ChuckDur> *generator;
    unsigned int samplesLeft;
    std::vector<UGen *> ugens;
    std::vector<Shred *> sporks;
};


#endif //COCHUCK_SHRED_H
