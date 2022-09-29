#ifndef FILTER_H
#define FILTER_H

#include <QFile>
#include <QDebug>
#include <QVector>
#include <QtCore>

typedef struct Signal {
    QVector<double> state; /*Переменная состояния МПО*/
    QVector<double> time;

    void clear() {
        state.clear();
        time.clear();
    }

    inline int size() const {
        if (state.size() == time.size()) return state.size();
    }

};

/*Флаг ошибки датчика*/
enum FailureFlag {
    FAILURE,
    NON_FAILURE
};

class Filter {


public:
    void getSignal(QString filename);
    virtual void filterSignal(Signal &signal, int signalNumber) {}
    void filterAll();
    inline void appendSignal(Signal &signal);


protected:
    void save(QVector<double> &filteredState, QString filename, int signalNumber);

private:

    QVector<Signal> listOfSignals; /* Все возможные сигналы */

};


class DiagnosticFilter : public Filter
{

public:
    DiagnosticFilter();

    void filterSignal(Signal &signal, int signalNumber) override;

private:

    double mean(const QVector<double> &vector); /* Среднее арифм */
    double stDev(const QVector<double> &vector, const QVector<double> &linearizedVector); /* Сигма */
    QVector<double> linearization(const Signal &signal); /* Линейная аппроксимация сигнала */
    inline FailureFlag checkBorder(double value, double borderUp, double borderDown) const;
    Signal checkSignal(Signal &signal, QVector<double> &linearizedState, double sigma, double coeff = 3); /*Проверка на критерий 3-сигма по умолчанию) */


    QVector<Signal> listOfSignals; /* Все возможные сигналы */

};

#endif // FILTER_H
