#include "Bot.h"
#include "Move.h"
#include "Table.h"
#include "Main.h"
#include "Piece.h"
#include "PieceHandlers.h"

#include <bits/stdc++.h>

#include <iostream>

extern std::ofstream *output;
const std::string Bot::BOT_NAME = "enpassant"; /* Edit this, escaped characters are forbidden */
void printTable(uint8_t table[8][8]);
void printTableBits(uint8_t table[8][8]);

extern FILE* f_out;
void printPiecesAvailable(Table &table);

Bot::Bot() { /* Initialize custom fields here */
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
  Move::convertStrToIdx(*move);
  
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
    if (PieceHandlers::getType(table.getPiece(move->destination_idx.value())) != NAP)
      table.addToCaptured(table, PieceHandlers::getType(table.getPiece(move->destination_idx.value())), sideToMove);

    table.setPiece(move->destination_idx.value(), moved_piece);
  }
  // drop in remove from the array of pieces available
  if (!move->source_idx.has_value() && move->destination_idx.has_value() && move->replacement.has_value()) {
    *output << "is drop in\n";
    if (sideToMove == BLACK) {
      for (unsigned long i = 0; i < table.capturedByBlack.size(); i++) {
        if (table.capturedByBlack[i] == move->replacement) {
          table.capturedByBlack[i] = table.capturedByBlack.back();
          table.capturedByBlack.pop_back();
          break;
        }
      }
    } else {
      for (unsigned long i = 0; i < table.capturedByWhite.size(); i++) {
        if (table.capturedByWhite[i] == move->replacement) {
          table.capturedByWhite[i] = table.capturedByWhite.back();
          table.capturedByWhite.pop_back();
          break;
        }
      }
    }
  }

  // in case of promotion/dropIn, the destination is replaced with "replacement"
  if (move->getReplacement().has_value())
    table.setPiece(move->destination_idx.value(), PieceHandlers::createPiece(move->getReplacement().value(), sideToMove));
    
  table.update_states();

  table.last_move = *move;
  *output << (unsigned int)table.rocinfo << "\n";
  printTable(table.table);
  fprintf(f_out, "\n\n");

  // printTableBits(table.table);
  // fprintf(f_out, "\n\n");
  
  printPiecesAvailable(table);


}

Move Bot::calculateNextMove(PlaySide sideToMove) {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */

  std::vector<Move> allMoves;
  table.generateAllPossibleMoves(sideToMove, table.last_move, allMoves);

  int idx = -1;
  for (unsigned long i = 0; i < allMoves.size(); i++)
  {
    
    // if (allMoves[i].source_idx.has_value() && allMoves[i].destination_idx.has_value() && 
    //   PieceHandlers::getType(table.getPiece((allMoves[i].source_idx.value()))) == KING &&
    //   abs(allMoves[i].source_idx.value().second - allMoves[i].destination_idx.value().second) == 2)
    //   {
    //     idx = i;
    //     break;
    //   }
    if (!allMoves[i].source_idx.has_value() && allMoves[i].destination_idx.has_value() && allMoves[i].replacement.has_value()) {
      idx = i;
      break;
    }
  }

  if (idx == -1) {
    std::srand(std::time(NULL));
    int index = std::rand() % allMoves.size();
    Move::convertIdxToStr(allMoves[index]);
    recordMove(&allMoves[index], sideToMove);

    return allMoves[index];  
  } else {
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

        fprintf(f_out,"en passant\n");
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

void printTable(uint8_t table[8][8]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            switch (PieceHandlers::getType(table[i][j]) | PieceHandlers::getColor(table[i][j]))
            {
            case PAWN | WHITE:
                fprintf(f_out, "wP\t");
                break;
            case KNIGHT | WHITE:
                fprintf(f_out, "wN\t");
                break;
            case BISHOP | WHITE:
                fprintf(f_out, "wB\t");
                break;
            case KING | WHITE:
                fprintf(f_out, "wK\t");
                break;
            case QUEEN | WHITE:
                fprintf(f_out, "wQ\t");
                break;
            case ROOK | WHITE:
                fprintf(f_out,"wR\t");
                break;
            case PAWN | BLACK:
                fprintf(f_out,"bP\t");
                break;
            case KNIGHT | BLACK:
                fprintf(f_out,"bN\t");
                break;
            case BISHOP | BLACK:
                fprintf(f_out,"bB\t");
                break;
            case KING | BLACK:
                fprintf(f_out, "bK\t");
                break;
            case QUEEN | BLACK:
                fprintf(f_out, "bQ\t");
                break;
            case ROOK | BLACK:
                fprintf(f_out,"bR\t");
                break;
            default:
                fprintf(f_out, "||\t");
                break;
            }
        }
        fprintf(f_out, "\n");
    }
}
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c %c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

void printTableBits(uint8_t table[8][8]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            fprintf(f_out, BYTE_TO_BINARY_PATTERN "\t", BYTE_TO_BINARY(table[i][j]));
        }
        fprintf(f_out,"\n");
    }
}

void printPiecesAvailable(Table &table) {

  fprintf(f_out, "pieces available for WHITE\n");
  for (auto piece : table.capturedByWhite) {
    switch (piece) {
      case PAWN:
        fprintf(f_out, "PAWN ");
        break;
      case ROOK:
        fprintf(f_out, "ROOK ");
        break;
      case KNIGHT:
        fprintf(f_out, "KNIGHT ");
        break;
      case BISHOP:
        fprintf(f_out, "BISHOP ");
        break;
      case QUEEN:
        fprintf(f_out, "QUEEN ");
        break;
      default:
        fprintf(f_out, "NAP?weird ");
        break;
    }
  }  
  fprintf(f_out, "\n\n");

    fprintf(f_out, "pieces available for BLACK\n");
  for (auto piece : table.capturedByBlack) {
    switch (piece) {
      case PAWN:
        fprintf(f_out, "PAWN ");
        break;
      case ROOK:
        fprintf(f_out, "ROOK ");
        break;
      case KNIGHT:
        fprintf(f_out, "KNIGHT ");
        break;
      case BISHOP:
        fprintf(f_out, "BISHOP ");
        break;
      case QUEEN:
        fprintf(f_out, "QUEEN ");
        break;
      default:
        fprintf(f_out, "NAP?weird ");
        break;
    }
  }  
  fprintf(f_out, "\n\n");
}