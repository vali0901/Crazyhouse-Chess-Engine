#include "Table.h"

#include <bits/stdc++.h>

#include "Piece.h"

std::pair<Piece, PlaySide> Table::getPiece(int x, int y) {
    return table[x][y];
}

void Table::setPiece(int x, int y, Piece new_piece, PlaySide playSide) {
    table[x][y].first = new_piece;
    table[x][y].second = playSide;
}