#include "Piece.h"
#include "PieceHandlers.h"
#include "Table.h"
#include "Move.h"

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

void printTable(uint8_t table[8][8]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            switch (PieceHandlers::getType(table[i][j]) | PieceHandlers::getColor(table[i][j]))
            {
            case PAWN | WHITE:
                printf("wP\t");
                break;
            case KNIGHT | WHITE:
                printf("wN\t");
                break;
            case BISHOP | WHITE:
                printf("wB\t");
                break;
            case KING | WHITE:
                printf("wK\t");
                break;
            case QUEEN | WHITE:
                printf("wQ\t");
                break;
            case ROOK | WHITE:
                printf("wR\t");
                break;
            case PAWN | BLACK:
                printf("bP\t");
                break;
            case KNIGHT | BLACK:
                printf("bN\t");
                break;
            case BISHOP | BLACK:
                printf("bB\t");
                break;
            case KING | BLACK:
                printf("bK\t");
                break;
            case QUEEN | BLACK:
                printf("bQ\t");
                break;
            case ROOK | BLACK:
                printf("bR\t");
                break;
            default:
                printf("||\t");
                break;
            }
        }
        printf("\n");
    }
}

void printTableBits(uint8_t table[8][8]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            printf(BYTE_TO_BINARY_PATTERN "\t", BYTE_TO_BINARY(table[i][j]));
        }
        printf("\n");
    }
}

// Testing getColor and getType functionalities for each piece and color
void testPieceHandler() {
    uint8_t pawnb = PieceHandlers::createPiece(PAWN, BLACK);
    uint8_t pawnw = PieceHandlers::createPiece(PAWN, WHITE);
    uint8_t rookb = PieceHandlers::createPiece(ROOK, BLACK);
    uint8_t rookw = PieceHandlers::createPiece(ROOK, WHITE);
    uint8_t bishopb = PieceHandlers::createPiece(BISHOP, BLACK);
    uint8_t bishopw = PieceHandlers::createPiece(BISHOP, WHITE);
    uint8_t knightb = PieceHandlers::createPiece(KNIGHT, BLACK);
    uint8_t knightw = PieceHandlers::createPiece(KNIGHT, WHITE);
    uint8_t queenb = PieceHandlers::createPiece(QUEEN, BLACK);
    uint8_t queenw = PieceHandlers::createPiece(QUEEN, WHITE);
    uint8_t kingb = PieceHandlers::createPiece(KING, BLACK);
    uint8_t kingw = PieceHandlers::createPiece(KING, WHITE);
    uint8_t empty = PieceHandlers::createPiece(NAP, NONE);

    // printf("Pawnb " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(pawnb));
    // printf("Pawnw " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(pawnw));

    if(PieceHandlers::getType(pawnb) == PAWN
        && PieceHandlers::getType(pawnw) == PAWN) 
        printf("Pawn test passed\n");

    // printf("Rookb " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rookb));
    // printf("Rookw " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rookw));

    if(PieceHandlers::getType(rookb) == ROOK
        && PieceHandlers::getType(rookw) == ROOK) 
        printf("Rook test passed\n");

    // printf("Bishopb " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(bishopb));
    // printf("Bishopw " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(bishopw));
    
    if(PieceHandlers::getType(bishopb) == BISHOP
        && PieceHandlers::getType(bishopw) == BISHOP) 
        printf("Bishop test passed\n");
    
    // printf("Kinghtb " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(knightb));
    // printf("Knightw " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(knightw));

    if(PieceHandlers::getType(knightb) == KNIGHT
        && PieceHandlers::getType(knightw) == KNIGHT) 
        printf("Kinght test passed\n");
    
    // printf("Queenb " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(queenb));
    // printf("Queenw " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(queenw));

    if(PieceHandlers::getType(queenb) == QUEEN
        && PieceHandlers::getType(queenw) == QUEEN) 
        printf("Queen test passed\n");
    
    // printf("Kingb " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(kingb));
    // printf("Kingw " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(kingw));

    if(PieceHandlers::getType(kingb) == KING
        && PieceHandlers::getType(kingw) == KING) 
        printf("King test passed\n");
    
    // printf("NAP " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(empty));

    if(PieceHandlers::getType(empty) == NAP)
        printf("NAP test passed\n");
    
    if(PieceHandlers::getColor(pawnb) == BLACK &&
        PieceHandlers::getColor(rookb) == BLACK &&
        PieceHandlers::getColor(bishopb) == BLACK &&
        PieceHandlers::getColor(knightb) == BLACK &&
        PieceHandlers::getColor(queenb) == BLACK &&
        PieceHandlers::getColor(kingb) == BLACK)
        printf("Black test passed\n");

    if(PieceHandlers::getColor(pawnw) == WHITE &&
        PieceHandlers::getColor(rookw) == WHITE &&
        PieceHandlers::getColor(bishopw) == WHITE &&
        PieceHandlers::getColor(knightw) == WHITE &&
        PieceHandlers::getColor(queenw) == WHITE &&
        PieceHandlers::getColor(kingw) == WHITE)
        printf("White test passed\n");
}

void testTableStatsUpdater() {
    // test initial chess table
    //Table *tableobj = new Table();

    // test random layout table
    //Table *tableobj = new Table(0);

    // test custom layout table
    Table *tableobj = new Table(0, 0);

    printTable(tableobj->table);

    printTableBits(tableobj->table);

}

void testMoveConvert() {
    Move *move = Move::moveTo("a1", "a8");
    Move::convertStrToIdx(*move);
    printf("Source (%hhd, %hhd)\nDestination (%hhd, %hhd)\n", move->source_idx->first, move->source_idx->second, move->destination_idx->first, move->destination_idx->second);

    std::pair a = {0, 0};
    std::pair b = {7, 7};
    move = Move::moveTo(a, b);
    Move::convertIdxToStr(*move);
    printf("Source %s\nDestination %s\n", move->source_str.value().c_str(), move->destination_str.value().c_str());
}

void testGenerateMoves() {
    // test custom layout table
    Table *tableobj = new Table(0, 0);

    printTable(tableobj->table);
    printTableBits(tableobj->table);

    std::vector<Move> moves = tableobj->generateAllPossibleMoves();

    for(auto move : moves)
        printf("Source (%hhd, %hhd)\tDestination (%hhd, %hhd)\n", move.source_idx->first, move.source_idx->second, move.destination_idx->first, move.destination_idx->second);

}

int main() {
    //testPieceHandler();
    //testTableStatsUpdater();
    //testMoveConvert();
    testGenerateMoves();
}