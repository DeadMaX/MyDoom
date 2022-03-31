// WebAssembloom.cpp : définit le point d'entrée de l'application.
//

#include "WebAssembloom.h"
#include <QApplication>
#include <QSurfaceFormat>

#include "mainwindow.h"

using namespace std;

int main(int argc, char* argv[])
{
	QApplication app{argc, argv};

	QCoreApplication::setApplicationName("Qt QOpenGLWidget Example");
	QCoreApplication::setOrganizationName("QtProject");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
//	if (parser.isSet(srgbOption))
//		format.setColorSpace(QSurfaceFormat::sRGBColorSpace);
//	if (parser.isSet(multipleSampleOption))
//		format.setSamples(4);
	QSurfaceFormat::setDefaultFormat(format);

	MainWindow mw;
	mw.show();

	return app.exec();
}
