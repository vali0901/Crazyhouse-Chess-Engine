#ifndef PIECEHNDLR_H
#define PIECEHNDLR_H

#include "Move.h"
#include "Piece.h"
#include <bits/stdc++.h>


class PieceHandlers {
    public:
        static const std::vector<std::pair<int8_t, int8_t>> knight_directions;
        static const std::vector<std::pair<int8_t, int8_t>> rook_directions;
        static const std::vector<std::pair<int8_t, int8_t>> bishop_directions;
        static const std::vector<std::pair<int8_t, int8_t>> queen_directions;
        static const std::vector<std::pair<int8_t, int8_t>> king_directions;
        static uint8_t createPiece(Piece type, PlaySide side);
        static Piece getType(uint8_t piececode);
        static PlaySide getColor(uint8_t piececode);
        static void setAttackedBy(uint8_t &piececode, PlaySide attacker_color);
        static bool slotIsSafe(uint8_t slotcode, PlaySide mycolor);
        static bool isProtectorOfTheKing(uint8_t piececode, PlaySide protected_color);
        static bool isAttackerOfTheKing(uint8_t piececode, PlaySide attacked_color);
        static std::vector<Move> calculateMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);
        static std::vector<Move> calculateProtectorOfTheKingMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t kingcode, int8_t kx, int8_t ky, uint8_t table[8][8], Move last_move);
        static std::vector<Move> calculateKingInCheckMoves(uint8_t kingcode, int8_t kx, int8_t ky, uint8_t table[8][8], Move last_move);
    
    private:
        static std::vector<Move> calculatePawnMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);
        static std::vector<Move> calculateKnightMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);
        static std::vector<Move> calculateRookMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);
        static std::vector<Move> calculateBishopMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);
        static std::vector<Move> calculateQueenMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);
        static std::vector<Move> calculateKingMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints);

};

#endif // PIECEHNDLR_H
