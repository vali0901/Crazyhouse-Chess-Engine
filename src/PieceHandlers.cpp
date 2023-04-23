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

std::vector<Move> PieceHandlers::calculateMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    
    // Irrelevant TODO
    // TODO: Depending on piecetype, call a (private) method for generating the possible moves for this piece
    // We can then filter through these possible moves and choose only the legal ones (in case of check or whatever)
    // Another way to do this is by giving some constraints to this method, with which we can tell the situation
    // on the table, if my king is in check, or if i move this piece it will get into check or shit like that.
    // I prefer the second option, but we'll have to figure out how to do those constraints. I'm thinking another
    // matrix which gives info about enemy's attacking directions and my attacked pieces. This will be useful later
    // when we will judge what move we are going to do, based on what pieces can we lose, like we will want to see
    // if my pawn and my bishop is attacked by the enemy's knight, and we are going to chose to save the bishop in 
    // this situation (this might be done using those additional bits, but we'll think about it) 
    
    switch (PieceHandlers::getType(piececode))
    {
    case PAWN:
        return PieceHandlers::calculatePawnMoves(piececode, x, y, table, last_move);
        break;
    case KNIGHT:
        return PieceHandlers::calculateKnightMoves(piececode, x, y, table, last_move);
        break;
    case ROOK:
        return PieceHandlers::calculateRookMoves(piececode, x, y, table, last_move);
        break;
    case BISHOP:
        return PieceHandlers::calculateBishopMoves(piececode, x, y, table, last_move);
        break;
    case QUEEN:
        return PieceHandlers::calculateQueenMoves(piececode, x, y, table, last_move);
        break;
    case KING:
        return PieceHandlers::calculateKingMoves(piececode, x, y, table, last_move);
        break;
    default:
        break;
    }
    
}

/*
    For these methods, you need to declare a vector of moves
    std::vector<Move> myvct
    and when you find a possible legal move, add it like this
    myvct.push_back(Move::moveTo(src, dest))
    where src and dest are strings (i.e. "e1", "f6", "a4" etc.)
    You'll need to use some function to convert from indexes to
    strings. (or maybe not)
*/

std::vector<Move> PieceHandlers::calculatePawnMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    /*
        TODO:
        Generate all the moves this pawn can make on this table.
        This means it can either go up / down (depending on color)
        only if there is an empty slot available (dont forget it can
        jump 2 slots, if it hasn't moved yet), or it can attack on
        diagonal. Also, check enpassant rule, based on last_move.
        Create a vector with all the moves you found.
    */
}

std::vector<Move> PieceHandlers::calculateKnightMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    /*
        TODO:
        Generate all the possible moves of this piece, go through
        the directions declared in PieceHandlers, check if the
        possible_move's coordinates are valid (check the Table::update_states()
        method so you can see how exactly this is done), and if that slot is
        empty, add the move in the vector. For each direction, stop when
        you hit another piece (!= NAP), and if this piece has a different
        color than our initial piece (and it is not the king) add it to
        the vector (this means out initial piece can attack an enemy piece)
    */
}

std::vector<Move> PieceHandlers::calculateRookMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    /*
        TODO:
        Generate all the possible moves of this piece, go through
        the directions declared in PieceHandlers, check if the
        possible_move's coordinates are valid (check the Table::update_states()
        method so you can see how exactly this is done), and if that slot is
        empty, add the move in the vector. For each direction, stop when
        you hit another piece (!= NAP), and if this piece has a different
        color than our initial piece (and it is not the king) add it to
        the vector (this means out initial piece can attack an enemy piece)
    */
}

std::vector<Move> PieceHandlers::calculateBishopMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    /*
        TODO:
        Generate all the possible moves of this piece, go through
        the directions declared in PieceHandlers, check if the
        possible_move's coordinates are valid (check the Table::update_states()
        method so you can see how exactly this is done), and if that slot is
        empty, add the move in the vector. For each direction, stop when
        you hit another piece (!= NAP), and if this piece has a different
        color than our initial piece (and it is not the king) add it to
        the vector (this means out initial piece can attack an enemy piece)
    */
}

std::vector<Move> PieceHandlers::calculateQueenMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    /*
        TODO:
        Generate all the possible moves of this piece, go through
        the directions declared in PieceHandlers, check if the
        possible_move's coordinates are valid (check the Table::update_states()
        method so you can see how exactly this is done), and if that slot is
        empty, add the move in the vector. For each direction, stop when
        you hit another piece (!= NAP), and if this piece has a different
        color than our initial piece (and it is not the king) add it to
        the vector (this means out initial piece can attack an enemy piece)
    */
}

std::vector<Move> PieceHandlers::calculateKingMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move) {
    /*
        TODO:
        Generate all the possible moves of this piece, go through
        the directions declared in PieceHandlers, check if the
        possible_move's coordinates are valid (check the Table::update_states()
        method so you can see how exactly this is done), and if that slot is
        empty, add the move in the vector. If the slot is not empty and the
        piece occupying the slot has a different color than my King, it can
        be attacked, so also add this move to the vector.

        The king might only go on a slot that is not under attack of another
        enemy piece, so you also need to check that the slot you are trying
        to go is safe.
    */
}