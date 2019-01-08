#include "datamanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication myApp(argc, argv);

    DataManager data;
    data.show();

    return myApp.exec();
}
