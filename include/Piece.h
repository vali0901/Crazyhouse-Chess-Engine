#ifndef PIECE_H
#define PIECE_H

#include "PlaySide.h"
#include <bits/stdc++.h>

#define piece_mask 0b01110000

/*
    Each piece bit layout:
        First bit -> Color 
        Next 3 bits -> Piecetype
        Next 4 bits -> Additional info
*/

enum Piece {
    PAWN = 0b00010000,
    ROOK = 0b00100000,
    BISHOP = 0b00110000,
    KNIGHT = 0b01000000,
    QUEEN = 0b01010000,
    KING = 0b01100000,
    NAP = 0
    };
    


#endif
