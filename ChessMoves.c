/* University of Central Florida
 * COP 3502C Fall 2016
 * Author: Michael Jimenez
 */
#include <stdio.h>
#include <stdlib.h>
#include "ChessMoves.h"
#include <string.h>
#include <ctype.h>

// Local structure definitions

typedef struct Location_2D
{
	// the square's row (0 through 1)
	int row;

	// the square's row (0 through 1)
	int col;
} Location_2D;

typedef struct Move_2D
{
	// location where this piece is moving from
	Location_2D from_loc;

	// location where this piece is moving to
	Location_2D to_loc;

	// what type of chess piece is being moved
	char piece;

	// whether this move captures another piece
	short int isCapture;

	// the color of the piece being moved
	Color color;
} Move_2D;

#define TRUE    1
#define FALSE   0
#define DEBUG2   //Set to DEBUG to put in debug mode.

// Local Function Prototypes

int translate_2D(Move *move, Move_2D *move_2d);        // converts chess notation to 2D notation
int find_Pawn_Loc(char **board, Move_2D *move_in_2d);  // finds where the pawn is located before the move
int find_Knight_Loc(char **board, Move_2D *move_2d);   // finds where the knight is located before the move
int find_Rook_Loc(char **board, Move_2D *move_2d);     // finds where the rook is located before the move
int find_Queen_Loc(char **board, Move_2D *move_2d);    // finds where the queen is located before the move
int find_Bishop_Loc(char **board, Move_2D *move_2d);   // finds where the bishop is located before the move
int find_King_Loc(char **board, Move_2D *move_2d);     // finds where the king is located before the move

char **playTheGame(Game *g) //This function should print out the game board with the moves altered on the board.
{
    int k;
    Move blackMoves, WhiteMoves;
    char **board = createChessBoard();
    printChessBoard(board);

    for(k=0;k< g->numMoves; k++)
    {
        parseNotationString(g->moves[k],&WhiteMoves,&blackMoves);
        findFromLoc(board,&WhiteMoves);
        movePiece(board,&WhiteMoves);
        printChessBoard(board);
        findFromLoc(board,&blackMoves);
        movePiece(board, &blackMoves);
        printChessBoard(board);
    }
    return board; //Needs to return (New 2D array) pointer to newly created game board with completed moves.
}

char **createChessBoard(void)
{
    int i=0,r=0;
    char **board = NULL;

    	board = malloc(sizeof(char *) * 8); //Allocates spaces for the Game Board.

    	for (i = 0; i < 8; i++)
        {
            board[i] = malloc(sizeof(char) * 8);
        }
                                                                      //Visual game board format for chess.
    board[0][0]='R',board[0][1]='N',board[0][2]='B',board[0][3]='Q'; //Fills all other pieces then Pawns.
    board[0][4]='K',board[0][5]='B',board[0][6]='N',board[0][7]='R';

    for(i=0;i<8;i++) //Fills in all the Pawn pieces.
    {
        board[1][i] = 'P';
        board[6][i] = 'p';
    }

    for(i=2;i<6;i++) //Makes empty spaces on game board and sets them to null preventing garbage.
    {
        for(r=0;r<8;r++)
        {
            board[i][r] = ' ';
        }
    }

    board[7][0]='r',board[7][1]='n',board[7][2]='b',board[7][3]='q';
    board[7][4]='k',board[7][5]='b',board[7][6]='n',board[7][7]='r';

    return board;
}

char **destroyChessBoard(char **board)
{
    int i;

    for (i = 0; i < 8; i++)
            free(board[i]);

    board = NULL;
    free(board);
    return board;
}

void printChessBoard(char **board)
{
    int i,k;
                            //Visual game board format for chess.
    printf("========\n");

    for(i=0;i<8;i++) // Prints chars in 2D array.
        {
            for(k=0;k<8;k++)
            {
                printf("%c",board[i][k]);
            }
            printf("\n");
        }

    printf("========\n\n");
    fflush(stdout);

    return;
}

void parseNotationString(char *str, Move *whiteMove, Move *blackMove)
{
    int i = 0;
    int k = 0;
    int FirstSpace = 0;
    int NumIndex = 0;
    int SecondSpace = 0;
    int MovLen = 0;
    int CaptureLoc = 0;
    char NumChessMoves[10];
    char WhiteChessScript[10];
    char BlackChessScript[10];

    MovLen = strlen(str);  // length of string

    // Parse the string into 3 sub strings

    for(i=0;i<MovLen;i++) //Creates the first sub-string.
    {
        if(str[i]==' ')
        {
            break;
        }
        NumChessMoves[i] = str[i];
    }
    NumChessMoves[i] = '\0';

    FirstSpace = i;
    for(i=FirstSpace+1;i<MovLen;i++) //Creates the Second sub-string.
    {
        if(str[i]==' ')
            break;

            WhiteChessScript[k] = str[i];
            k++;
    }
    WhiteChessScript[k] = '\0';
    SecondSpace = i;
    k=0;
    for(i=SecondSpace+1;i<=MovLen-1;i++) //Creates the Final sub STR.
    {
            BlackChessScript[k] = str[i];
            k++;
    }
    BlackChessScript[k] = '\0';
    NumIndex = atoi(NumChessMoves);

    for(i=0;i<(strlen(WhiteChessScript));i++) //Checks if the white move is a capture.
    {
        if(WhiteChessScript[i] == 'x')
        {
             whiteMove->isCapture = 1;
             CaptureLoc = i;
             break;
        }
        else
        {
            whiteMove->isCapture = 0;
        }
    }

    for(i=0;i<(strlen(BlackChessScript));i++) //Checks if the black move is a capture.
    {
        if(BlackChessScript[i] == 'x')
        {
             blackMove->isCapture = 1;
             break;
        }
        else
        {
            blackMove->isCapture = 0;
        }
    }
        whiteMove->piece = 'P';//Default piece if no other piece is found.
        blackMove->piece = 'P';

    for(i=0; i<(strlen(WhiteChessScript));i++) //Checks what the white piece is.
    {
        if((WhiteChessScript[i] == 'R') ||
           (WhiteChessScript[i] == 'N') ||
           (WhiteChessScript[i] == 'B') ||
           (WhiteChessScript[i] == 'Q') ||
           (WhiteChessScript[i] == 'K'))
        {
         whiteMove->piece = WhiteChessScript[i];
         break;
        }
    }

    for(i=0; i<(strlen(BlackChessScript));i++) //Checks what the black piece is.
    {
        if((BlackChessScript[i] == 'R') ||
           (BlackChessScript[i] == 'N') ||
           (BlackChessScript[i] == 'B') ||
           (BlackChessScript[i] == 'Q') ||
           (BlackChessScript[i] == 'K'))
        {
         blackMove->piece = BlackChessScript[i];
         break;
        }
    }

    whiteMove->color = WHITE; //Sets piece color.
    blackMove->color = BLACK;

/* ******** Now setting the to and from positions for the white pieces **********/

    whiteMove->from_loc.col = 'x'; //Default set from location.
    if(strlen(WhiteChessScript) >= 4)
    {
        for(i=0;i<2;i++) //Check for White moves fromloc.col
        {
            if( (WhiteChessScript[i] >= 'a') && (WhiteChessScript[i] <= 'h'))
            {
                whiteMove->from_loc.col = WhiteChessScript[i]; // store the from location column
                break;
            }
        }

        whiteMove->from_loc.row = -1; //Default white position row
        for(i=0;i<2;i++) //Check for White moves fromloc.row
        {
            if((WhiteChessScript[i] >= '1') && (WhiteChessScript[i] <= '8'))
            {
                whiteMove->from_loc.row = (int)(WhiteChessScript[i] - '0');  // Convert Ascii Characters '1'-'8' to an integer and store from location row
                break;
            }
        }

        whiteMove->to_loc.col = 'x'; //Default white toloc.col position.
        for(i=2;i<strlen(WhiteChessScript);i++) //Check for White moves toloc.col
        {
            if((WhiteChessScript[i] >= 'a') && (WhiteChessScript[i] <= 'h')) /////Check this!!!!!!!!!!!!!!!!!!!!!!!!!! i=2????????
            {
                whiteMove->to_loc.col = WhiteChessScript[i];
                break;
            }
        }

        whiteMove->to_loc.row = -1; //Default for white toloc.row.
        for(i=1;i<strlen(WhiteChessScript);i++) //Check for White moves toloc.row
        {
            if((WhiteChessScript[i] >= '1') && (WhiteChessScript[i] <= '8' ))
            {
                whiteMove->to_loc.row = (int)(WhiteChessScript[i] - '0');
                break;
            }
        }
    }

 /* ******** Now setting the to and from positions for the black pieces **********/
    if(strlen(BlackChessScript) >= 4)
    {
        blackMove->from_loc.col = 'x'; //Default black from loc position.
        for(i=0;i<2;i++) //Check for Black moves fromloc.col
        {
            if((BlackChessScript[i] >= 'a') && (BlackChessScript[i] >= 'a'))
            {
                blackMove->from_loc.col = BlackChessScript[i];
                break;
            }
        }

        blackMove->from_loc.row = -1; //Default black from loc position.
        for(i=0;i<2;i++) //Check for Black moves fromloc.row
        {
            if((BlackChessScript[i] >= '1') && (BlackChessScript[i] <= '8'))
            {
                blackMove->from_loc.row = (int) (BlackChessScript[i] - '0');
                break;
            }
        }

        blackMove->to_loc.col = 'x'; //Default black to loc position.
        for(i=2;i<strlen(BlackChessScript);i++) //Check for Black moves toloc.col
        {
            if((BlackChessScript[i] >= 'a') && (BlackChessScript[i] <= 'h'))
            {
                blackMove->to_loc.col = BlackChessScript[i];
                break;
            }
        }

        blackMove->to_loc.row = -1; //Default black to loc position.
        for(i=1;i<strlen(BlackChessScript);i++) //Check for Black moves toloc.row
        {
            if((BlackChessScript[i] >= '1') && (BlackChessScript[i] <= '8'))
            {
                blackMove->to_loc.row = (int) (BlackChessScript[i] - '0');
                break;
            }
        }
    }

printf("%s\n",BlackChessScript);

    printf("black piece = %c\n",blackMove->piece);
    printf("black is capture = %d\n",blackMove->isCapture);
    printf("black fromloc.row = %d\n",blackMove->from_loc.row);
    printf("black fromloc.col = %c\n",blackMove->from_loc.col);
    printf("black toloc.row = %d\n",blackMove->to_loc.row);
    printf("black toloc.col = %c\n",blackMove->to_loc.col);


    if(strlen(WhiteChessScript) < 4)
    {
      whiteMove->from_loc.col = 'x';
      whiteMove->from_loc.row = -1;
      whiteMove->to_loc.col = 'x'; //Default white toloc.col position.
      whiteMove->to_loc.row = -1; //Default for white toloc.row.
        for(i=0;i<strlen(WhiteChessScript);i++) //Check for White moves toloc.col
        {
            if((WhiteChessScript[i] >= 'a') && (WhiteChessScript[i] <= 'h'))
            {
                whiteMove->to_loc.col = WhiteChessScript[i];
                break;
            }
        }

        for(i=0;i<strlen(WhiteChessScript);i++) //Check for White moves toloc.row
        {
            if((WhiteChessScript[i] >= '1') && (WhiteChessScript[i] <= '8'))
            {
                whiteMove->to_loc.row = (int) (WhiteChessScript[i] - '0');
                break;
            }
        }
    }

    if(strlen(BlackChessScript) < 4)
    {
      blackMove->from_loc.col = 'x';
      blackMove->from_loc.row = -1;
      blackMove->to_loc.col = 'x'; //Default black to loc position.
      blackMove->to_loc.row = -1; //Default black to loc position.
        for(i=0;i<strlen(BlackChessScript);i++) //Check for Black moves toloc.col
        {
            if((BlackChessScript[i] >= 'a') && (BlackChessScript[i] <= 'h'))
            {
                blackMove->to_loc.col = BlackChessScript[i];
                break;
            }
        }
        for(i=0;i<strlen(BlackChessScript);i++) //Check for Black moves toloc.row
        {
            if((BlackChessScript[i] >= '1') && (BlackChessScript[i] <='8'))
            {
                blackMove->to_loc.row =  (int) (BlackChessScript[i] - '0');
                break;
            }
        }
    }
return;
}

void translate_2d(Move *move, Move_2D *move_2d)
{
    move_2d->color = move->color;
    move_2d->piece = move->piece;
    move_2d->isCapture = move->isCapture;

    move_2d->to_loc.row = 8 - move->to_loc.row;
    move_2d->to_loc.col = (int) (move->to_loc.col - 'a');

    if(move->from_loc.col == 'x')
    {
        move_2d->from_loc.col = -1;
    }
    else
    {
         move_2d->from_loc.col = (int) (move->from_loc.col - 'a');
    }
    if(move->from_loc.row == -1)
    {
        move_2d->from_loc.row = -1;
    }
    else
    {
          move_2d->from_loc.row = 8 - move->from_loc.row;
    }
    return;
}

int find_Pawn_Loc(char **board, Move_2D *move_2d)
{
   int piece_found = FALSE;
   int pawn1 = FALSE;
   int pawn2 = FALSE;
   int num_pawns = 0;
   int t_row=0, t_col=0;    // 2D coordinates of pawn to move
   int row_p1=0, col_p1=0;  // temp coordinates  p1
   int row_p2=0, col_p2=0;  // temp coordinates  p2

    t_row = move_2d->to_loc.row;
    t_col = move_2d->to_loc.col;

    if(move_2d->color == WHITE)
    {
        if(move_2d->isCapture == TRUE)
        {
           // can only be one space away on the diagonal

            if(t_col >=1 && t_col <=6) // if center of the board
            {
                if((board[t_row+1][t_col+1] =='p')) // check diagonal right
                {
                    num_pawns++;
                    row_p1 = t_row+1;
                    col_p1 = t_col+1;
                    piece_found = TRUE;
                    pawn1 = TRUE;
                }
                if((board[t_row+1][t_col-1] =='p')) // check diagonal diagonal left
                {
                    num_pawns++;
                    row_p2 = t_row+1;
                    col_p2 = t_col-1;
                    piece_found = TRUE;
                    pawn2 = TRUE;
                }
                if((pawn1 == TRUE) && (pawn2 == TRUE))  // both pawns are valid moves, check move structure for col provided in move
                {
                    if((col_p1 == move_2d->from_loc.col))   // if col provided matches in move
                    {
                        pawn2 = FALSE;                      // knight 1 is the right piece
                    }
                    else if((col_p2 == move_2d->from_loc.col))
                    {
                        pawn1 = FALSE;                      // pawn 2 is the right piece
                    }
                }
            }
            else  //pawn edge case
            {
                if(t_col == 0)  //check pawn to the right
                {
                    if(board[t_row+1][t_col+1] =='p')
                    {
                        num_pawns++;
                        row_p1 = t_row+1;
                        col_p1 = t_col+1;
                        piece_found = TRUE;
                        pawn1 = TRUE;
                    }
                }
                else if (t_col == 7) // check pawn to the left
                {
                        num_pawns++;
                        row_p1 = t_row+1;
                        col_p1 = t_col-1;
                        piece_found = TRUE;
                        pawn1 = TRUE;
                }
                else
                {
                        piece_found = FALSE;
                        printf("** Error: Pawn Not Found\n");
                }
            }
        }
        else   // normal pawn processing, one or two moves forward
        {
           if(t_row == 4)
           {
               if((board[t_row+1][t_col] == ' ') && (board[t_row+2][t_col] =='p'))  // is it pawn's first move (2 spaces)?
               {
                   piece_found = TRUE;
                   pawn1 = TRUE;
                   row_p1 = t_row+2;
                   col_p1 = t_col;
               }
               else if(board[t_row+1][t_col] == 'p') // only one space move
               {
                   piece_found = TRUE;
                   pawn1 = TRUE;
                   row_p1 = t_row+1;
                   col_p1 = t_col;
               }
           }
           else //can only be one space move
           {
               if(board[t_row+1][t_col] == 'p')
               {
                   piece_found = TRUE;
                   pawn1 = TRUE;
                   row_p1 = t_row+1;
                   col_p1 = t_col;
               }
           }
        }
    }
    else if(move_2d->color == BLACK)
    {
        if(move_2d->isCapture == TRUE)
        {
           // can only be one space away on the diagonal

            if(t_col >=1 && t_col <=6) // if center of the board
            {
                if((board[t_row-1][t_col-1] =='P')) // check diagonal right
                {
                    num_pawns++;
                    row_p1 = t_row-1;
                    col_p1 = t_col-1;
                    piece_found = TRUE;
                    pawn1 = TRUE;
                }
                if((board[t_row-1][t_col+1] =='P')) // check diagonal diagonal left
                {
                    num_pawns++;
                    row_p2 = t_row-1;
                    col_p2 = t_col+1;
                    piece_found = TRUE;
                    pawn2 = TRUE;
                }
                if((pawn1 == TRUE) && (pawn2 == TRUE))  // both pawns are valid moves, check move structure for col provided in move
                {
                    if((col_p1 == move_2d->from_loc.col))   // if col provided matches in move
                    {
                        pawn2 = FALSE;                      // pawn 1 is the right piece
                    }
                    else if((col_p2 == move_2d->from_loc.col))
                    {
                        pawn1 = FALSE;                      // pawn 2 is the right piece
                    }
                }
            }
            else  //pawn edge case
            {
                if(t_col == 0)  //check pawn to the right
                {
                    if(board[t_row-1][t_col+1] =='P')
                    {
                        num_pawns++;
                        row_p1 = t_row-1;
                        col_p1 = t_col+1;
                        piece_found = TRUE;
                        pawn1 = TRUE;
                    }
                }
                else if (t_col == 7) // check pawn to the left
                {
                        num_pawns++;
                        row_p1 = t_row+1;
                        col_p1 = t_col-1;
                        piece_found = TRUE;
                        pawn1 = TRUE;
                }
                else
                {
                        piece_found = FALSE;
                        printf("** Error: Pawn Not Found\n");
                }

            }
        }
        else   // normal pawn processing, one or two moves forward
        {
           if(t_row == 3)
           {
               if((board[t_row-1][t_col] == ' ') && (board[t_row-2][t_col] =='P'))  // is it pawn's first move (2 spaces)?
               {
                   piece_found = TRUE;
                   pawn1 = TRUE;
                   row_p1 = t_row-2;
                   col_p1 = t_col;
               }
               else if(board[t_row-1][t_col] == 'P') // only one space move
               {
                   piece_found = TRUE;
                   pawn1 = TRUE;
                   row_p1 = t_row-1;
                   col_p1 = t_col;
               }
           }
           else //can only be one space move
           {
               if(board[t_row-1][t_col] == 'P')
               {
                   piece_found = TRUE;
                   pawn1 = TRUE;
                   row_p1 = t_row-1;
                   col_p1 = t_col;
               }
           }
        }
    }

    if(pawn1 == TRUE)
    {
        piece_found = TRUE;               // set that we found the piece
        move_2d->from_loc.row = row_p1;
        move_2d->from_loc.col = col_p1;
    }
    else if(pawn2 == TRUE)
    {
        piece_found = TRUE;               // set that we found the piece
        move_2d->from_loc.row = row_p2;
        move_2d->from_loc.col = col_p2;
    }

    if(piece_found == TRUE)
    {
        #ifdef DEBUG
        printf("pawn found [row:%d][col:%d]\n",move_2d->from_loc.row, move_2d->from_loc.col);
        #endif // Debug
    }
    else
    {
        printf("**Error: No Pawn found\n");
    }

   return(piece_found);
}


int find_Knight_Loc(char **board, Move_2D *move_2d)
{
    int piece_found = FALSE;
    int knight1 = FALSE;
    int knight2 = FALSE;
    int num_knights = 0;
    int row=0,col=0;
    int row_n1=0, col_n1=0;
    int row_n2=0, col_n2=0;

    int piece ='N'; // default Look for black Knights(s) 'B'

#ifdef  DEBUG

    printf("color=%d\n isCapture=%d\n piece=%c\n move2d_to_row=%d\n move2d_to_col=%d\n move2d_from_row=%d\n move2d_from_col=%d\n\n",
           move_2d->color,
           move_2d->isCapture,
           move_2d->piece,
           move_2d->to_loc.row,
           move_2d->to_loc.col,
           move_2d->from_loc.row,
           move_2d->from_loc.col);
#endif

    if(move_2d->color == WHITE)
        piece = tolower(move_2d->piece); //look for white knights

    for(row=0; row<8;row++)
    {
        for(col=0; col<8; col++)
        {
            if(board[row][col] == piece)
            {
                piece_found = TRUE;
                num_knights++;
                if(num_knights == 1) //first knight found
                {
                    row_n1=row;
                    col_n1=col;
                }
                if(num_knights == 2) //second knight found
                {
                    row_n2=row;
                    col_n2=col;
                    break;
                }
            }
        }
    }
    if(num_knights == 1)
    {
        // only one bishop found save location in move (from_loc)
        move_2d->from_loc.row = row_n1;
        move_2d->from_loc.col = col_n1;
    }
    else if (num_knights == 2)
    {
        piece_found = FALSE;  //reset piece found until the right one is found

       // we found two knights, we now need to figure out which one is the right one moving by proper distance

       row = abs(row_n1 - move_2d->to_loc.row);  // positive distance away
       col = abs(col_n1 - move_2d->to_loc.col);  // positive distance away

       if(((row == 2) && (col==1)) || ((row==1) && (col==2))) //is knight 1 right distance away
       {
           knight1 = TRUE;
       }

       row = abs(row_n2 - move_2d->to_loc.row);  // positive distance away
       col = abs(col_n2 - move_2d->to_loc.col);  // positive distance away

        if(((row == 2) && (col==1)) || ((row==1) && (col==2))) //is knight 2 right distance away
       {
           knight2 = TRUE;
       }

       if((knight1 == TRUE) && (knight2 == TRUE))  // both knights are valid moves, check move structure for row or col provided in move
       {
           if((row_n1 == move_2d->from_loc.row) || (col_n1 == move_2d->from_loc.col))  // if knight1 found matches row or col provided in move
           {
              knight2 = FALSE;                  // knight 1 is the right piece
           }
           else if((row_n2 == move_2d->from_loc.row) || (col_n2 == move_2d->from_loc.col))
           {
              knight1 = FALSE;                  // knight 2 is the right piece
           }
       }

       if(knight1 == TRUE)
       {
            piece_found = TRUE;               // set that we found the piece
            move_2d->from_loc.row = row_n1;
            move_2d->from_loc.col = col_n1;

       }
       else if(knight2 == TRUE)
       {
            piece_found = TRUE;               // set that we found the piece
            move_2d->from_loc.row = row_n2;
            move_2d->from_loc.col = col_n2;
       }
    }

    if(piece_found == FALSE)
    {
        printf("** Error ** knight not found\n");
    }

   return(piece_found);
}

int find_Rook_Loc(char **board, Move_2D *move_2d)
{
    int piece_found = FALSE;
    int rook1 = FALSE;
    int rook2 = FALSE;
    int num_rooks = 0;
    int r1_row_distance =0, r1_col_distance = 0;
    int r2_row_distance =0, r2_col_distance = 0;
    int row=0,col=0,i;
    int row_r1=0, col_r1=0;
    int row_r2=0, col_r2=0;

    int piece ='R'; // default Look for rook 'R'

    if(move_2d->color == WHITE)
        piece = tolower(move_2d->piece); //look for white rooks.

    for(row=0; row<8;row++)
    {
        for(col=0; col<8; col++)
        {
            if(board[row][col] == piece)
            {
                piece_found = TRUE;
                num_rooks++;
                if(num_rooks == 1) //first rook found
                {
                    rook1 = TRUE;
                    row_r1=row;
                    col_r1=col;
                }
                if(num_rooks == 2) //second rook found
                {
                    rook2 = TRUE;
                    row_r2=row;
                    col_r2=col;
                    break;
                }
            }
        }
    }

    if(num_rooks == 1)         // only one rook found save location in move (from_loc)
    {
        piece_found = TRUE;
        rook1 = TRUE;
    }
    else if (num_rooks == 2)
    {
        piece_found = FALSE;  //reset piece found until the right one is found
                             // we found two rooks, we now need to figure out which one is the right one

            r1_row_distance = abs(row_r1 - move_2d->to_loc.row);  // positive distance away for the first rook
            r1_col_distance = abs(col_r1 - move_2d->to_loc.col);  // positive distance away for the first rook

        if(r1_row_distance == 0 || r1_col_distance == 0)
                {
                    rook1 = TRUE;
                }
                else
                    rook1 = FALSE;

            r2_row_distance = abs(row_r2 - move_2d->to_loc.row);  // positive distance away for the second rook
            r2_col_distance = abs(col_r2 - move_2d->to_loc.col);  // positive distance away for the second rook

        if(r2_row_distance == 0 || r2_col_distance == 0)
            {
                rook2 = TRUE;
            }
            else
                rook2 = FALSE;

        if(rook1 == TRUE && rook2 == TRUE)
            {
                if(row_r1 == move_2d->from_loc.row || col_r1 == move_2d->from_loc.col)
                    rook2 = FALSE;
                 if(row_r2 == move_2d->from_loc.row || col_r2 == move_2d->from_loc.col)
                    rook1 = FALSE;
            }
    }

    if(rook1 == TRUE)
    {
        piece_found = TRUE;               // set that we found the piece
        move_2d->from_loc.row = row_r1;
        move_2d->from_loc.col = col_r1;
    }
    else if(rook2 == TRUE)
    {
        piece_found = TRUE;               // set that we found the piece
        move_2d->from_loc.row = row_r2;
        move_2d->from_loc.col = col_r2;
    }

        if(piece_found == TRUE)
    {
        #ifdef DEBUG
        printf("rook found [row:%d][col:%d]\n",move_2d->from_loc.row, move_2d->from_loc.col);
        #endif
    }
    else
    {
        printf("** Error *** rook not found\n");
    }
    return(piece_found);
}

int find_Queen_Loc(char **board, Move_2D *move_2d)
{
    int piece_found = FALSE;
    int row=0;
    int col=0;
    int piece ='Q'; // default to black queen 'Q'

    if(move_2d->color == WHITE)
        piece = tolower(move_2d->piece);  //look for white queen

    for(row=0; row<8;row++)
    {
        for(col=0; col<8; col++)
        {
            if(board[row][col] == piece)
            {
                piece_found = TRUE;
                move_2d->from_loc.row = row;
                move_2d->from_loc.col = col;
                break;
            }
        }
    }
   return(piece_found);
}

int find_King_Loc(char **board, Move_2D *move_2d)
{
    int piece_found = FALSE;
    int row=0,col=0;
    int piece ='K'; // default to black King 'K'

    if(move_2d->color == WHITE)
        piece = tolower(move_2d->piece);  //look for white queen

    for(row=0; row<8;row++)
    {
        for(col=0; col<8; col++)
        {
            if(board[row][col] == piece)
            {
                piece_found = TRUE;
                move_2d->from_loc.row = row;
                move_2d->from_loc.col = col;
                break;
            }
        }
    }
   return(piece_found);
}

int find_Bishop_Loc(char **board, Move_2D *move_2d)
{
    int piece_found = FALSE;
    int bishop1 = FALSE;
    int bishop2 = FALSE;
    int num_bishops = 0;
    int b1_row_distance =0, b1_col_distance = 0;
    int b2_row_distance =0, b2_col_distance = 0;
    int row=0,col=0;
    int row_b1=0, col_b1=0;
    int row_b2=0, col_b2=0;

    int piece ='B'; // default Look for black bishop(s) 'B'

    if(move_2d->color == WHITE)
        piece = tolower(move_2d->piece); //look for white bishops

    for(row=0; row<8;row++)
    {
        for(col=0; col<8; col++)
        {
            if(board[row][col] == piece)
            {
                piece_found = TRUE;
                num_bishops++;
                if(num_bishops == 1) //first bishop found
                {
                    bishop1 = TRUE;
                    row_b1=row;
                    col_b1=col;
                }
                if(num_bishops == 2) //second bishop found
                {
                    bishop2 = TRUE;
                    row_b2=row;
                    col_b2=col;
                    break;
                }
            }
        }
    }

    if(num_bishops == 1)         // only one bishop found save location in move (from_loc)
    {
        piece_found = TRUE;
        bishop1 = TRUE;
    }
    else if (num_bishops == 2)
    {
        piece_found = FALSE;  //reset piece found until the right one is found
                             // we found two bishops, we now need to figure out which one is the right one

            b1_row_distance = abs(row_b1 - move_2d->to_loc.row);  // positive distance away for the first bishop
            b1_col_distance = abs(col_b1 - move_2d->to_loc.col);  // positive distance away for the first bishop

            if(b1_row_distance == b1_col_distance)
            {
                bishop1 = TRUE;
            }
            else
                bishop1 = FALSE;

            b2_row_distance = abs(row_b2 - move_2d->to_loc.row);  // positive distance away
            b2_col_distance = abs(col_b2 - move_2d->to_loc.col);  // positive distance away

            if(b2_row_distance == b2_col_distance)
            {
                bishop2 = TRUE;
            }
            else
                bishop2 = FALSE;

            if(bishop1 == TRUE && bishop2 == TRUE)
            {
                if(row_b1 == move_2d->from_loc.row || col_b1 == move_2d->from_loc.col)
                    bishop2 = FALSE;
                else if(row_b2 == move_2d->from_loc.row || col_b2 == move_2d->from_loc.col)
                    bishop1 = FALSE;
            }
    }

    if(bishop1 == TRUE)
    {
        piece_found = TRUE;               // set that we found the piece
        move_2d->from_loc.row = row_b1;
        move_2d->from_loc.col = col_b1;
    }
    else if(bishop2 == TRUE)
    {
        piece_found = TRUE;               // set that we found the piece
        move_2d->from_loc.row = row_b2;
        move_2d->from_loc.col = col_b2;
    }

    if(piece_found == TRUE)
    {
        #ifdef DEBUG
        printf("bishop found [row:%d][col:%d]\n",move_2d->from_loc.row, move_2d->from_loc.col);
        #endif
    }
    else
    {
        printf("** Error ** bishop not found\n");
    }

   return(piece_found);
}

void movePiece(char **board, Move *move) //This function simply moves the game pieces and clears the space from where the piece came from.
{
   findFromLoc(board, move);            // identify the from location of the piece

    // Check if the move is for a white piece

    if(move->color == WHITE)            //Moves only the white pieces on the board (denoted by lower case)
    {
            board[(8-move->to_loc.row)][(int)(move->to_loc.col - 'a')] = tolower(move->piece);
            board[(8-move->from_loc.row)][(int)(move->from_loc.col- 'a')] = ' ';
    }

        //Check if the move is for a black pieces.

    if(move->color == BLACK) //Moves only the black pieces on the board.
    {
  //      printf("Black Set %d   %d\n",(8-move->to_loc.row),(move->to_loc.col - 'a'));
            board[(8-move->to_loc.row)][(int)(move->to_loc.col - 'a')] = move->piece;
            board[(8-move->from_loc.row)][(int)(move->from_loc.col- 'a')] = ' ';
    }

 return;
}

void findFromLoc(char **board, Move *move) // Finds where the game piece is currently located.  2D coordinates are used to find the pieces                                           // but returns piece location in
{
    Move_2D move_2d;
    int piece_found = FALSE;
    translate_2d(move, &move_2d);    //translate chess coordinates to 2D

#ifdef  DEBUG

    printf("color=%d\n isCapture=%d\n piece=%c\n move2d_to_row=%d\n move2d_to_col=%d\n move2d_from_row=%d\n move2d_from_col=%d\n\n",
           move_2d.color,
           move_2d.isCapture,
           move_2d.piece,
           move_2d.to_loc.row,
           move_2d.to_loc.col,
           move_2d.from_loc.row,
           move_2d.from_loc.col);
#endif

    if(move->piece == 'N') //Knight game piece.
    {
        piece_found = find_Knight_Loc(board, &move_2d);   // finds where the knight is located
    }
    if(move->piece == 'P') //Pawn game piece.
    {
        piece_found = find_Pawn_Loc(board, &move_2d);     // finds where the pawn is located
    }
    if(move->piece == 'B') //Bishop game piece.
    {
        piece_found = find_Bishop_Loc(board, &move_2d);   // finds where the bishop is located
    }
    if(move->piece == 'R') //Rook game piece. Checks all possible move cases.
    {
        piece_found = find_Rook_Loc(board,&move_2d);     // finds where the rook is located
    }
    if(move->piece == 'K') //King game piece. Checks all possible move cases.
    {
        piece_found = find_King_Loc(board, &move_2d);   // finds where the king is located
    }
    if(move->piece == 'Q') //Queen game piece
    {
        piece_found = find_Queen_Loc(board, &move_2d);    // finds where the queen is located
    }
    if(piece_found == TRUE)  //convert coordinates back to chess coordinate notation
    {
        #ifdef DEBUG
        printf("piece coordinate in 2D [row:%d][col:%d]\n", move_2d.from_loc.row, move_2d.from_loc.col);
        #endif // DEBUG
        move->from_loc.col =  (char) ((move_2d.from_loc.col) + 'a');
        move->from_loc.row =  (8 - move_2d.from_loc.row);
        #ifdef DEBUG
        printf("piece coordinate in Chess [col:%c][row:%d]\n", move->from_loc.col, move->from_loc.row);
        #endif // DEBUG
    }
    else
    {
       // Error, piece not located on board
       printf("** Error Invalid Move Piece:%c, Color:%d Not Found\n", move->piece, move->color);
    }
  return;
}

double difficultyRating(void)
{
    double HowHard = 3.3; //Enter Number from 1.0 to 5.0 which represents the difficulty of this assignment.
    return HowHard;
}

double hoursSpent(void) //An estimate of hours you spent on this programming assignment.
{
    double hoursSpent = 33.3;
    return hoursSpent;
}
