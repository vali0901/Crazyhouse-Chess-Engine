#ifndef PIECEHNDLR_H
#define PIECEHNDLR_H

#include "Move.h"
#include "Piece.h"
#include <bits/stdc++.h>


class PieceHandlers {
    public:
        static uint8_t createPiece(Piece type, PlaySide side);
        static Piece getType(uint8_t piececode);
        static PlaySide getColor(uint8_t piececode);
        static std::vector<Move> calculateMoves(uint8_t piececode, uint8_t table[8][8]);
        // TODO: deciding how to use those additional bits and some functions for manipulating them

};

#endif // PIECEHNDLR_H
