#include "rulemanager.h"
#include <cmath>
#include <iostream>
#include <vector>

SurakartaPosition SurakartaRuleManager::row_line(SurakartaPosition p, circle c) {
    // 将具体的行列映射到outer/inner中的行列
    int r = 4, l = 4;
    if (c == circle::out) {
        if (p.x == 2)
            r = 0;
        else if (p.x == 3)
            r = 2;
        if (p.y == 3)
            l = 1;
        else if (p.y == 2)
            l = 3;

    } else if (c == circle::in) {
        if (p.x == 1)
            r = 0;
        else if (p.x == 4)
            r = 2;
        if (p.y == 4)
            l = 1;
        else if (p.y == 1)
            l = 3;
    }

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
    std::vector<std::vector<SurakartaPiece>> outer, inner;  // outer: x2,y3,x3,y2
        // inner: x1,y4,x4,y1
    std::vector<SurakartaPosition> outerlin, innerlin;      // 外/内圈相邻直线
    std::vector<SurakartaPiece> t;
    int direction[3] = {0, 1};               // 初始移动方向
    int rotation[3][3] = {{0, 1}, {-1, 0}};  // 逆时针旋转90度
    SurakartaPosition op(2, 0), ip(1, 0);    // 外圈和内圈的起始位置

    do {
        // 取出外圈上的所有棋子
        if ((*board_)[op.x][op.y]->color_ != PieceColor::NONE) {
            t.push_back(*(*board_)[op.x][op.y]);
        }
        op.x += direction[0], op.y += direction[1];
        if (op.x == (unsigned)-1 || op.x == 6 || op.y == (unsigned)-1 || op.y == 6) {
            outer.push_back(t);
            t.clear();
            if (op.x == (unsigned)-1 || op.x == 6) {
                op.y = (op.x == (unsigned)-1 ? 0 : 5);
                op.x -= 3 * direction[0];
            } else if (op.y == (unsigned)-1 || op.y == 6) {
                op.x = (op.y == (unsigned)-1 ? 5 : 0);
                op.y -= 3 * direction[1];
            }
            int temp1 = direction[0], temp2 = direction[1];
            direction[0] = rotation[0][0] * temp1 + rotation[0][1] * temp2;
            direction[1] = rotation[1][0] * temp1 + rotation[1][1] * temp2;
        }

    } while (op != SurakartaPosition(2, 0));
    // 外圈的相邻直线(逆时针方向)
    for (int i = 0; i < 4; i++) {
        outerlin.push_back(SurakartaPosition(i, (i + 1) % 4));
    }
    for (int i = 0; i < 4; i++) {
        if (outer[i].empty()) {
            continue;
        }
        int j = (i + 1) % 4;
        do {
            if (!outer[j].empty()) {
                outerlin.push_back(SurakartaPosition(i, j));
                break;
            }
            j = (j + 1) % 4;
        } while (j != (i + 1) % 4);
    }

    t.clear();
    direction[0] = 0, direction[1] = 1;
    do {
        // 取出内圈上的所有棋子
        if ((*board_)[ip.x][ip.y]->color_ != PieceColor::NONE) {
            t.push_back(*(*board_)[ip.x][ip.y]);
        }
        ip.x += direction[0], ip.y += direction[1];
        if (ip.x == (unsigned)-1 || ip.x == 6 || ip.y == (unsigned)-1 || ip.y == 6) {
            inner.push_back(t);
            t.clear();
            if (ip.x == (unsigned)-1 || ip.x == 6) {
                ip.y = (ip.x == (unsigned)-1 ? 0 : 5);
                ip.x -= 2 * direction[0];
            } else if (ip.y == (unsigned)-1 || ip.y == 6) {
                ip.x = (ip.y == (unsigned)-1 ? 5 : 0);
                ip.y -= 2 * direction[1];
            }
            int temp1 = direction[0], temp2 = direction[1];
            direction[0] = rotation[0][0] * temp1 + rotation[0][1] * temp2;
            direction[1] = rotation[1][0] * temp1 + rotation[1][1] * temp2;
        }
    } while (ip.x != 1 || ip.y != 0);
    for (int i = 0; i < 4; i++) {
        innerlin.push_back(SurakartaPosition(i, (i + 1) % 4));
    }
    // 内圈的相邻直线(逆时针方向)
    for (int i = 0; i < 4; i++) {
        if (inner[i].empty()) {
            continue;
        }
        int j = (i + 1) % 4;
        do {
            if (!inner[j].empty()) {
                innerlin.push_back(SurakartaPosition(i, j));
                break;
            }
            j = (j + 1) % 4;
        } while (j != (i + 1) % 4);
    }

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
        SurakartaPosition ftemp = row_line(mf, circle::out), ttemp = row_line(mt, circle::out);  // 两个位置在外圈的位置

        for (std::vector<SurakartaPosition>::size_type i = 0; i < outerlin.size(); i++) {
            auto x = outerlin[i].x, y = outerlin[i].y;
            if ((ftemp.x == x || ftemp.y == x) && (ttemp.x == y || ttemp.y == y)) {
                // 两个位置在相邻的直线上
                int id = outer[x].size() - 1;
                if (mf == outer[x][id].position_ && mf == outer[y][0].position_)
                    // 特判争议下法
                    if (mt == outer[y][1].position_)
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (mf == outer[x][id].position_ && mt == outer[y][0].position_)
                    // the source position是X上最后一个棋子，the target position是Y上第一个棋子
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            }
            // 考虑顺时针吃子的情况，相当于由target position到source position
            if ((ftemp.x == y || ftemp.y == y) && (ttemp.x == x || ttemp.y == x)) {
                int id = outer[x].size() - 1;
                if (mf == outer[y][0].position_ && mf == outer[x][id].position_)
                    if (mt == outer[x][id - 1].position_)
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (mf == outer[y][0].position_ && mt == outer[x][id].position_)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            }
        }

        ftemp = row_line(mf, circle::in), ttemp = row_line(mt, circle::in);  // 两个位置在内圈的位置

        for (std::vector<SurakartaPosition>::size_type i = 0; i < innerlin.size(); i++) {
            auto x = innerlin[i].x, y = innerlin[i].y;
            if ((ftemp.x == x || ftemp.y == x) && (ttemp.x == y || ttemp.y == y)) {
                auto id = inner[x].size() - 1;
                if (mf == inner[x][id].position_ && mf == inner[y][0].position_)
                    if (mt == inner[y][1].position_)
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (mf == inner[x][id].position_ && mt == inner[y][0].position_)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
            }
            if ((ftemp.x == y || ftemp.y == y) && (ttemp.x == x || ttemp.y == x)) {
                auto id = inner[x].size() - 1;
                if (mf == inner[y][0].position_ && mf == inner[x][id].position_)
                    if (mt == inner[x][id - 1].position_)
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                if (mf == inner[y][0].position_ && mt == inner[x][id].position_)
                    return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
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
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
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
