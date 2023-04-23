#include "Bot.h"
#include "Move.h"
#include "Table.h"
#include "Main.h"
#include "Piece.h"
#include "PieceHandlers.h"

#include <bits/stdc++.h>

const std::string Bot::BOT_NAME = "enpassant"; /* Edit this, escaped characters are forbidden */

Bot::Bot() { /* Initialize custom fields here */
  last_move_opposite_player = Move::resign();
  table = new Table();
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
    /* You might find it useful to also separately
     * record last move in another custom field */
    if (getEngineSide() != sideToMove) {
      last_move_opposite_player->source_str = move->source_str;
      last_move_opposite_player->destination_str = move->destination_str;
      last_move_opposite_player->replacement = move->replacement;
    }
    int x_src = -1, y_src = -1, x_dst = -1, y_dst = -1;
    // daca are sursa, ii iau coordonatele transformate din e4 in x = 4 y = 4
    //TODO: verificat ca indicii sa inceapa de la 1/0
    if (move->source_str.has_value()) {
      x_src = (int)(move->source_str.value()[0] - 'a');
      y_src = (int)move->source_str.value()[1];
    }

    if (move->destination_str.has_value()) {
      x_dst = (int)(move->destination_str.value()[0] - 'a');
      y_dst = (int)move->destination_str.value()[1];
    }

    // old declaration:
    // std::pair<Piece, PlaySide> moved_piece = std::pair(NAP, NONE);

    // new declaration
    uint8_t moved_piece = PieceHandlers::createPiece(NAP, NONE);

    // daca exista sursa, elementul de pe pozitia sursa e sters, si memorat in moved_piece
    if (x_src != -1 && y_src != -1) {
      moved_piece = table->getPiece(x_src, y_src);
      table->setPiece(x_src, y_src, NAP);
    }
    // daca are destinatie, elementul e pus pe destinatie, daca e un atac ar trebui
    // testat daca e o piesa inamica, in cazul asta o adaug la piesele mele disponibile
    if (x_dst != -1 && y_dst != -1) {
      table->setPiece(x_dst, y_dst, moved_piece);
    }
    // in caz de promovare/dropIn, e replaced la destinatie cu piesa replacement
    if (move->getReplacement().has_value()) {
      table->setPiece(x_dst, y_dst, PieceHandlers::createPiece(move->getReplacement().value(), sideToMove));
    }

}

Move* Bot::calculateNextMove() {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */
  return Move::resign();
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }
