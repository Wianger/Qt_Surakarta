#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBrush>
#include <QString>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1200,800);
    game.StartGame();
    ui->label->setText("Current_Player : " + str_player(game.game_info_->current_player_));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //绘制棋盘
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    drawLine(painter);
    drawCycle(painter, game.board_->squareSize);
    drawCycle(painter, game.board_->squareSize * 2);
    drawStone(painter);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    QPointF pt = ev->pos();
    for (unsigned int i = 0; i < game.board_size_; ++i) {
        for (unsigned int j = 0; j < game.board_size_; ++j) {
            if(game.GetRowCol(pt, game.coordinate(i, j))){
                if(game.game_info_->from == SurakartaPosition(-1, -1))
                    game.game_info_->from = SurakartaPosition(i, j);
                else{
                    game.game_info_->to = SurakartaPosition(i, j);
                    game.Move(SurakartaMove(game.game_info_->from, game.game_info_->to, game.game_info_->current_player_));
                    game.game_info_->from = SurakartaPosition(-1, -1);
                    updatePlayerInfo();
                    update();
                }
                return;
            }

        }
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
                painter.drawEllipse(game.coordinate(i, j), (*game.board_)[i][j]->r_ , (*game.board_)[i][j]->r_);
            }
        }
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



