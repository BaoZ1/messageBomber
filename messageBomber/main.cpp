#include "messageBomber.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	messageBomber w;

	return a.exec();
}