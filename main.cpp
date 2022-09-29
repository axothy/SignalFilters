#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCharts/QValueAxis>
#include "diagnostic_filter.h"
#include "randomsignalgenerator.h"


QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SignalParameters params;
    params.length = 1000;
    params.frequency = 0.01;
    params.amplitude = 10;
    params.phase = 0;
    params.offset = 1;
    params.type = SINE;

    RandomSignalGenerator generator(params);
    generator.createNewSignal();
    generator.saveNewSignal("/home/user/QtTestProject/test/datetimeaxis/generatedtestsignal.txt");

    DiagnosticFilter diagnosticFilter;
    diagnosticFilter.getSignal("/home/user/QtTestProject/test/datetimeaxis/generatedtestsignal.txt");
    diagnosticFilter.filterAll();


    QLineSeries *series1 = new QLineSeries();
    QLineSeries *series2 = new QLineSeries();

    QFile outs("/home/user/QtTestProject/test/datetimeaxis/signal_values.txt");
    if (!outs.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 1;
    }

    QTextStream stream(&outs);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.startsWith("#"))
            continue;
        QStringList values = line.split(" ", QString::SkipEmptyParts);
        series1->append(values[0].toDouble(), values[1].toDouble());
        series2->append(values[0].toDouble(), values[2].toDouble());
    }
    outs.close();

    QChart *chart = new QChart();
    chart->addSeries(series1);
    chart->addSeries(series2);
    chart->legend()->hide();
    chart->setTitle("Данные с датчика и их фильтрация диагностическим фильтром");

    QValueAxis *axisX = new QValueAxis;
    axisX->setLabelFormat("%.2lf");
    axisX->setTitleText("Время");
    chart->addAxis(axisX, Qt::AlignBottom);
    series1->attachAxis(axisX);
    series2->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%.2lf");
    axisY->setTitleText("Данные с гирокомпаса");
    chart->addAxis(axisY, Qt::AlignLeft);
    series1->attachAxis(axisY);
    series2->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(820, 600);
    window.show();




    return a.exec();
}
