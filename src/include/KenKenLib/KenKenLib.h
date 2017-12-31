#ifndef KENKENLIB_H_INCLUDED
#define KENKENLIB_H_INCLUDED

typedef void* KenKenLib;

#define KENKENLIB_OK			(0)
#define KENKENLIB_BADARGUMENT		(-1)
#define KENKENLIB_OUT_OF_MEMORY		(-2)
#define KENKENLIB_NOT_PUZZLE_FILE	(-3)
#define KENKENLIB_UNABLE			(-4)

#define KENKENLIB_GAMEOVER		(1)
#define KENKENLIB_NOT_GAMEOVER		(0)

#define KENKENLIB_SHARE_EQUATION (1)
#define KENKENLIB_NOT_SHARE_EQUATION (0)

#define KENKENLIB_UNIQUE_VALUE  (1)
#define KENKENLIB_NOT_UNIQUE_VALUE  (0)

#define KENKENLIB_EQUATION_SOLVED (1)
#define KENKENLIB_EQUATION_NOT_SOLVED (0)

#define KENKENLIB_CANNOT_UNDO	(1)
#define KENKENLIB_CANNOT_REDO	(1)

enum MathOperation
{
   Add = 0,
   Subtract,
   Multiply,
   Divide
};

enum LevelOperations
{
   AddOperation      = 1 << 0,
   SubtractOperation = 1 << 1,
   MultiplyOperation = 1 << 2,
   DivideOperation   = 1 << 3,
};

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int KenKenLibCreate(KenKenLib* api, const char* pstrFile);
int KenKenLibFree(KenKenLib* api);

int GetKenKenLibError(KenKenLib api);
void ClearKenKenLibError(KenKenLib api);

//////////////////////////////////////////////
//KenKenLib related functions
//////////////////////////////////////////////
int GetKenKenWidth(KenKenLib api);
int GetKenKenHeight(KenKenLib api);
int IsKenKenGameOver(KenKenLib api);

int KenKenUndo(KenKenLib api);
int KenKenRedo(KenKenLib api);

int GetKenKenSpotValue(KenKenLib api, int x, int y);
int SetKenKenSpotValue(KenKenLib api, int x, int y, int value);

int KenKenSpotShareSameEquation(KenKenLib api, int x1, int y1, int x2, int y2);
int GetKenKenEquationValue(KenKenLib api, int x, int y);
int IsKenKenUniqueValueForRowColumn(KenKenLib api, int x, int y);

int GetNumKenKenEquations(KenKenLib api);
int IsKenKenEquationSolved(KenKenLib api, int nIndex);

enum MathOperation GetKenKenEquationType(KenKenLib api, int x, int y);
enum LevelOperations GetKenKenOperations(KenKenLib api);

#endif
