#include "window.h"

#include <cmath>  // for sine stuff


Window::Window()
{
	ads1115 = new ADS1115(this);
	
	// set up the thermometer
	thermo = new QwtThermo; 
	thermo->setFillBrush( QBrush(Qt::red) );
	thermo->setScale(0, 10);
	thermo->show();


	// set up the initial plot data
	for( int index=0; index<plotDataSize; ++index )
	{
		xData[index] = index;
		yData[index] = 0;
	}

	curve = new QwtPlotCurve;
	plot = new QwtPlot;
	// make a plot curve from the data and attach it to the plot
	curve->setSamples(xData, yData, plotDataSize);
	curve->attach(plot);

	plot->replot();
	plot->show();

	button = new QPushButton("Reset");
	// see https://doc.qt.io/qt-5/signalsandslots-syntaxes.html
	connect(button,&QPushButton::clicked,this,&Window::reset);

	// set up the layout - button above thermometer
	vLayout = new QVBoxLayout();
	vLayout->addWidget(button);
	vLayout->addWidget(thermo);

	// plot to the left of button and thermometer
	hLayout = new QHBoxLayout();
	hLayout->addLayout(vLayout);
	hLayout->addWidget(plot);

	setLayout(hLayout);
}


void Window::startDAQ() {
	ADS1115settings s;
	s.samplingRate = ADS1115settings::FS64HZ;
	ads1115->start(s);
	// call the timerEvent function every 40 ms to refresh the screen
	startTimer(40);
	}

Window::~Window() {
	ads1115->stop();
	delete ads1115;
}



void Window::reset() {
	// set up the initial plot data
	for( int index=0; index<plotDataSize; ++index )
	{
		xData[index] = index;
		yData[index] = 0;
	}
}


void Window::addSample( float v ) {
        std::move( yData, yData + plotDataSize - 1, yData+1 );
	yData[0] = v;
}


void Window::timerEvent( QTimerEvent * )
{
	curve->setSamples(xData, yData, plotDataSize);
	plot->replot();
	thermo->setValue( yData[0] );
	update();
}
