#include "Piece.h"
#include "PieceHandlers.h"
#include "Move.h"

#include <bits/stdc++.h>


const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::knight_directions =
    {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::rook_directions =
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::bishop_directions =
    {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::queen_directions =
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const std::vector<std::pair<int8_t, int8_t>> PieceHandlers::king_directions =
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};




uint8_t PieceHandlers::createPiece(Piece type, PlaySide side) {
    uint8_t piececode = 0;
    piececode |= type;

    if(type != NAP)
        piececode |= side;

    return piececode;
}

Piece PieceHandlers::getType(uint8_t piececode)
{   
    return (Piece)(piececode & piece_type_mask);
}

PlaySide PieceHandlers::getColor(uint8_t piececode) {
    return(PlaySide)(piececode & playsidemask);
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

bool PieceHandlers::slotIsSafe(uint8_t slotcode, PlaySide mycolor) {
    if(mycolor == WHITE)
        return (slotcode & ATTACKED_BY_BLACK) == 0;

    return (slotcode & ATTACKED_BY_WHITE) == 0;
}

bool PieceHandlers::isProtectorOfTheKing(uint8_t piececode, PlaySide protected_color) {
    return (piececode & PROTECTOR_OF_THE_KING) != 0 && PieceHandlers::getColor(piececode) == protected_color;
}

bool PieceHandlers::isAttackerOfTheKing(uint8_t piececode, PlaySide attacked_color) {
    return (piececode & ATTACKER_OF_THE_KING) != 0 && PieceHandlers::getColor(piececode) != attacked_color;
}


std::vector<Move> PieceHandlers::calculateMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
   switch (PieceHandlers::getType(piececode))
    {
    case PAWN:
        return PieceHandlers::calculatePawnMoves(piececode, x, y, table, last_move, constraints);
        break;
    case KNIGHT:
        return PieceHandlers::calculateKnightMoves(piececode, x, y, table, last_move, constraints);
        break;
    case ROOK:
        return PieceHandlers::calculateRookMoves(piececode, x, y, table, last_move, constraints);
        break;
    case BISHOP:
        return PieceHandlers::calculateBishopMoves(piececode, x, y, table, last_move, constraints);
        break;
    case QUEEN:
        return PieceHandlers::calculateQueenMoves(piececode, x, y, table, last_move, constraints);
        break;
    case KING:
        return PieceHandlers::calculateKingMoves(piececode, x, y, table, last_move, constraints);
        break;
    default:
	    return std::vector<Move>();
        break;
    }

}

std::vector<Move> PieceHandlers::calculateProtectorOfTheKingMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t kingcode, int8_t kx, int8_t ky, uint8_t table[8][8], Move last_move) {
    // Generate vector of constraint moves
    
    std::vector<Move> constraints;
    int8_t init_x = x, init_y = y;
    int8_t dx = (x - kx < 0) ? -1 : (x - kx == 0) ? 0 : 1;
    int8_t dy = (y - ky < 0) ? -1 : (y - ky == 0) ? 0 : 1;

    // going away from the king untill we find the attacker
    do {
        x += dx;
        y += dy;

        constraints.push_back(*Move::moveTo(std::pair(0, 0), std::pair(x, y)));

    } while (PieceHandlers::getType(table[x][y]) == NAP);

    // going towards the king untill it is found
    dx = -dx;
    dy = -dy;

    x = init_x + dx;
    y = init_y + dy;

    while (PieceHandlers::getType(table[x][y]) == NAP) {
        constraints.push_back(*Move::moveTo(std::pair(0, 0), std::pair(x, y)));

        x += dx;
        y += dy;
    } 

    return calculateMoves(piececode, init_x, init_y, table, last_move, constraints);
}

std::vector<Move> PieceHandlers::calculateKingInCheckMoves(uint8_t kingcode, int8_t kx, int8_t ky, uint8_t table[8][8], Move last_move) {
    // get attacker piece ant its coords
    uint8_t attackercode = 0;
    int8_t attx, atty;

    for(int i = 0; i < 8; i ++) 
        for(int j = 0; j < 8; j++)
            if(PieceHandlers::isAttackerOfTheKing(table[i][j], PieceHandlers::getColor(kingcode))) {
                attackercode = table[i][j];
                attx = i;
                atty = j;
                break;
            }

    // generate constraints depending on the attacker type
    std::vector<Move> constraints, king_constraints;
    int8_t dx, dy, x, y;

    switch (PieceHandlers::getType(attackercode))
    {
    case PAWN: case KNIGHT:
        constraints.push_back(*Move::moveTo(std::pair(0, 0), std::pair(attx, atty)));
        break;
    case BISHOP: case ROOK: case QUEEN:
        dx = (attx - kx < 0) ? -1 : (attx - kx == 0) ? 0 : 1;
        dy = (atty - ky < 0) ? -1 : (atty - ky == 0) ? 0 : 1;

        x = kx; 
        y = ky;
        
        do {
            x += dx;
            y += dy;

            constraints.push_back(*Move::moveTo(std::pair(0, 0), std::pair(x, y)));
        } while (PieceHandlers::getType(table[x][y]) == NAP);

        // constraint for king, he is not allowed to move on the direction of
        // the attack
        king_constraints.push_back(*Move::moveTo(std::pair(0, 0), std::pair(kx - dx, ky - dy)));

        break;    
    case KING:
        break;
    default:
        break;
    }
    
    // calculate all possible moves for all pieces, considering constraints

    std::vector<Move> moves;
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            if(PieceHandlers::getType(table[i][j]) != NAP &&
                PieceHandlers::getColor(table[i][j]) == PieceHandlers::getColor(kingcode) &&
				!PieceHandlers::isProtectorOfTheKing(table[i][j], PieceHandlers::getColor(kingcode))) {
                std::vector<Move> helper = calculateMoves(table[i][j], i, j, table, last_move, table[i][j] == kingcode ? king_constraints : constraints);
                moves.insert(moves.end(), helper.begin(), helper.end());
            }
    return moves;
}



/*
    For these methods, you need to declare a vector of moves
    std::vector<Move> myvct
    and when you find a possible legal move, add it like this
    myvct.push_back(*Move::moveTo(src, dest))
    where src and dest are std::pair containing indices of the slot
*/

std::vector<Move> PieceHandlers::calculatePawnMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
    /*
        TODO:
        Generate all the moves this pawn can make on this table.
        This means it can either go up / down (depending on color)
        only if there is an empty slot available (dont forget it can
        jump 2 slots, if it hasn't moved yet), or it can attack on
        diagonal. Also, check enpassant rule, based on last_move.
        Create a vector with all the moves you found.
    */

    std::vector<Move> possible_moves;

    int8_t dx = PieceHandlers::getColor(piececode) == WHITE ? -1 : 1;

    // check simple moves, without capture
    // if the slot in front of the pown is empty
    if(PieceHandlers::getType(table[x + dx][y]) == NAP) {
        // reached the end of the table, promotion
        if(x + dx == 0 || x + dx == 7) {
            // add promotion move, but for now do it as it is
            possible_moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx, y)));
        } else {
            possible_moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx, y)));
        }

        // if the pawn hasn't moved yet, it can move two slots (if that slot is empty)
        if(PieceHandlers::getType(table[x + dx + dx][y]) == NAP &&
            ((PieceHandlers::getColor(piececode) == BLACK && x == 1) ||
            	(PieceHandlers::getColor(piececode) == WHITE && x == 6))) {
                possible_moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx + dx, y)));
            }
    }

    // capture
    // capture the enemy to the right or left, if it's there
    for(auto dy : {1, -1})
        if((y + dy < 8 && y + dy > -1) &&
            PieceHandlers::getType(table[x + dx][y + 1]) != NAP &&
            PieceHandlers::getColor(table[x + dx][y + 1]) != PieceHandlers::getColor(piececode))
                possible_moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx, y + 1)));

    // some check for en-passant 
    // TODO

    // filter through constraints
    if(!constraints.has_value())
        return possible_moves;

    std::vector<Move> moves;

    for(auto cmove : constraints.value())
        for(auto pmove : possible_moves) 
            if(pmove.destination_idx.value() == cmove.destination_idx.value()) {
                moves.push_back(pmove);
                continue;
            }

    return moves;
}

std::vector<Move> PieceHandlers::calculateKnightMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
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
   	
    std::vector<Move> moves;

    for(auto &[dx, dy] : PieceHandlers::knight_directions) {
		if(x + dx < 0 || x + dx > 7 || y + dy < 0 || y + dy > 7)
			continue;

		// move to an empty slot or capture an enemy piece(not the king)
		if(PieceHandlers::getType(table[x + dx][y + dy]) == NAP ||
			(PieceHandlers::getType(table[x + dx][y + dy]) != NAP &&
				PieceHandlers::getType(table[x + dx][y + dy]) != KING &&
				PieceHandlers::getColor(table[x + dx][y + dy]) != PieceHandlers::getColor(piececode))) {
			if(!constraints.has_value())
				moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx, y + dy)));
			else
				// check if this current move is in contstraints, if it is, it's legal
				for(auto move : constraints.value()) {
					//printf("%hhd, %hhd\n", move.destination_idx->first, move.destination_idx->second);
					if(move.destination_idx.value() == std::pair((int8_t)(x + dx), (int8_t)(y + dy))) {
                    	moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx, y + dy)));
						break;
					}
				}
			
		}
	}
    
	return moves;   
}

std::vector<Move> PieceHandlers::calculateRookMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
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

   	std::vector<Move> moves;

    for(auto &[dx, dy] : PieceHandlers::rook_directions) {
		int8_t currx = x + dx, curry = y + dy;
		while(1){
			if(!(currx > - 1 && currx < 8 && curry > -1 && curry < 8))
				break;
			
			// break if curr_piece is a King or is an allied piece
			if(PieceHandlers::getType(table[currx][curry]) == KING ||
				(PieceHandlers::getType(table[currx][curry]) != NAP &&
				PieceHandlers::getColor(table[currx][curry]) == PieceHandlers::getColor(piececode)))
				break;
			
			// otherwise add the move
			if(!constraints.has_value())
				moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(currx, curry)));
			else
				// check if this current move is in contstraints, if it is, it's legal
				for(auto move : constraints.value())
					if(move.destination_idx.value() == std::pair((int8_t)(currx), (int8_t)(curry))) {
                    	moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(currx, curry)));
						break;
					}

			// if the last move was capturing an enemy piece, break
			if(PieceHandlers::getType(table[currx][curry]) != NAP)
				break;

			currx += dx;
			curry += dy;
		}
	}

   	return moves;
}

std::vector<Move> PieceHandlers::calculateBishopMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
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

   	std::vector<Move> moves;

    for(auto &[dx, dy] : PieceHandlers::bishop_directions) {
		int8_t currx = x + dx, curry = y + dy;
		while(1){
			if(!(currx > - 1 && currx < 8 && curry > -1 && curry < 8))
				break;
			
			// break if curr_piece is a King or is an allied piece
			if(PieceHandlers::getType(table[currx][curry]) == KING ||
				(PieceHandlers::getType(table[currx][curry]) != NAP &&
				PieceHandlers::getColor(table[currx][curry]) == PieceHandlers::getColor(piececode)))
				break;
			
			// otherwise add the move
			if(!constraints.has_value())
				moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(currx, curry)));
			else
				// check if this current move is in contstraints, if it is, it's legal
				for(auto move : constraints.value())
					if(move.destination_idx.value() == std::pair((int8_t)(currx), (int8_t)(curry))) {
                    	moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(currx, curry)));
						break;
					}
			
			// if the last move was capturing an enemy piece, break
			if(PieceHandlers::getType(table[currx][curry]) != NAP)
				break;

			currx += dx;
			curry += dy;
		}
	}

   	return moves;
}

std::vector<Move> PieceHandlers::calculateQueenMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
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

   	// for(auto move : constraints.value())
	// 				printf("%hhd, %hhd\n", move.destination_idx->first, move.destination_idx->second);
	// 			printf("\n\n");
   	std::vector<Move> moves;

    for(auto &[dx, dy] : PieceHandlers::queen_directions) {
		int8_t currx = x + dx, curry = y + dy;
		while(1){
			if(!(currx > - 1 && currx < 8 && curry > -1 && curry < 8))
				break;
			
			// break if curr_piece is a King or is an allied piece
			if(PieceHandlers::getType(table[currx][curry]) == KING ||
				(PieceHandlers::getType(table[currx][curry]) != NAP &&
				PieceHandlers::getColor(table[currx][curry]) == PieceHandlers::getColor(piececode)))
				break;
			
			// otherwise add the move
			if(!constraints.has_value())
				moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(currx, curry)));
			else
				// check if this current move is in contstraints, if it is, it's legal
				for(auto move : constraints.value())
					if(move.destination_idx.value() == std::pair((int8_t)(currx), (int8_t)(curry))) {
						moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(currx, curry)));
						break;
					}

			// if the last move was capturing an enemy piece, break
			if(PieceHandlers::getType(table[currx][curry]) != NAP)
				break;

			currx += dx;
			curry += dy;
		}
	}

   	return moves;
}

std::vector<Move> PieceHandlers::calculateKingMoves(uint8_t piececode, int8_t x, int8_t y, uint8_t table[8][8], Move last_move, std::optional<std::vector<Move>> constraints) {
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
        to go is safe. (see PieceHandlers::slotIsSafe method)
    */

    std::vector<Move> moves;
    
    for(auto &[dx, dy] : PieceHandlers::king_directions) {
        if(x + dx < 0 || x + dx > 7 || y + dy < 0 || y + dy > 7)
            continue;

        // if the slot is not safe, we cannot move there
        if(!PieceHandlers::slotIsSafe(table[x + dx][y + dy], PieceHandlers::getColor(piececode)))
            continue;
        
        // if it's an allied piece, we cannot move there
        if(PieceHandlers::getType(table[x + dx][y + dy]) != NAP &&
            PieceHandlers::getColor(table[x + dx][y + dy]) == PieceHandlers::getColor(piececode))
            continue;
        
        // if it's enemy's king
        if(PieceHandlers::getType(table[x + dx][y + dy]) == KING)
            continue;


        // otherwise, if this move is not a constraint, we can move there, hopefully
        bool move_allowed = true;

        if(constraints.has_value())
            for(auto move : constraints.value())
                if(move.destination_idx.value() == std::pair((int8_t)(x + dx), (int8_t)(y + dy)))
                    move_allowed = false;

        if(move_allowed)
            moves.push_back(*Move::moveTo(std::pair(x, y), std::pair(x + dx, y + dy)));

    }

    /*
        Partially done, need a mechanism for castling
    */

    return moves;
}