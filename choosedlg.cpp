#include "choosedlg.h"
#include <QVBoxLayout>
#include <mainwindow.h>

ChooseDlg::ChooseDlg(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->addWidget(_button = new QPushButton("开始游戏"));
    connect(_button, SIGNAL(clicked()), this, SLOT(slotClicked()));
    resize(100, 100);
}

void ChooseDlg::slotClicked()
{
    accept();
}

ChooseDlg::~ChooseDlg()
{

}
