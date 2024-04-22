#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <game.h>
#include <QPainter>
#include <QMouseEvent>
#include <rulemanager.h>
#include <QString>
#include <QTimer>

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
    void endGame(const SurakartaMoveResponse& response);
    QString endReasonToString(SurakartaEndReason endReason);
    QString str_player(SurakartaPlayer);
    void updatePlayerInfo(); // 更新玩家信息的槽函数
    bool is_select=0;
    bool gameEnded;
    int select_i;
    int select_j;
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
