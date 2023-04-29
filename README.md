# en-passant


## Structure
- `src`:
    - **Bot.cpp** -> Gets done the two main functionalities: `recording a move`(received from opponent as well as the one sended by the engine), and `calculates the next move` based on all possible moves. 
    - **Main.cpp** -> Handles the `communication` with the `xboard`
    - **PieceHandlers.cpp** -> Calculates `every field that a piece has`, creates pieces and calculates `its possible moves`, which in turn will be added to the poll of all possible moves.
    - **Table.cpp** -> Initializes the `2D array`(the chess board) with the pieces in `their proper position` at the start of a game and most notably goes through all the squares and `calculates its fields`(who is `attacked by` and many more).
    - **Move.cpp** -> Creates and handles Move objects, converts the `string representation` of a move to a `index` in the table and opposite, checks what kind of a move is (drop in, normal, promotion based on which parameters are set: dst, src, replacement)
    - **Test.cpp** -> File used by us for `debugging` purposeses
- `include` -> header files
- `build` -> objects created in the process of compiling
----

## How it was implemented
-  For `memory saving purposes`, we chose to save the square of a chess table and its parameteres as a `one byte`, in which every bit represents something as follows:
    - bit 7: the `colour` of the piece(black/white)
    - bit [6:4]: encoding of the type of piece (Pawn, Rook, Bishop, Knight, King, Queen) (check `Piece.h`)
    - bit 3: if the square is `attacked` by a `black` piece
    - bit 2: if the square is `attacked` by a `white` piece
    - bit 1: if the piece in the square is an `attacker` of the `opponent's king`
    - bit 0: if the piece in the square is a `protector` of its king
---        

### What a table contains:
- `rocinfo` -> which tells us wheter the castling is available: first 4 bits for whites: 1st empty, 2nd the `left rook moved`, 3rd `king moved`, 4th `right rook moved`, and exactly the same for the rest of 4 bits for black.
- `wKx, wKy, bKx, bKy` -> the `coordonates` of the `white & black king` on the table, for easier finding whether the king is in `check`.
- `capturedByWhite`/`capturedByBlack` -> two vectors of pieces captured by either side that are available for dropping in.
- `last_move` -> a Move object representing the last move the `opponent of sideToMove` player did.
- `promotedPawnsWhite`/`promotedPawnsBlack` -> vector that contains the position of the `promoted` pawns of either side, so when `capturing` it by the opposite player, he will get a `pawn`.
- `table[8][8]` -> the chess table itself, every element being an 1 byte described above.
---
### How the recording of a move was done:
- Checks if it's a `promotion`(in this case, its `coordinates` should be retained in `promotedPawns*` to keep monitoring them)
- Checks if it's an `en passant` move (the piece that was captured by passing is added to `captured*` and later the piece itself that moved in diagonal is recorded)
- Checks if its a castling (king moves `2 squares` -> the `rook` that he chooses is `moved to the specific spot`, and later the king is moved as well; `cancels` all `castling` for the player)
- Checks if its a `rook/king that moved` -> mark them in `rocinfo` for knowing in the future if the castling will be available.
- After all the `checks were done`, proceed with the recording
    - the move has a `source` -> the piece is saved, the square is marked as `NAP`(not a piece)
    - the move has a `destination` -> the piece that was `saved` is `put` there, and if there was a piece, means its a `capture` so its save in *captured pieces*
    - the move has a `replacement` -> puts the piece to the destination
- `updates the states` for all the squares of the table -> `recompute` the rest of the bits of a `table square` (attacked by who, attacker of the king, protector of the king).
---
### How updating the state of a square was done:
- Going through all the squares of the table, checks what kind of piece there is:
    - `NAP` -> do nothing about it
    - `PAWN`/`KNIGHT`/`KING` -> they can attack as much as they can move(pawn `one` square in `front` to the `left/right` and knight in `L shape`)
    - `BISHOP`/`QUEEN`/`ROOK` -> they can `attack the empty slots` in all directions they can move `untill` a `piece is there`, and that would be the `last square` they `can attack` in that `direction`. If a `piece` is in the `range of attack` of one of these 3 pieces, checks if the range would countinue on empty squares `without it`, if it would find a `king` of the opposite colour, in this case, the piece in between is a `protector of the king` and should be marked respectively.
---
### How computing all the moves available was done:
- `generateAllPossibleMoves`(Table.h): firstly, it checks if the king is in check, in this situation only a specific moves can be made:
    - `constraints` -> where the destination of the moves available should be(to block the attackers check or to capture it)
    - finds the attacker's position, and thus what kind of piece that is too, and adds the empty spots in the range to the king as `constraints`
- next, based on `if there is any constraint` generated above, the moves available for each piece is a `valid one` if it `blocks the attacker's`(if any) `check`/ moves to an empty square or captures an enemy piece; the moves available for the `king` that is in check are different, he has a `king)constraints`, the range of the attacker, where he can not move to.
- lastly, adds to the `list of moves` the `drop in` possible move: `for each empty spot` on the table, adds a possible move as dropping in `a piece from the captured ones` (`except first` and `last row` for `pawn` drop in)
---

### Choosing the next move from the poll of the moves available:
- For the purpose of `testing the castling`, if there `is` a `castle` move, it `will choose` the first one available, `otherwise`, it will generate a `random` index from the poll of available moves and send that one.