#include "window.h"

#include <cmath>  // for sine stuff

Window::Window()
{
    plot = new QCustomPlot();
    plot->xAxis->setRange(0, nRealtimePoints);
	plot->yAxis->setRange(0, 1);
   	plot->xAxis->setLabel("samples");
	plot->yAxis->setLabel("Volt");
    plot->addGraph();
	plot->graph()->setName(QString("ADC"));
	animdata.reset(new QCPDataContainer<QCPGraphData>);
	for(int i=0;i<nRealtimePoints;i++) {
		QCPGraphData data(i,0);
		animdata->add(data);
	}
	plot->graph()->setData(animdata);
	QPen graphPen;
	graphPen.setColor(QColor(0,0,128));
	graphPen.setWidthF(2);
	plot->graph()->setPen(graphPen);
	plot->replot();

    button = new QPushButton("Reset");
    connect(button,&QPushButton::clicked,this,&Window::reset);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    adcLabel = new QLabel();
    adcLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // set up the layout - button below the adcLabel
    vLayout = new QVBoxLayout();
    vLayout->addWidget(adcLabel);
    vLayout->addWidget(button);

    // plot to the left of button and thermometer
    hLayout = new QHBoxLayout();
    hLayout->addLayout(vLayout);
    hLayout->addWidget(plot);

    setLayout(hLayout);
}


void Window::startDAQ() {
    ads1115.registerCallback([&](float v){hasADS1115Sample(v);});
    ADS1115settings s;
    s.samplingRate = ADS1115settings::FS64HZ;
    ads1115.start(s);
    // call the timerEvent function every 40 ms to refresh the screen
    startTimer(40);
}


Window::~Window() {
    ads1115.stop();
}



void Window::reset() {
	for (auto i = animdata->begin(); i != (animdata->end()); i++) {
		i->value = 0;
	}
}


void Window::hasADS1115Sample( float v ) {
    // shift the values
	for (auto i = animdata->end(); i != (animdata->begin()); --i) {
		i->value = (i-1)->value;
	}
	// add a new datapoint at the start
	animdata->begin()->value = v;
    char tmp[256];
    sprintf(tmp,"%0.2f V",v);
    adcLabel->setText(tmp);
}


void Window::timerEvent( QTimerEvent * )
{
    plot->replot();
    update();
}
