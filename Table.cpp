#include "Table.h"

#include <bits/stdc++.h>

#include "Piece.h"

Piece Table::getPiece(int x, int y) {
    return table[x][y];
}

void Table::setPiece(int x, int y, Piece new_piece) {
    table[x][y] = new_piece;
}