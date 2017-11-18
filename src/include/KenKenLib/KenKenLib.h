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

enum MathOperation
{
   Add = 0,
   Subtract,
   Multiply,
   Divide
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

#endif
