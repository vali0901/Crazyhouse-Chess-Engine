#include "Bot.h"
#include "Move.h"
#include "Table.h"
#include "Main.h"
#include "Piece.h"
#include "PieceHandlers.h"

#include <bits/stdc++.h>

const std::string Bot::BOT_NAME = "en-passant";

Bot::Bot() {
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
  Move::convertStrToIdx(*move);

  checkPromotedPawns(move, sideToMove, table);

  checkImportantPiecesThatMoved(move, table);

  checkCastling(table, move, sideToMove);
  
  checkEnPassant(table, move, sideToMove);

  uint8_t moved_piece = PieceHandlers::createPiece(NAP, NONE);

  // if move source exists(normal/promotion), the piece is removed and saved in moved_piece
  if (move->source_idx.has_value())
  {
    moved_piece = table.getPiece(move->source_idx.value());
    table.setPiece(move->source_idx.value(), PieceHandlers::createPiece(NAP, NONE));
  }

  if (move->destination_idx.has_value())
  {
    // check if it captured something
    if (PieceHandlers::getType(table.getPiece(move->destination_idx.value())) != NAP) {
      // piece = the piece that the player can add to their captured pieces
      // if the piece was a promoted pawn, the player that captured it received a pawn
      Piece piece = PieceHandlers::getType(table.getPiece(move->destination_idx.value()));

      std::vector<std::pair<int8_t, int8_t>> &promotedPieces = (sideToMove == BLACK) ? table.promotedPawnsWhite : table.promotedPawnsBlack;
      for (unsigned long i = 0; i < promotedPieces.size(); i++) {
        if (promotedPieces[i] == move->destination_idx.value()) {
          piece = PAWN;

          promotedPieces[i] = promotedPieces.back();
          promotedPieces.pop_back();

          break;
        }
      } 
      table.addToCaptured(table, piece, sideToMove);
    }

    table.setPiece(move->destination_idx.value(), moved_piece);
  }
  // drop in, remove from the array of pieces available
  if (!move->source_idx.has_value() && move->destination_idx.has_value() && move->replacement.has_value()) {

    std::vector<Piece> &capturedPieces = (sideToMove == BLACK) ? table.capturedByBlack : table.capturedByWhite;   
    
    for (unsigned long i = 0; i < capturedPieces.size(); i++) {
      if (capturedPieces[i] == move->replacement) {
        capturedPieces[i] = capturedPieces.back();
        capturedPieces.pop_back();
        break;
      }
    }
  }

  // in case of promotion/dropIn, the destination is replaced with "replacement"
  if (move->getReplacement().has_value())
    table.setPiece(move->destination_idx.value(), PieceHandlers::createPiece(move->getReplacement().value(), sideToMove));
    
  table.update_states();
  table.last_move = *move;
}

Move Bot::calculateNextMove(PlaySide sideToMove) {
  std::vector<Move> allMoves;
  table.generateAllPossibleMoves(sideToMove, table.last_move, allMoves);

  // find if there is any castle move
  int idx = -1;
  for (unsigned long i = 0; i < allMoves.size(); i++)
  {
    if (allMoves[i].source_idx.has_value() && allMoves[i].destination_idx.has_value() && 
      PieceHandlers::getType(table.getPiece((allMoves[i].source_idx.value()))) == KING &&
      abs(allMoves[i].source_idx.value().second - allMoves[i].destination_idx.value().second) == 2)
      {
        idx = i;
        break;
      }
  }
  if (idx == -1) {
    // pick a random move from the moves available
    std::srand(std::time(NULL));
    int index = std::rand() % allMoves.size();
    Move::convertIdxToStr(allMoves[index]);
    recordMove(&allMoves[index], sideToMove);

    return allMoves[index];  
  } else {
    // if there is a castle move available, use that one
    Move::convertIdxToStr(allMoves[idx]);
    recordMove(&allMoves[idx], sideToMove);

    return allMoves[idx];
  }
  
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }

bool checkEnPassant(Table &table, Move *move, PlaySide &sideToMove)
{
  // checks if the last move forwarded the piece 2 positions on the same column, if that piece is a pawn
  // and if the current move is a normal move(has src + dst) and it moves a pawn as well
  if (table.last_move.destination_idx.has_value() &&
      table.last_move.source_idx.has_value() &&
      abs(table.last_move.destination_idx.value().first - table.last_move.source_idx.value().first) == 2 &&
      PieceHandlers::getType(table.getPiece(table.last_move.destination_idx.value())) == PAWN &&
      move->source_idx.has_value() &&
      move->destination_idx.has_value() &&
      PieceHandlers::getType(table.getPiece(move->source_idx.value())) == PAWN)
  {
    
    int x_src = move->source_idx.value().first;
    int y_src = move->source_idx.value().second;

    int x_dst = move->destination_idx.value().first;
    int y_dst = move->destination_idx.value().second;

    int x_last = table.last_move.destination_idx.value().first;
    int y_last = table.last_move.destination_idx.value().second;

    // if before the move is made, the two pawns are not on the same line, at the distance
    // of 1 column apart, there's no way it's an en-passant
    if (x_src == x_last && abs(y_src - y_last) == 1 && abs(y_dst - y_src) == 1)
    {
      // offset = the row behind last_move piece, despite it's colour
      int offset = 0;
      
      if (sideToMove == BLACK)
        offset = +1;
      else
        offset = -1;

      if (x_dst == x_last + offset && PieceHandlers::getType(table.getPiece(x_dst, y_dst)) == NAP)
      {
        table.addToCaptured(table, PieceHandlers::getType(table.getPiece(table.last_move.destination_idx.value())), sideToMove);

        table.setPiece(table.last_move.destination_idx.value(), PieceHandlers::createPiece(NAP, NONE));
        return true;
      }
    }
  }
  return false;
}

bool checkCastling(Table &table, Move *move, PlaySide &sideToMove)
{
  // check if castle: the only way a king can move 2 squares is by castling
  if (move->source_idx.has_value() && move->destination_idx.has_value() && 
      PieceHandlers::getType(table.getPiece((move->source_idx.value()))) == KING &&
      abs(move->source_idx.value().second - move->destination_idx.value().second) == 2)
  {
    int kx_src = move->source_idx.value().first;

    int ky_src = move->source_idx.value().second;
    int ky_dst = move->destination_idx.value().second;
    
    uint8_t moved_rook;
    table.pieceHasMoved(sideToMove == WHITE ? 0b00001111 : 0b11110000);

    if (ky_dst > ky_src) // the right rook should be moved
    {
      moved_rook = table.getPiece(kx_src, 7);
      table.setPiece(kx_src, 7, PieceHandlers::createPiece(NAP, NONE));
      table.setPiece(kx_src, ky_src + 1, moved_rook);
    } else { // the left rook is moved

      moved_rook = table.getPiece(kx_src, 0);
      table.setPiece(kx_src, 0, PieceHandlers::createPiece(NAP, NONE));
      table.setPiece(kx_src, ky_src - 1, moved_rook);
    }
    return true;
  }
  return false;
}

void checkPromotedPawns(Move *move, PlaySide sideToMove, Table &table) {
  // fprintf(f_out, "in checkPromotedPawns\n");
  if (move->source_idx.has_value() && move->destination_idx.has_value() && move->replacement.has_value()) {
    if (sideToMove == BLACK)
      table.promotedPawnsBlack.push_back(move->destination_idx.value());
    else
      table.promotedPawnsWhite.push_back(move->destination_idx.value());
  }
  if (move->source_idx.has_value() && move->destination_idx.has_value() && !move->replacement.has_value()) {
    std::vector<std::pair<int8_t, int8_t>> &promotedPawns = sideToMove == BLACK ? table.promotedPawnsBlack : table.promotedPawnsWhite;
    for (auto& pos : promotedPawns) {
      if (pos == move->source_idx.value()) {
        pos = move->destination_idx.value();
      }
    }
  }
}

void checkImportantPiecesThatMoved(Move *move, Table &table) {
   if (!move->isDropIn()) {
    if(PieceHandlers::getType(table.getPiece(move->source_idx.value())) == KING) {
    if(PieceHandlers::getColor(table.getPiece(move->source_idx.value())) == WHITE) {
      table.wKx = move->destination_idx->first;
      table.wKy = move->destination_idx->second;
      table.pieceHasMoved(0b11011111);
    } else {
      table.bKx = move->destination_idx->first;
      table.bKy = move->destination_idx->second;
      table.pieceHasMoved(0b11111101);
    }
  } else if (PieceHandlers::getType(table.getPiece(move->source_idx.value())) == ROOK) {
    // left rook has moved
    if(move->source_idx.value() == std::pair((int8_t)0, (int8_t)0) || move->source_idx.value() == std::pair((int8_t)7, (int8_t)0))
      table.pieceHasMoved(PieceHandlers::getColor(table.getPiece((move->source_idx.value()))) == WHITE ? 0b10111111 : 0b11111011 );
    // right rook has moved
    if(move->source_idx.value() == std::pair((int8_t)0, (int8_t)7) || move->source_idx.value() == std::pair((int8_t)7, (int8_t)7))
      table.pieceHasMoved(PieceHandlers::getColor(table.getPiece((move->source_idx.value()))) == WHITE ? 0b11101111 : 0b11111110 );
    }
  }
}
