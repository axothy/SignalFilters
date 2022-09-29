#include "diagnostic_filter.h"

DiagnosticFilter::DiagnosticFilter()
{

}

void Filter::getSignal(QString filename) {
    Signal signal;

    QFile outs(filename);
    if (!outs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&outs);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.startsWith("#"))
            continue;
        QStringList values = line.split(" ", QString::SkipEmptyParts);
        (signal.time).append(values[0].toDouble());
        (signal.state).append(values[1].toDouble());
    }
    outs.close();


    appendSignal(signal);
}

inline void Filter::appendSignal(Signal &signal) {
    listOfSignals.append(signal);
}


double DiagnosticFilter::mean(const QVector<double> &vector) {
    double vectorSum = 0;

    foreach(double el, vector) {
        vectorSum += el;
    }

    return vectorSum/(vector.size());
}

double DiagnosticFilter::stDev(const QVector<double> &vector, const QVector<double> &linearizedVector) {
    double sum = 0;

    for (int i = 0; i < vector.size()-1; ++i) {
        sum += ((vector.at(i)-linearizedVector.at(i))*(vector.at(i)-linearizedVector.at(i)));
    }

    return qSqrt(1.0/(vector.size()-2) * sum);
}


QVector<double> DiagnosticFilter::linearization(const Signal &signal) {
    QVector<double> linearizedState;

    double b0 = mean(signal.state);

    double sum1 = 0, sum2 = 0, sum3 = 0;
    for (int i = 0; i < (signal.time).size(); ++i)
    {
        sum1 += (signal.time.at(i) - mean(signal.time));
        sum2 += (signal.state.at(i) - b0);
        sum3 += (signal.time.at(i) - mean(signal.time))*(signal.time.at(i) - mean(signal.time));
    }

    double b1 = (sum1 * sum2) / sum3;

    for (int i = 0; i < (signal.state).size(); i++)
    {
        linearizedState.append(b1 * (signal.time).at(i) + b0); //linear approximation
    }

    return linearizedState;
}

inline FailureFlag DiagnosticFilter::checkBorder(double value, double borderUp, double borderDown) const {
    return ((value > borderUp) || (value < borderDown)) ? FAILURE : NON_FAILURE;
}

Signal DiagnosticFilter::checkSignal(Signal &signal, QVector<double> &linearizedState, double sigma, double coeff) {
    QVector<double> corridoreUp, corridoreDown;
    corridoreUp.append(linearizedState.at(0) + coeff * sigma);
    corridoreDown.append(linearizedState.at(0) - coeff*sigma);

    for (int i = 1; i < linearizedState.size(); i++) {

        corridoreUp.append(linearizedState.at(i) + coeff * sigma);
        corridoreDown.append(linearizedState.at(i) - coeff * sigma);

        if (checkBorder((signal.state).at(i), corridoreUp.at(i), corridoreDown.at(i)) == FAILURE) {
            (signal.state).replace(i, (signal.state).at(i-1)); //Замена сбоя
        }
    }

    return signal;
}

void DiagnosticFilter::filterSignal(Signal &signal, int signalNumber) {
    int N = 2; //Число окон

    Signal window;
    QVector<double> linearizedState;

    int begin = 0;
    int end = (signal.state).size()/N;

    while (end <= (signal.state).size()) {

        /* Заполняем окно: */
        for (int i = begin; i < end; ++i) {
            (window.state).append((signal.state).at(i));
            (window.time).append((signal.time).at(i));
        }


        linearizedState = linearization(window);

        double sigma = stDev(window.state, linearizedState);

        /* Проверка на то, что значения удовлетворяют границам коридора (критерий 3-сигма) */
        window = checkSignal(window, linearizedState, sigma);

        int j = 1;
        for (int i = begin + 1; i < end - 1; ++i) {
            if ((signal.state).at(i) != (window.state).at(j)) {
                (signal.state).replace(i, (window.state).at(j));
            }
            j++;
        }

       // begin = end;
       //end += (signal.state).size()/N;
        begin++;
        end++;

        window.clear(); linearizedState.clear();
    }


    save(signal.state, "/home/user/QtTestProject/test/datetimeaxis/signal_values.txt", signalNumber);
}

void Filter::save(QVector<double> &filteredState, QString filename, int signalNumber) {
    QFile outs(filename);

    if (!outs.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream outF(&outs);
    outF.setRealNumberPrecision(6);

    outF << "#" << "\n";
    outF << "#                             DATA" << "\n";
    outF << "#                                   "      << "\n";
    outF << "#" << "\n";
    outF << "#" << "\n";
    outF << "# TIME      VALUE    FILTERED " << "\n";
    outF << "#-------------------------------" << "\n";


    for (int i = 0; i < (listOfSignals.at(signalNumber)).size(); ++i) {
        outF << ((listOfSignals.at(signalNumber)).time).at(i) << "       "
             << ((listOfSignals.at(signalNumber)).state).at(i) << "       "
             << filteredState.at(i) << "\n";
    }

    outs.close();
}


void Filter::filterAll() {
    Signal signal;
    int signalNumber = 0;
    foreach (signal, listOfSignals) {
        filterSignal(signal, signalNumber);
        signalNumber++;
    }
}
