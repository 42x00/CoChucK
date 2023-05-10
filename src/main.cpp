#include <iostream>
#include <fstream>
#include <cstdlib>
#include <boost/program_options.hpp>
#include "RtAudio.h"
#include "Chuck.h"

namespace po = boost::program_options;

int process(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData) {
    auto *chuck = (Chuck *) userData;
    auto *inData = (double *) inputBuffer;
    auto *outData = (double *) outputBuffer;
    chuck->forward(inData, outData, nBufferFrames);
    return 0;
}

int main(int ac, char *av[]) {
    // default parameters
    unsigned int sampleRate = 44100;
    unsigned int bufferSize = 256;
    unsigned int inChannels = 1;
    unsigned int outChannels = 1;
    std::vector<std::string> files;

    // options
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "produce help message")
                ("srate", po::value<unsigned int>(), "set sample rate")
                ("bufsize", po::value<unsigned int>(), "set buffer size")
                ("in", po::value<unsigned int>(), "set in channels")
                ("out", po::value<unsigned int>(), "set out channels")
                ("file", po::value<std::vector<std::string> >(), "input files");

        po::positional_options_description p;
        p.add("file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("srate")) {
            sampleRate = vm["srate"].as<unsigned int>();
        }

        if (vm.count("bufsize")) {
            bufferSize = vm["bufsize"].as<unsigned int>();
        }

        if (vm.count("in")) {
            inChannels = vm["in"].as<unsigned int>();
        }

        if (vm.count("out")) {
            outChannels = vm["out"].as<unsigned int>();
        }

        if (vm.count("file")) {
            files = vm["file"].as<std::vector<std::string> >();
        }
    }
    catch (std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    // Chuck
    Chuck chuck(sampleRate, bufferSize, inChannels, outChannels);

    // RtAudio
    RtAudio audio;
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = inChannels;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = outChannels;
    try {
        audio.openStream(&oParams, &iParams, RTAUDIO_FLOAT64,
                         sampleRate, &bufferSize, &process, &chuck);
        audio.startStream();
    }
    catch (RtAudioError &e) {
        e.printMessage();
        exit(0);
    }

    char input;
    std::cout << "\nPlaying ... press <enter> to quit.\n";
    std::cin.get(input);
    try {
        // Stop the stream
        audio.stopStream();
    }
    catch (RtAudioError &e) {
        e.printMessage();
    }
    if (audio.isStreamOpen()) audio.closeStream();
    return 0;
}