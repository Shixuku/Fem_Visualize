#include "Fem_Visualize.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Fem_Visualize w;
	w.show();
	return a.exec();
}
