#include "randomsignalgenerator.h"


RandomSignalGenerator::RandomSignalGenerator(SignalParameters params)
{
    _params = params;
}

inline void RandomSignalGenerator::setParameters(SignalParameters params)
{
    _params = params;
}

inline double RandomSignalGenerator::sign(double x) {
    return x < 0 ? -1 : ((x > 0) ? 1 : 0);
}

void RandomSignalGenerator::createSineSignal() {
    foreach(double t, signal.time) {
         (signal.state).append(_params.amplitude * qSin(2*M_PI*t) + _params.offset);
    }
}

void RandomSignalGenerator::createSquareSignal() {
    foreach(double t, signal.time) {
         (signal.state).append(_params.amplitude * sign(qSin(2*M_PI*t)) + _params.offset);
    }
}

void RandomSignalGenerator::createTriangleSignal() {
    foreach(double t, signal.time) {
         (signal.state).append(_params.amplitude * (1 - 4 * qAbs(round(t - 0.25) - (t - 0.25))) + _params.offset);
    }
}

void RandomSignalGenerator::createSawToothSignal() {
    foreach(double t, signal.time) {
         (signal.state).append(_params.amplitude * (2 * (t - floor(t + 0.5))) + _params.offset);
    }
}

void RandomSignalGenerator::createNewSignal()
{
    QVector<double> t;
    double value = 0;
    for (int i = 0; i < _params.length; i++) {
        t.append(value);
        value++;
    }

    for (int i = 0; i < _params.length; i++) {
        (signal.time).append(_params.frequency * t.at(i) + _params.phase);
    }

    if (_params.type == SINE) createSineSignal();
    if (_params.type == SQUARE) createSquareSignal();
    if (_params.type == TRIANGLE) createTriangleSignal();
    if (_params.type == SAWTOOTH) createSawToothSignal();

    makeFailureInSignal();
}

void RandomSignalGenerator::makeFailureInSignal() {

    for (int i = 0; i < numberOfFailures; i++) {
        int numberOfErrorValue = rand() % signal.size() + 1;
        signal.state.replace(numberOfErrorValue, (rand() % 4 + 3)  * signal.state.at(numberOfErrorValue));
    }
}

void RandomSignalGenerator::saveNewSignal(QString filename) {
    QFile outs(filename);

    if (!outs.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream outF(&outs);
    outF.setRealNumberPrecision(6);

    outF << "#" << "\n";
    outF << "#                         GENERATED DATA" << "\n";
    outF << "#                                   "      << "\n";
    outF << "#" << "\n";
    outF << "#" << "\n";
    outF << "# TIME      VALUE    FILTERED " << "\n";
    outF << "#-------------------------------" << "\n";


    for (int i = 0; i < signal.size(); ++i) {
        outF << signal.time.at(i) << "       "
             << signal.state.at(i) << "\n";
    }

    outs.close();
}

Signal RandomSignalGenerator::getNewSignal()
{
    return signal;
}
