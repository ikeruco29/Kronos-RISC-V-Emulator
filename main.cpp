#include "display.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    int RAM_SIZE = 1 * 1024; //kb
    QApplication a(argc, argv);
    Display w;
    Computer computer = Computer(RAM_SIZE);
    w.show();
    w.computer = &computer;
    return a.exec();
}
