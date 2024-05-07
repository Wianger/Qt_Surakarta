#include "rulemanager.h"
#include <cstdlib>
#include <iostream>
#include <vector>

SurakartaPosition SurakartaRuleManager::row_line(SurakartaPosition p, unsigned int c) {
    // 将具体的行列映射到outer/inner中的行列
    int r = 4, l = 4;
    if (p.x == c)
        r = 0;
    else if (p.x == board_size_ - 1 - c)
        r = 2;
    if (p.y == c)
        l = 3;
    else if (p.y == board_size_ - 1 - c)
        l = 1;

    return SurakartaPosition(r, l);
}

void SurakartaRuleManager::OnUpdateBoard() {
    // TODO:
    // Every time the board and game_info is updated to the next round version, this function will be called.
    // You don't need to implement this function if you don't need it.
    // A more delicate way is to use Qt's signal and slot mechanism, but that's the advanced part.
}

SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move) {
    // TODO: Implement this function.
    SurakartaPlayer current_player = game_info_->current_player_;
    SurakartaPosition mf = move.from, mt = move.to;

    if (move.player != current_player) {
        // It's not the player's turn.
        return SurakartaIllegalMoveReason::NOT_PLAYER_TURN;
    } else if (!board_->IsInside(mf) || !board_->IsInside(mt)) {
        // The position is out of the board.
        return SurakartaIllegalMoveReason::OUT_OF_BOARD;
    }
    PieceColor color1 = (*board_)[mf.x][mf.y]->GetColor();
    PieceColor color2 = (*board_)[mt.x][mt.y]->GetColor();
    if (color1 == PieceColor::NONE) {
        // There is no piece at the position.
        return SurakartaIllegalMoveReason::NOT_PIECE;
    } else if (color1 != current_player) {
        // The piece at the position is not the player's piece.
        return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;
    }

    if (color2 == current_player)
        // The target position is occupied by the player's piece.
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    else if (color2 == PieceColor::NONE) {
        // The target position is empty.
        if (mf.x - mt.x == 0 && mf.y - mt.y == 0)
            // The target position is the same as the source position.
            return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
        else if (abs((int)mf.x - (int)mt.x) > 1 || abs((int)mf.y - (int)mt.y) > 1) {
            //
            return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
        } else
            return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
    } else {
        // The target position is occupied by the opponent's piece.
        for (unsigned int i = 1; i < board_size_ / 2; i++) {
            SurakartaPosition ftemp, ttemp;
            int direction[3] = {0, 1};               // 初始移动方向
            int rotation[3][3] = {{0, 1}, {-1, 0}};  // 逆时针旋转90度
            std::vector<std::vector<SurakartaPiece>> piece;
            std::vector<SurakartaPosition> line;
            std::vector<SurakartaPiece> t;
            SurakartaPosition start(i, 0);  // 起始位置

            do {
                // 取出外圈上的所有棋子
                if ((*board_)[start.x][start.y]->color_ != PieceColor::NONE) {
                    t.push_back(*(*board_)[start.x][start.y]);
                }
                start.x += direction[0], start.y += direction[1];
                if (start.x == (unsigned)-1 || start.x == board_size_ || start.y == (unsigned)-1 || start.y == board_size_) {
                    piece.push_back(t);
                    t.clear();
                    if (start.x == (unsigned)-1 || start.x == board_size_) {
                        start.y = (start.x == (unsigned)-1 ? 0 : board_size_ - 1);
                        start.x -= (i + 1) * direction[0];
                    } else if (start.y == (unsigned)-1 || start.y == board_size_) {
                        start.x = (start.y == (unsigned)-1 ? board_size_ - 1 : 0);
                        start.y -= (i + 1) * direction[1];
                    }
                    int temp1 = direction[0], temp2 = direction[1];
                    direction[0] = rotation[0][0] * temp1 + rotation[0][1] * temp2;
                    direction[1] = rotation[1][0] * temp1 + rotation[1][1] * temp2;
                }

            } while (start != SurakartaPosition(i, 0));

            for (int j = 0; j < 4; j++) {
                line.push_back(SurakartaPosition(j, (j + 1) % 4));
            }

            for (int k = 0; k < 4; k++) {
                if (piece[k].empty())
                    continue;

                int j = (k + 1) % 4;
                do {
                    if (!piece[j].empty()) {
                        line.push_back(SurakartaPosition(k, j));
                        break;
                    }
                    j = (j + 1) % 4;
                } while (j != (k + 1) % 4);
            }

            ftemp = row_line(mf, i), ttemp = row_line(mt, i);
            for (std::vector<SurakartaPosition>::size_type j = 0; j < line.size(); j++) {
                auto x = line[j].x, y = line[j].y;
                if ((ftemp.x == x || ftemp.y == x) && (ttemp.x == y || ttemp.y == y)) {
                    // 两个位置在相邻的直线上
                    int id = piece[x].size() - 1;
                    if (mf == piece[x][id].position_ && mf == piece[y][0].position_)
                        // 特判争议下法
                        if (mt == piece[y][1].position_){
                            circle = i, clockwise = -1;
                            road = x;
                            return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                        }
                    if (mf == piece[x][id].position_ && mt == piece[y][0].position_){
                        // the source position是X上最后一个棋子，the target position是Y上第一个棋子
                        circle = i, clockwise = -1;
                        road = x;
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                    }
                }
                // 考虑顺时针吃子的情况，相当于由target position到source position
                if ((ftemp.x == y || ftemp.y == y) && (ttemp.x == x || ttemp.y == x)) {
                    int id = piece[x].size() - 1;
                    if (mf == piece[y][0].position_ && mf == piece[x][id].position_)
                        if (mt == piece[x][id - 1].position_)
                    if (mf == piece[y][0].position_ && mt == piece[x][id].position_){
                        circle = i, clockwise = 1;
                        road = y;
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                    }
                    {
                        circle = i, clockwise = 1;
                        road = y;
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                    }        }
            }
        }
        return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
    }
    return SurakartaIllegalMoveReason::LEGAL;
}

std::pair<SurakartaEndReason, SurakartaPlayer> SurakartaRuleManager::JudgeEnd(const SurakartaIllegalMoveReason reason) {
    // TODO: Implement this function.
    // Note that at this point, the board and game_info have not been updated yet.
    SurakartaPlayer opponent_player = (game_info_->current_player_ == SurakartaPlayer::BLACK) ? SurakartaPlayer::WHITE : SurakartaPlayer::BLACK;  // 对手
    // 判断是否有非法走子
    if (reason == SurakartaIllegalMoveReason::OUT_OF_BOARD) {
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, opponent_player);
    } else if (reason == SurakartaIllegalMoveReason::NOT_PIECE) {
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, opponent_player);
    } else if (reason == SurakartaIllegalMoveReason::NOT_PLAYER_PIECE) {
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, opponent_player);
    } else if (reason == SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE) {
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, opponent_player);
    } else if (reason == SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE) {
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, opponent_player);
    } else if (reason == SurakartaIllegalMoveReason::NOT_PLAYER_TURN) {
        return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, opponent_player);
    }

    // 计算黑白棋子数
    int b_num = 0, w_num = 0;
    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            if ((*board_)[i][j]->color_ == PieceColor::BLACK)
                b_num++;
            else if ((*board_)[i][j]->color_ == PieceColor::WHITE)
                w_num++;
        }
    }
    if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
        if (game_info_->current_player_ == SurakartaPlayer::BLACK)
            w_num--;
        else
            b_num--;
    }
    // 判断一方棋子数是否为0
    if (b_num == 0)
        return std::make_pair(SurakartaEndReason::CHECKMATE, SurakartaPlayer::WHITE);
    else if (w_num == 0)
        return std::make_pair(SurakartaEndReason::CHECKMATE, SurakartaPlayer::BLACK);

    SurakartaPlayer winner;  // 判断此时场上剩余棋子数更多的一方为胜者
    if (b_num > w_num) {
        winner = SurakartaPlayer::BLACK;
    } else if (b_num < w_num) {
        winner = SurakartaPlayer::WHITE;
    } else {
        winner = SurakartaPlayer::NONE;
    }
    // 判断是否和棋
    if (reason != SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE && game_info_->num_round_ - game_info_->last_captured_round_ >= game_info_->max_no_capture_round_) {
        return std::make_pair(SurakartaEndReason::STALEMATE, winner);
    }

    return std::make_pair(SurakartaEndReason::NONE, SurakartaPlayer::NONE);
}

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaRuleManager::GetAllLegalTarget(const SurakartaPosition postion) {
    // TODO:
    // We don't test this function, you don't need to implement this function if you don't need it.
    (void)postion;
    return std::make_unique<std::vector<SurakartaPosition>>();
}

void SurakartaRuleManager::HelloWorld() {
    std::cout << "Hello World!" << std::endl;
}
