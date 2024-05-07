#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <game.h>
#include <QPainter>
#include <QMouseEvent>
#include <rulemanager.h>
#include <QString>
#include <QTimer>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SurakartaGame game;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void drawLine(QPainter&);
    void drawCycle(QPainter&, int);
    void drawStone(QPainter&);
    void endGame();
    QString endReasonToString(SurakartaEndReason endReason);
    QString str_player(SurakartaPlayer);
    void updatePlayerInfo(); // 更新玩家信息的槽函数
    bool is_select=0;
    bool gameEnded;
    unsigned int select_i;
    unsigned int select_j;
    int CountDown = 30;
    void restartGame();
    void forfeitGame();

private:
    Ui::MainWindow *ui;
    int countDown;  // 倒计时剩余秒数
    QTimer *timer;  // 定时器对象

private slots:
    void updateCountdown();



};
#endif // MAINWINDOW_H
