#include "choosedlg.h"
#include <QVBoxLayout>

ChooseDlg::ChooseDlg(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->addWidget(_button = new QPushButton("开始游戏"));
    connect(_button, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

void ChooseDlg::slotClicked()
{
    accept();
}

ChooseDlg::~ChooseDlg()
{

}

int ChooseDlg::run()
{
    ChooseDlg dlg;
    if(dlg.exec() != QDialog::Accepted)
        return 0;
    return 1;
}
