#include <mainwindow.h>
#include "choosedlg.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChooseDlg dlg;
    if(dlg.exec()){
        MainWindow w;
        w.show();
        return a.exec();
    };
    return 0;
}
