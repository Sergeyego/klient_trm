#include <QApplication>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int id_oven=19;
    int interval=60;
    bool manual=false;
    if (argc>1){
        id_oven=QString(argv[1]).toInt();
    }
    if (argc>2){
        interval=QString(argv[2]).toInt();
    }

    if (argc>3){
        manual=QString(argv[3]).toInt()==1;
    }

    MainWidget w(id_oven,interval,manual);
    w.show();
    return a.exec();
}
