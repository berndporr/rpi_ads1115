#ifndef WINDOW_H
#define WINDOW_H

#include "ads1115rpi.h"

#include <qwt/qwt_thermo.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#include <QBoxLayout>
#include <QPushButton>

// class definition 'Window'
class Window : public QWidget
{
	// must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
	Q_OBJECT

private:

	class ADS1115 : public ADS1115rpi {
	public:
		ADS1115(Window* w) : window(w) {}
		virtual void hasSample(float v) {
			window->addSample(v);
		}
	private:
		Window* window;
	};

	
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

// internal variables for the window class
private:
	static constexpr int plotDataSize = 100;
	static constexpr double gain = 7.5;

	QPushButton  *button;
	QwtThermo    *thermo;
	QwtPlot      *plot;
	QwtPlotCurve *curve;

	// layout elements from Qt itself http://qt-project.org/doc/qt-4.8/classes.html
	QVBoxLayout  *vLayout;  // vertical layout
	QHBoxLayout  *hLayout;  // horizontal layout

	// data arrays for the plot
	double xData[plotDataSize];
	double yData[plotDataSize];

	void reset();

	void addSample(float v);

	void timerEvent( QTimerEvent * );

	ADS1115* ads1115;
};

#endif // WINDOW_H
