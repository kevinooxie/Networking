#include "NetworkChat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	NetworkChat w;
	w.show();
	return a.exec();
}
