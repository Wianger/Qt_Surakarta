#ifndef CHOOSEDLG_H
#define CHOOSEDLG_H

#include <QDialog>
#include <QPushButton>
#include <QTimer>

class ChooseDlg : public QDialog
{
    Q_OBJECT
public:
    explicit ChooseDlg(QWidget *parent = 0);
    ~ChooseDlg();

    QPushButton* _button;
signals:

public slots:
    void slotClicked();
};

#endif // CHOOSEDLG_H
