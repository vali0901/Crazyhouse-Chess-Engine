#include "Table.h"

#include <bits/stdc++.h>

#include "Piece.h"
#include "PieceHandlers.h"

uint8_t Table::getPiece(int x, int y) {
	return table[x][y];
}

void Table::setPiece(int x, int y, uint8_t piece) {
	table[x][y] = piece;
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

std::vector<Move> Table::generateAllPossibleMoves() {
	/*
		TODO
	*/
}

Table::Table() {
	memset(table, 0, 64);

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

	table[x][y] = PieceHandlers::createPiece(KING, WHITE);

	do {
		x = std::rand() % 8;
		y = std::rand() % 8;
	} while(table[x][y] != NAP);

	table[x][y] = PieceHandlers::createPiece(KING, BLACK);

	update_states();
}

Table::Table(int custom, int youchoose) {
	memset(table, 0, 64);

	//table[4][4] = PieceHandlers::createPiece(QUEEN, WHITE);
	//table[1][4] = PieceHandlers::createPiece(KING, BLACK);
	//table[2][4] = PieceHandlers::createPiece(KNIGHT, BLACK);
	//table[3][4] = PieceHandlers::createPiece(PAWN, WHITE);

	//table[5][4] = PieceHandlers::createPiece(BISHOP, WHITE);
	//table[6][4] = PieceHandlers::createPiece(ROOK, BLACK);

	// table[5][5] = PieceHandlers::createPiece(ROOK, WHITE);
	// table[5][6] = PieceHandlers::createPiece(KNIGHT, BLACK);
	// table[5][7] = PieceHandlers::createPiece(KING, WHITE);

	table[3][5] = PieceHandlers::createPiece(QUEEN, WHITE);
	table[4][6] = PieceHandlers::createPiece(KNIGHT, BLACK);
	table[5][7] = PieceHandlers::createPiece(KING, BLACK);

	update_states();
}
