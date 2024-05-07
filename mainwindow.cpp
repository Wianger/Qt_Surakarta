#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBrush>
#include <QString>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <cmath>

const double PI = std::acos(-1.0);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1200,800);
    game.StartGame();
    ui->label->setText("Current_Player : " + str_player(game.game_info_->current_player_));
    countDown = CountDown;// 初始倒计时时间为10秒
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCountdown);
    timer->start(1000); // 每秒触发一次updateCountdown
    QPushButton *forfeitButton = new QPushButton("认输", this);
    forfeitButton->setGeometry(800, 150, 100, 40); // 设置按钮的位置和大小
    connect(forfeitButton, &QPushButton::clicked, this, &MainWindow::forfeitGame);

    tm.setInterval(10);
    connect(&tm, &QTimer::timeout, this, &MainWindow::MovePiece);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //倒计时
    QFont font = painter.font();
    font.setPointSize(20);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(800, 100, "倒计时：" + QString::number(countDown) + " 秒");

    //绘制棋盘
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    drawLine(painter);
    for (unsigned int var = 1; var < game.board_size_ / 2; ++var)
        drawCycle(painter, game.board_->squareSize * var);

    if(game.start){
        piece = game.coordinate(game.mf.x, game.mf.y);
        Set_Stra_Rot();
        game.start = false;
        tm.start();
    }
    drawStone(painter);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    QPointF pt = ev->pos();
    for (unsigned int i = 0; i < game.board_size_; ++i) {
        for (unsigned int j = 0; j < game.board_size_; ++j) {
            if(game.GetRowCol(pt, game.coordinate(i, j))){
                if(game.game_info_->from == SurakartaPosition(-1, -1))
                {
                    game.game_info_->from = SurakartaPosition(i, j);
                    MainWindow::is_select=1;
                    MainWindow::select_i=i;
                    MainWindow::select_j=j;
                    update();
                }
                else if(game.game_info_->from == SurakartaPosition(i, j)){
                    game.game_info_->from = SurakartaPosition(-1, -1);
                    MainWindow::is_select = 0;
                    update();
                }
                else{
                    game.game_info_->to = SurakartaPosition(i, j);
                    game.Move(SurakartaMove(game.game_info_->from,game.game_info_->to,game.game_info_->current_player_));
                    if (game.game_info_->IsEnd())
                         endGame();

                    else{
                        countDown = CountDown;
                        timer->start(1000);
                        updatePlayerInfo();
                    }
                    game.game_info_->from = SurakartaPosition(-1, -1);
                    MainWindow::is_select = 0;
                    update();
                }
                return;
            }

        }
    }
}
void MainWindow::updateCountdown()
{
    if (countDown > 0) {
        countDown--;
        update(); // 每秒更新界面
    } else {
        timer->stop();
        game.game_info_->end_reason_ = SurakartaEndReason::TIMEOUT;
        SurakartaPlayer cp = game.game_info_->current_player_;
        if(cp == PieceColor::BLACK)
            game.game_info_->winner_ = PieceColor::WHITE;
        else
            game.game_info_->winner_ = PieceColor::BLACK;
        endGame();
    }
}

void MainWindow::endGame()
{
    gameEnded = true;

    // 根据游戏结果显示信息
    QString endMessage;

    if (game.game_info_->end_reason_ == SurakartaEndReason::CHECKMATE) {
        endMessage = "玩家 " + str_player(game.game_info_->winner_) + " 获胜!";
    } else if (game.game_info_->end_reason_ == SurakartaEndReason::TIMEOUT) {
        endMessage = "玩家 " + str_player(game.game_info_->current_player_) + " 超时认输!" + "\n" + "Winner : " + str_player(game.game_info_->winner_);
    } else {
        endMessage = "游戏结束，失败原因 : " + endReasonToString(game.game_info_->end_reason_) + "\n" + "Winner : " + str_player(game.game_info_->winner_);
    }


    // 停止计时器
    timer->stop();

    // 显示游戏结束消息框
    QMessageBox::StandardButton result = QMessageBox::information(this, "游戏结束", endMessage);
    if (result == QMessageBox::Ok)
    {
        // 在用户点击确定后再询问是否再来一局
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "游戏结束", "是否再来一局", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // 重新开始游戏，包括重置参数和启动计时器
            restartGame();
        }
    }
}


QString MainWindow::endReasonToString(SurakartaEndReason endReason) {
    switch (endReason) {
    case SurakartaEndReason::NONE:
        return "NONE";
    case SurakartaEndReason::STALEMATE:
        return "STALEMATE";
    case SurakartaEndReason::CHECKMATE:
        return "CHECKMATE";
    case SurakartaEndReason::TRAPPED:
        return "TRAPPED";
    case SurakartaEndReason::RESIGN:
        return "RESIGN";
    case SurakartaEndReason::TIMEOUT:
        return "TIMEOUT";
    case SurakartaEndReason:: ILLIGAL_MOVE:
        return "ILLEGAL_MOVE";
    default:
        return "UNKNOWN";
    }
}

void MainWindow::drawLine(QPainter & painter)
{
    for (int var = game.board_->gapSize; var <= game.board_->gapSize + game.board_->boardSize; var += game.board_->squareSize) {
        QPoint pointStart(game.board_->gapSize,var);
        QPoint pointEnd(game.board_->gapSize + game.board_->boardSize,var);
        painter.drawLine(pointStart,pointEnd);
    }
    for (int var = game.board_->gapSize; var <= game.board_->gapSize + game.board_->boardSize; var+=game.board_->squareSize) {
        QPoint pointStart(var,game.board_->gapSize);
        QPoint pointEnd(var,game.board_->gapSize + game.board_->boardSize);
        painter.drawLine(pointStart,pointEnd);
    }
}

void MainWindow::drawCycle(QPainter &painter, int radius)
{
    QRectF rec(game.board_->gapSize - radius, game.board_->gapSize - radius, 2 * radius, 2 * radius);
    painter.drawArc(rec, 0, 270 * 16);
    rec.setRect(game.board_->gapSize + game.board_->boardSize - radius,game.board_->gapSize - radius, 2 * radius, 2 * radius);
    painter.drawArc(rec, -90 * 16, 270 * 16);
    rec.setRect(game.board_->gapSize - radius, game.board_->gapSize + game.board_->boardSize - radius, 2 * radius, 2 * radius);
    painter.drawArc(rec, 90 * 16, 270 * 16);
    rec.setRect(game.board_->gapSize + game.board_->boardSize - radius, game.board_->gapSize + game.board_->boardSize - radius, 2 * radius, 2 * radius);
    painter.drawArc(rec, 180 * 16, 270 * 16);
}

void MainWindow::drawStone(QPainter &painter)
{
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    for (unsigned int i = 0; i < game.board_->board_size_; ++i) {
        for (unsigned int j = 0; j < game.board_->board_size_; ++j) {
            if((*game.board_)[i][j]->color_ != PieceColor::NONE){
                if((*game.board_)[i][j]->color_ == PieceColor::BLACK)
                    brush.setColor(Qt::black);
                else if((*game.board_)[i][j]->color_ == PieceColor::WHITE)
                    brush.setColor(Qt::white);
                painter.setBrush(brush);
                if(is_select && i==select_i && j==select_j)
                    painter.drawEllipse(game.coordinate(i, j), (*game.board_)[i][j]->expand_r_ , (*game.board_)[i][j]->expand_r_);
                else
                    painter.drawEllipse(game.coordinate(i, j), (*game.board_)[i][j]->r_, (*game.board_)[i][j]->r_);

            }
        }
    }
    if(game.is_captured){
        if(game.game_info_->current_player_ == PieceColor::BLACK)
            brush.setColor(Qt::white);
        else
            brush.setColor(Qt::black);
        painter.setBrush(brush);
        painter.drawEllipse(piece, game.piece_r, game.piece_r);
    }
}

QString MainWindow::str_player(SurakartaPlayer pl)
{
    QString player;
    if(pl == PieceColor::BLACK)
        player = "Black";
    else
        player = "White";
    return player;
}

void MainWindow::updatePlayerInfo()
{
    ui->label->setText("Current_Player : " + str_player(game.game_info_->current_player_));
}

void MainWindow::restartGame()
{
    // 重置游戏参数
    game.StartGame();

    // 更新界面
    updatePlayerInfo();
    countDown = CountDown;
    timer->start(1000);
    game.start = false, game.is_captured = false;
    update();
}

void MainWindow::forfeitGame() {
    // 根据当前玩家确定哪个棋子认输
    SurakartaPlayer current_player = game.game_info_->current_player_;

    // 改变棋子颜色或在界面上显示认输信息
    if (current_player == PieceColor::BLACK)
        // 标记黑棋认输，可根据需求改变颜色或显示文字等
        QMessageBox::information(this, "认输", "黑棋认输");
    else if (current_player == PieceColor::WHITE)
        // 标记白棋认输，可根据需求改变颜色或显示文字等
        QMessageBox::information(this, "认输", "白棋认输");

    // 重新开始游戏
    game.game_info_->end_reason_=SurakartaEndReason::RESIGN;
    SurakartaPlayer cp = game.game_info_->current_player_;
    if(cp == PieceColor::BLACK)
        game.game_info_->winner_ = PieceColor::WHITE;
    else
        game.game_info_->winner_ = PieceColor::BLACK;
    endGame();
}

void MainWindow::Set_Stra_Rot()
{
    if(piece.x() == game.coordinate(0, 0).x() || piece.x() == game.coordinate(game.board_size_ - 1, 0).x()){
        rotation = true;
        Set_Circle();
    }
    else if(piece.y() == game.coordinate(0, 0).y() || piece.y() == game.coordinate(0, game.board_size_ - 1).y()){
        rotation = true;
        Set_Circle();
    }
    else
        rotation = false, straight = true;
}

void MainWindow::MovePiece_Circle()
{
    piece.setX(center.x() + (game.board_->squareSize * game.rule_manager_->circle) * cos(angle));
    piece.setY(center.y() + (game.board_->squareSize * game.rule_manager_->circle) * sin(angle));
    angle += (PI / 60) * (game.rule_manager_->clockwise);
}

void MainWindow::MovePiece_line()
{
    int clock = game.rule_manager_->clockwise;
    switch (game.rule_manager_->road) {
    case 0:
        piece.setY(piece.y() - 0.1 * clock);
        break;
    case 1:
        piece.setX(piece.x() - 0.1 * clock);
        break;
    case 2:
        piece.setY(piece.y() + 0.1 * clock);
        break;
    case 3:
        piece.setX(piece.x() + 0.1 * clock);
        break;
    default:
        break;
    }
}

void MainWindow::MovePiece()
{
    if(piece == game.coordinate(game.mt.x, game.mt.y))
        game.is_captured = false;
    else{
        if(straight)
            MovePiece_line();
        else if(rotation){
            MovePiece_Circle();
        }
    }
}

void MainWindow::Set_Center()
{
    int road = game.rule_manager_->road, clock = game.rule_manager_->clockwise;
    unsigned int size = game.board_size_;
    if((road == 0 && clock == 1) || (road == 3 && clock == -1))
        center = game.coordinate(0, 0);
    else if((road == 0 && clock == -1) || (road == 1 && clock == 1))
        center = game.coordinate(0, size - 1);
    else if((road == 2 && clock == -1) || (road == 3 && clock == 1))
        center = game.coordinate(size - 1, 0);
    else if((road == 1 && clock == -1) || (road == 2 && clock == 1))
        center = game.coordinate(size - 1, size - 1);
}

void MainWindow::Set_Angle()
{
    int road = game.rule_manager_->road;
    if(road == 0)
        angle = 0;
    else if(road == 1)
        angle = PI / 2;
    else if(road == 2)
        angle = PI;
    else if(road == 3)
        angle = - PI / 2;
}

void MainWindow::Set_Circle()
{
    Set_Center();
    Set_Angle();
}



