#include "window.h"

#include <QApplication>

// Main program
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	// create the window
	Window window;
	window.show();

	window.startDAQ();

	// execute the application
	return app.exec();
}
