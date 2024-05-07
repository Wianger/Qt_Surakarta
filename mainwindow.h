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

    QPointF piece;
    int ini_angle;
    double angle;
    QPointF center;
    bool is_initial = false;
    bool straight = false, rotation = false;
    void Set_Stra_Rot();
    void MovePiece_line();
    void MovePiece_Circle();
    void Set_Center();
    void Set_Angle();
    void Set_Circle();

private:
    Ui::MainWindow *ui;
    int countDown;  // 倒计时剩余秒数
    QTimer *timer;  // 定时器对象
    QTimer tm;

private slots:
    void updateCountdown();
    void MovePiece();



};
#endif // MAINWINDOW_H
