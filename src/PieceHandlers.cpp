#include "Piece.h"
#include "PieceHandlers.h"
#include "Move.h"

#include <bits/stdc++.h>


const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::knight_directions =
    {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::rook_directions =
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::bishop_directions =
    {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::queen_directions =
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::king_directions =
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};




uint8_t PieceHandlers::createPiece(Piece type, PlaySide side) {
    uint8_t piececode = 0;
    piececode |= type;

    if(type != NAP)
        piececode |= side;

    return piececode;
}

Piece PieceHandlers::getType(uint8_t piececode)
{   
    // i found a smarter way to do this :)
    return (Piece)(piececode & piece_type_mask);

    // if ((code & KING) == KING)
    //     return KING;
    
    // if ((code & QUEEN) == QUEEN)
    //     return QUEEN;
    
    // if ((code & KNIGHT) == KNIGHT)
    //     return KNIGHT;
    
    // if ((code & BISHOP) == BISHOP)
    //     return BISHOP;
    
    // if ((code & ROOK) == ROOK)
    //     return ROOK;

    // if ((code & PAWN) == PAWN)
    //     return PAWN;    

    // return NAP;
}

PlaySide PieceHandlers::getColor(uint8_t piececode) {
    // same here
    return(PlaySide)(piececode & playsidemask);
    // if((code & WHITE) == WHITE)
    //     return WHITE;
    
    // if((code & BLACK) == BLACK)
    //     return BLACK;
    
    // return NONE;
}

void PieceHandlers::setAttackedBy(uint8_t &piececode, PlaySide attacker_color) {
    switch (attacker_color)
    {
    case WHITE:
        piececode |= ATTACKED_BY_WHITE;
        break;
    case BLACK:
        piececode |= ATTACKED_BY_BLACK;
        break;
    default:
        printf("WTF!!! ATTACKER DOESN'T HAVE A COLOR, THIS SHOULDN'T HAPPEN!!!\n");
        break;
    }
}

std::vector<Move> PieceHandlers::calculateMoves(uint8_t piececode, uint8_t table[8][8]) {
    // TODO: Depending on piecetype, call a (private) method for generating the possible moves for this piece
    // We can then filter through these possible moves and choose only the legal ones (in case of check or whatever)
    // Another way to do this is by giving some constraints to this method, with which we can tell the situation
    // on the table, if my king is in check, or if i move this piece it will get into check or shit like that.
    // I prefer the second option, but we'll have to figure out how to do those constraints. I'm thinking another
    // matrix which gives info about enemy's attacking directions and my attacked pieces. This will be useful later
    // when we will judge what move we are going to do, based on what pieces can we lose, like we will want to see
    // if my pawn and my bishop is attacked by the enemy's knight, and we are going to chose to save the bishop in 
    // this situation (this might be done using those additional bits, but we'll think about it) 
    std::vector<Move> tmp;
    return tmp;
}
