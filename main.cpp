#include <mainwindow.h>
#include "choosedlg.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ChooseDlg dlg;
    while(dlg.run()){
        w.show();
        return a.exec();
    }
}
