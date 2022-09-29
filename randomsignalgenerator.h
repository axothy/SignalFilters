#ifndef RANDOMSIGNALGENERATOR_H
#define RANDOMSIGNALGENERATOR_H
#include "diagnostic_filter.h"

enum SignalType {
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH,
    UNPREDICTABLE
};

typedef struct SignalParameters {

    double length;
    double phase;
    double frequency;
    double amplitude;
    double offset;
    SignalType type;

};

class RandomSignalGenerator
{

public:
    RandomSignalGenerator(SignalParameters params);

    inline void setParameters(SignalParameters params);
    void createNewSignal();
    Signal getNewSignal();
    void saveNewSignal(QString filename);

private:
    inline double sign(double x);
    void makeFailureInSignal();
    void createSineSignal();
    void createSquareSignal();
    void createTriangleSignal();
    void createSawToothSignal();

    Signal signal;
    SignalParameters _params;
    int numberOfFailures = 50;

};

#endif // RANDOMSIGNALGENERATOR_H
