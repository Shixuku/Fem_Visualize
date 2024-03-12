#include "Fem_Visualize.h"
#include "TowerTest.h"
#include "H_Tower.h"
#include "A_Tower.h"
#include "HUA_Tower.h"
#include "Y_Tower.h"
#include "I_Tower.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Fem_Visualize w;
	//TowerTest w;
	//I_Tower w;
	w.show();
	return a.exec();
}
