#ifndef PIECE_H
#define PIECE_H

#include "PlaySide.h"
#include <bits/stdc++.h>

#define piece_type_mask 0b01110000
#define piece_state_mask 0b00001111

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
    
enum PieceState {
    NOT_ATTACKED = 0,
    ATTACKED_BY_WHITE = 0b00001000,
    ATTACKED_BY_BLACK = 0b00000100,
    ATTACKER_OF_THE_KING = 0b00000010,
    PROTECTOR_OF_THE_KING = 0b00000001
    };


#endif
