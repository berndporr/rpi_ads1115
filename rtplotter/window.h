#ifndef WINDOW_H
#define WINDOW_H

#include "ads1115rpi.h"

#include <qcustomplot.h>

#include <QBoxLayout>
#include <QPushButton>

// class definition 'Window'
class Window : public QWidget
{
    // must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
    Q_OBJECT

public:
    /**
     * Initialises the GUI and sets up the ADC
     **/
    Window();

    /**
     * Starts data acquisition and screen refreshing
     **/
    void startDAQ();

    /**
     * Shuts down data acquisition
     **/
    ~Window();

    /**
     * Adds a new sample via callback
     **/
    void hasADS1115Sample(float v);

private:
    static constexpr int nRealtimePoints = 500;

    QLabel *adcLabel;
    QPushButton *button;
    QCustomPlot *plot;
    QSharedPointer<QCPDataContainer<QCPGraphData>> animdata;

    QVBoxLayout *vLayout; // vertical layout
    QHBoxLayout *hLayout; // horizontal layout

    void reset();

    void timerEvent(QTimerEvent *);

    // set up the low level driver
    ADS1115rpi ads1115;
};

#endif // WINDOW_H
