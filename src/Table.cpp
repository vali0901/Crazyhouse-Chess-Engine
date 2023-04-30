#include "Table.h"

#include <bits/stdc++.h>

#include "Piece.h"
#include "PieceHandlers.h"
#include "PlaySide.h"

uint8_t Table::getPiece(int x, int y) {
	return table[x][y];
}

uint8_t Table::getPiece(std::pair<int, int> coords) {
	return table[coords.first][coords.second];
}

void Table::setPiece(int x, int y, uint8_t piece) {
	table[x][y] = piece;
}

void Table::setPiece(std::pair<int, int> coords, uint8_t piece) {
	table[coords.first][coords.second] = piece;
}

bool Table::kingIsInCheck(PlaySide color) {
	if(color == WHITE)
		return (table[wKx][wKy] & ATTACKED_BY_BLACK) != 0;
	
	return (table[bKx][bKy] & ATTACKED_BY_WHITE) != 0;
}

void Table::pieceHasMoved(uint8_t mask) {
	rocinfo &= mask;
}

void Table::addToCaptured(Table &table, Piece piece, PlaySide playside) {
	if (playside == BLACK)
		capturedByBlack.push_back(piece);
	else
		capturedByWhite.push_back(piece);
}

void Table::update_states() {
	// iterate trough all pieces and mark their states
	
	// first, reset their states, and then update them
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			table[i][j] &= ~piece_state_mask;

	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++) {
			PlaySide piece_color = PieceHandlers::getColor(table[i][j]);
			int8_t pawn_direction;
			switch (PieceHandlers::getType(table[i][j]))
			{
			case PAWN:
				// setting its direction, up or down depending on its color
				pawn_direction = piece_color == WHITE ? -1 : 1;

				// mark slots on diagnoal as attacked, also mark this pawn as the Attacker of the king if the case
				if(j + 1 < 8) {
					PieceHandlers::setAttackedBy(table[i + pawn_direction][j + 1], piece_color);
					table[i][j] |= (PieceHandlers::getType(table[i + pawn_direction][j + 1]) == KING &&
									PieceHandlers::getColor(table[i + pawn_direction][j + 1]) != piece_color) ? ATTACKER_OF_THE_KING : 0;
				}

				if(j - 1 > -1) {
					PieceHandlers::setAttackedBy(table[i + pawn_direction][j - 1], piece_color);
					table[i][j] |= (PieceHandlers::getType(table[i + pawn_direction][j - 1]) == KING &&
									PieceHandlers::getColor(table[i + pawn_direction][j - 1]) != piece_color) ? ATTACKER_OF_THE_KING : 0;
				}
				break;

			case KNIGHT:	
				// iterate through directions and set the attacked slots, also set the attribute Attacker of the king if needed
				for(auto & [x, y] : PieceHandlers::knight_directions)
					if(i + x < 0 || i + x > 7 || j + y < 0 || j + y > 7)
						continue;
					else {
						PieceHandlers::setAttackedBy(table[i + x][j + y], piece_color);
						table[i][j] |= (PieceHandlers::getType(table[i + x][j + y]) == KING &&
										PieceHandlers::getColor(table[i + x][j + y]) != piece_color) ? ATTACKER_OF_THE_KING : 0;
					}

				break;

			case ROOK:
				// iterate through table in each direction, and stop setting attacked pieces when hitting other piece
				for(auto & [x, y] : PieceHandlers::rook_directions) {
					int8_t dx = i + x, dy = j + y;
					while((dx > -1 && dx < 8) && (dy > -1 && dy < 8)) {
						PieceHandlers::setAttackedBy(table[dx][dy], piece_color);
						// empty slot, we keep going
						if(PieceHandlers::getType(table[dx][dy]) == NAP) {
							dx += x;
							dy += y;
							continue;
						}
						// we found a piece
						// if this attacked piece has the same color, we stop
						if(PieceHandlers::getColor(table[dx][dy]) == piece_color)
							break;
						// if this attacking piece is the enemy's king, set current piece as the king Attacker
						if(PieceHandlers::getType(table[dx][dy]) == KING) {
							table[i][j] |= ATTACKER_OF_THE_KING;
							break;
						}
						// check to see if this attacked piece is a Protector of the king
						int8_t tmp_x = dx, tmp_y = dy;
						dx += x;
						dy += y;
						while((dx > -1 && dx < 8) && (dy > -1 && dy < 8)) {
							if(PieceHandlers::getType(table[dx][dy]) != NAP) {
								if(PieceHandlers::getType(table[dx][dy]) == KING &&
									PieceHandlers::getColor(table[dx][dy]) != piece_color)
									table[tmp_x][tmp_y] |= PROTECTOR_OF_THE_KING;
								break;
							}
							dx += x;
							dy += y;
						}
						break;
					}
				}
				break;

			case BISHOP:
				// iterate through table in each direction, and stop setting attacked pieces when hitting other piece
				for(auto & [x, y] : PieceHandlers::bishop_directions) {
					int8_t dx = i + x, dy = j + y;
					while((dx > -1 && dx < 8) && (dy > -1 && dy < 8)) {
						PieceHandlers::setAttackedBy(table[dx][dy], piece_color);
						// empty slot, we keep going
						if(PieceHandlers::getType(table[dx][dy]) == NAP) {
							dx += x;
							dy += y;
							continue;
						}
						// we found a piece
						// if this attacked piece has the same color, we stop
						if(PieceHandlers::getColor(table[dx][dy]) == piece_color)
								break;
						// if this attacking piece is the enemy's king, set current piece as the king Attacker
						if(PieceHandlers::getType(table[dx][dy]) == KING) {
							table[i][j] |= ATTACKER_OF_THE_KING;
							break;
						}
						// check to see if this attacked piece is a Protector of the king
						int8_t tmp_x = dx, tmp_y = dy;
						dx += x;
						dy += y;
						while((dx > -1 && dx < 8) && (dy > -1 && dy < 8)) {
							if(PieceHandlers::getType(table[dx][dy]) != NAP) {
								if(PieceHandlers::getType(table[dx][dy]) == KING &&
									PieceHandlers::getColor(table[dx][dy]) != piece_color)
									table[tmp_x][tmp_y] |= PROTECTOR_OF_THE_KING;
								break;
							}
							dx += x;
							dy += y;
						}
						break;
					}
				}
				break;

			case QUEEN:
				// iterate through table in each direction, and stop setting attacked pieces when hitting other piece
				for(auto & [x, y] : PieceHandlers::queen_directions) {
					int8_t dx = i + x, dy = j + y;
					while((dx > -1 && dx < 8) && (dy > -1 && dy < 8)) {
						PieceHandlers::setAttackedBy(table[dx][dy], piece_color);
						// empty slot, we keep going
						if(PieceHandlers::getType(table[dx][dy]) == NAP) {
							dx += x;
							dy += y;
							continue;
						}
						// we found a piece
						// if this attacked piece has the same color, we stop
						if(PieceHandlers::getColor(table[dx][dy]) == piece_color)
								break;
						// if this attacking piece is the enemy's king, set current piece as the king Attacker
						if(PieceHandlers::getType(table[dx][dy]) == KING) {
							table[i][j] |= ATTACKER_OF_THE_KING;
							break;
						}
						// check to see if this attacked piece is a Protector of the king
						int8_t tmp_x = dx, tmp_y = dy;
						dx += x;
						dy += y;
						while((dx > -1 && dx < 8) && (dy > -1 && dy < 8)) {
							if(PieceHandlers::getType(table[dx][dy]) != NAP) {
								if(PieceHandlers::getType(table[dx][dy]) == KING &&
									PieceHandlers::getColor(table[dx][dy]) != piece_color)
									table[tmp_x][tmp_y] |= PROTECTOR_OF_THE_KING;
								break;
							}
							dx += x;
							dy += y;
						}
						break;
					}
				}
				break;

			case KING:
				for(auto & [x, y] : PieceHandlers::king_directions) 
					if(i + x < 0 || i + x > 7 || j + y < 0 || j + y > 7)
						continue;
					else
						PieceHandlers::setAttackedBy(table[i + x][j + y], piece_color);

				break;
			default:
				break;
			}
		}
}

void Table::generateAllPossibleMoves(PlaySide turn, Move last_move, std::vector<Move> &moves) {
	if(kingIsInCheck(turn)) {
		std::vector<Move> helper;
		if(turn == WHITE)
			helper = PieceHandlers::calculateKingInCheckMoves(table[wKx][wKy], wKx, wKy, table, last_move, capturedByWhite);	
		else 
			helper = PieceHandlers::calculateKingInCheckMoves(table[bKx][bKy], bKx, bKy, table, last_move, capturedByBlack);

		moves.insert(moves.end(), helper.begin(), helper.end());
		return;
	}

	for(int i = 0; i < 8; i ++)
		for(int j = 0; j < 8; j++) {
			if(PieceHandlers::isProtectorOfTheKing(table[i][j], turn)) {
				std::vector<Move> helper;
				if(turn == WHITE)
					helper = PieceHandlers::calculateProtectorOfTheKingMoves(table[i][j], i, j, table[wKx][wKy], wKx, wKy, table, last_move);
				else
					helper = PieceHandlers::calculateProtectorOfTheKingMoves(table[i][j], i, j, table[bKx][bKy], bKx, bKy, table, last_move);

				moves.insert(moves.end(), helper.begin(), helper.end());
			} else if(PieceHandlers::getType(table[i][j]) != NAP && PieceHandlers::getColor(table[i][j]) == turn) {
				std::vector<Move> helper = PieceHandlers::calculateMoves(table[i][j], i, j, table, last_move, rocinfo, {}); 
				moves.insert(moves.end(), helper.begin(), helper.end());
			}
		}

	// generate drop-in moves
	std::vector<Piece> &capturedPieces = (turn == WHITE) ? capturedByWhite : capturedByBlack;

	// go through eachi slot on the table, if it is empty generate all possible drop-in moves
	for(int8_t i = 0; i < 8; i++) {
		for(int8_t j = 0; j < 8; j++) {
			if(PieceHandlers::getType(table[i][j]) != NAP)
				continue;

			for(auto piece : capturedPieces) {
				if(piece == PAWN && (i == 0 || i == 7))
					continue;
				
				moves.push_back(*Move::dropIn(std::pair(i, j), piece));
			}
		}
	}
}

Table::Table() {
	memset(table, 0, 64);
	last_move.source_idx = {};
	last_move.destination_idx = {};

	last_move.source_str = "";
	last_move.destination_str = "";

	rocinfo = 0b01110111;

	for(int j = 0; j < 8; j++) {
		table[1][j] = PieceHandlers::createPiece(PAWN, BLACK);
		table[6][j] = PieceHandlers::createPiece(PAWN, WHITE);
	}

	table[0][0] = table[0][7] = PieceHandlers::createPiece(ROOK, BLACK);
	table[7][0] = table[7][7] = PieceHandlers::createPiece(ROOK, WHITE);

	table[0][1] = table[0][6] = PieceHandlers::createPiece(KNIGHT, BLACK);
	table[7][1] = table[7][6] = PieceHandlers::createPiece(KNIGHT, WHITE);

	table[0][2] = table[0][5] = PieceHandlers::createPiece(BISHOP, BLACK);
	table[7][2] = table[7][5] = PieceHandlers::createPiece(BISHOP, WHITE);

	table[0][3] = PieceHandlers::createPiece(QUEEN, BLACK);
	table[7][3] = PieceHandlers::createPiece(QUEEN, WHITE);

	table[0][4] = PieceHandlers::createPiece(KING, BLACK);
	table[7][4] = PieceHandlers::createPiece(KING, WHITE);

	bKx = 0;
	bKy = 4;
	wKx = 7;
	wKy = 4;

	update_states();
}

Table::Table(int random) {
	memset(table, 0, 64);

	int x;
	int y;

	std::srand(time(NULL));

	for(int i = 0; i < 8; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(PAWN, BLACK);
	}

	for(int i = 0; i < 8; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(PAWN, WHITE);
	}

	for(int i = 0; i < 2; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(ROOK, WHITE);
	}

	for(int i = 0; i < 2; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(ROOK, BLACK);
	}

	for(int i = 0; i < 2; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(KNIGHT, BLACK);
	}

	for(int i = 0; i < 2; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(KNIGHT, WHITE);
	}

	for(int i = 0; i < 2; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(BISHOP, BLACK);
	}

	for(int i = 0; i < 2; i++) {
		do {
			x = std::rand() % 8;
			y = std::rand() % 8;
		} while(table[x][y] != NAP);

		table[x][y] = PieceHandlers::createPiece(BISHOP, WHITE);
	}

	do {
		x = std::rand() % 8;
		y = std::rand() % 8;
	} while(table[x][y] != NAP);

	table[x][y] = PieceHandlers::createPiece(QUEEN, WHITE);

	do {
		x = std::rand() % 8;
		y = std::rand() % 8;
	} while(table[x][y] != NAP);

	table[x][y] = PieceHandlers::createPiece(QUEEN, BLACK);

	do {
		x = std::rand() % 8;
		y = std::rand() % 8;
	} while(table[x][y] != NAP);

	table[x][y] = PieceHandlers::createPiece(KING, WHITE);\
	wKx = x;
	wKy = y;

	do {
		x = std::rand() % 8;
		y = std::rand() % 8;
	} while(table[x][y] != NAP);

    table[x][y] = PieceHandlers::createPiece(KING, BLACK);
	bKx = x;
	bKy = y;

	update_states();
}

Table::Table(int custom, int youchoose) {
	memset(table, 0, 64);

	//table[4][4] = PieceHandlers::createPiece(QUEEN, WHITE);
	table[1][4] = PieceHandlers::createPiece(KING, WHITE);
	wKx = 1;
	wKy = 4;
	table[3][5] = PieceHandlers::createPiece(KNIGHT, BLACK);
	table[0][2] = PieceHandlers::createPiece(BISHOP, WHITE);
	// table[7][0] = PieceHandlers::createPiece(ROOK, WHITE);
	// table[7][7] = PieceHandlers::createPiece(ROOK, WHITE);
	//rocinfo = 0b01110000;
	//table[2][2] = PieceHandlers::createPiece(KNIGHT, WHITE);
	//table[5][0] = PieceHandlers::createPiece(QUEEN, WHITE);
	//table[3][3] = PieceHandlers::createPiece(PAWN, BLACK);
	//table[3][4] = PieceHandlers::createPiece(PAWN, WHITE);

	//table[5][5] = PieceHandlers::createPiece(BISHOP, BLACK);
	//table[7][0] = PieceHandlers::createPiece(ROOK, BLACK);

	// table[5][5] = PieceHandlers::createPiece(ROOK, WHITE);
	// table[5][6] = PieceHandlers::createPiece(KNIGHT, BLACK);
	// table[5][7] = PieceHandlers::createPiece(KING, WHITE);

	// table[3][5] = PieceHandlers::createPiece(QUEEN, WHITE);
	// table[4][6] = PieceHandlers::createPiece(KNIGHT, BLACK);
	// table[5][7] = PieceHandlers::createPiece(KING, BLACK);

	update_states();
}
