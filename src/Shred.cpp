//
// Created by Yikai Li on 5/8/23.
//

#include "Shred.h"
#include "utils.h"
#include "Chuck.h"

Shred::Shred(Generator<ChuckDur> *generator, Chuck *VM) {
    this->generator = generator;
    if (next(VM)) {
        VM->shreds.push_back(this);
    }
}

Shred::~Shred() {
    delete generator;
}

bool Shred::next(Chuck *VM) {
    VM->currentShred = this;
    try {
        ChuckDur dur = (*generator)();
        if (dur.samples < 0) {
            return false;
        } else {
            samplesLeft = (unsigned int) dur.samples;
            return true;
        }
    } catch (...) {
        return false;
    }
}

void Shred::update(unsigned int samples, Chuck *VM) {
    samplesLeft -= samples;
    if (samplesLeft == 0) {
        if (!next(VM)) {
            remove(VM);
        }
    }
}

void Shred::remove(Chuck *VM) {
    for (auto shred: sporks) {
        shred->remove(VM);
    }
    sporks.clear();
    for (auto ugen: ugens) {
        ugen->remove();
    }
    ugens.clear();
    VM->shreds.erase(std::find(VM->shreds.begin(), VM->shreds.end(), this));
}








