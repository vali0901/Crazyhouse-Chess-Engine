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
        static std::vector<Move> calculateMoves(uint8_t piececode, uint8_t table[8][8]);
        // TODO: deciding how to use those additional bits and some functions for manipulating them

};

#endif // PIECEHNDLR_H
