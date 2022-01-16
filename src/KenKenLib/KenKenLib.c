//Public domain :)

#ifdef _TINSPIRE
#include <os.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif
#include "KenKenLib/KenKenLib.h"
#include "Defines.h"

#define KENKENLIB_RUN_SAFE

//#define DEBUG_DETAILS

#define MAX_CELL_VALUES	(10)

struct KenKenEquation
{
   int m_nValue;
   enum MathOperation m_eOperation;
};

struct Cell
{
   int m_nValue;
   struct KenKenEquation* m_pEquation;
};

struct KenKenBoard
{
   int m_nWidth;
   int m_nHeight;
   struct Cell* m_pItems;
};

struct Cell* GetAt(struct KenKenBoard* pBoard, int nX, int nY)
{
#ifdef KENKENLIB_RUN_SAFE
   if( nX < 0 || nY < 0 || (nX >= pBoard->m_nWidth) || (nY >= pBoard->m_nHeight)) {
      printf("Accessing non-existing element %d,%d\n", nX, nY);
      return NULL;
   }
#endif

   struct Cell* pItem = pBoard->m_pItems + (pBoard->m_nWidth * nY + nX);

   return pItem;
}

struct KenKenEquation* GetEquation(struct KenKenEquation* pEquation, int nID)
{
#ifdef KENKENLIB_RUN_SAFE
   if (nID < 0) {
      printf("Accessing non-existing equation %d\n", nID);
      return NULL;
   }
#endif

   struct KenKenEquation* pEq = pEquation + nID;
   return pEq;
}

struct KenKenAction
{
   int x;
   int y;
   int value;
   struct KenKenAction* m_pNext;
};

struct KenKen
{
   int m_nLastError;
   int m_nNumEquations;
   struct KenKenBoard* m_pBoard;
   struct KenKenEquation* m_pEquations;
   struct KenKenAction* m_pUndoActions;
   struct KenKenAction* m_pRedoActions;
};

void ClearUndos( KenKenLib api );
void ClearRedos( KenKenLib api );

void Cleanup(struct KenKen** ppK)
{
   if( *ppK != NULL ) {
      struct KenKen* pK = *ppK;
      if(pK->m_pBoard != NULL ) {
         if(pK->m_pBoard->m_pItems != NULL ) {
            free(pK->m_pBoard->m_pItems);
            pK->m_pBoard->m_pItems = NULL;
         }
         free(pK->m_pBoard);
         pK->m_pBoard = NULL;
      }
      if (pK->m_pEquations != NULL) {
         free(pK->m_pEquations);
         pK->m_pEquations = NULL;
      }
   }
}

int KenKenLibCreate(KenKenLib* api, const char* pstrFile)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = malloc(sizeof(struct KenKen));
   if( pK == NULL ){//Out of memory
      return KENKENLIB_OUT_OF_MEMORY;
   }
   pK->m_pBoard = NULL;

   pK->m_pBoard = malloc(sizeof(struct KenKenBoard));
   if( pK->m_pBoard == NULL ){//Out of memory
      Cleanup(&pK);
      return KENKENLIB_OUT_OF_MEMORY;
   }

   pK->m_pBoard->m_pItems = NULL;

   if( strstr(pstrFile, "KenKen ") != pstrFile ) {//KenKen file version check
      printf("Not KenKen data\n");
      Cleanup(&pK);
      return KENKENLIB_NOT_PUZZLE_FILE;
   }

   if( strstr(pstrFile, "1 ") != (pstrFile + strlen("KenKen ")) ) {//Version check
      printf("Unable to read KenKen data\n");
      Cleanup(&pK);
      return KENKENLIB_NOT_PUZZLE_FILE;
   }

   char* pstr = pstrFile + strlen("KenKen 1 ");

   char buffer[16];
   int nSpotInBuffer = 0;

   int nWidth = -1, nHeight = -1;
   int nNumEquations = -1;
   //int nShowOperations = -1, nKenDoKu = -1;
   while(pstr != '\0') {
      char ch = *pstr; pstr++;
      if( isdigit(ch) ) {
         buffer[nSpotInBuffer++] = ch;
      }
      else {
         if( ch != ' ' && ch != '\n' && ch != '\r' )
            break;
         if( nSpotInBuffer == 0 )
            continue;
         buffer[nSpotInBuffer] = '\0';
         nSpotInBuffer = 0;
         int nValue = atoi(buffer);

#ifdef DEBUG_DETAILS
         printf("Value %d\n", nValue);
#endif

         if (nNumEquations < 0) {
#ifdef KENKENLIB_RUN_SAFE
            if (nValue < 0 || nValue > 100) {
               printf("Bad value for number of equations with value %d\n", nValue);
            }
#endif
            nNumEquations = nValue;
            pK->m_nNumEquations = nNumEquations;
            pK->m_pEquations = malloc(sizeof(struct KenKenEquation)*nNumEquations);

            for (int nEquation = 0; nEquation < nNumEquations; nEquation++) {
               int nEquationValue = -1;
               nSpotInBuffer = 0;
               while (pstr != '\0') {
                  ch = *pstr; pstr++;
                  if (isdigit(ch)) {
                     buffer[nSpotInBuffer++] = ch;
                  }
                  else {
                     if (ch != ' ' && ch != '\n' && ch != '\r') {
                        pstr--;
                        break;
                     }
                     if (nSpotInBuffer == 0)
                        continue;
                     buffer[nSpotInBuffer] = '\0';
                     nSpotInBuffer = 0;
                     nValue = atoi(buffer);

                     if (nEquationValue < 0) {
                        nEquationValue = nValue;
                     }
                     else
                     {
                        struct KenKenEquation* pEquation = pK->m_pEquations + nEquation;
                        pEquation->m_nValue = nEquationValue;
                        pEquation->m_eOperation = (enum MathOperation)nValue;
                        break;
                     }
                  }
               }
            }
         }
         else if( nWidth < 0 ) {
#ifdef KENKENLIB_RUN_SAFE
            if( nValue < 0 || nValue > 20 ) {
               printf("Bad value for width of value %d\n", nValue);
            }
#endif
            nWidth = nValue;
            pK->m_pBoard->m_nWidth = nWidth;
         }
         else if( nHeight < 0 ) {
            int x, y;

#ifdef KENKENLIB_RUN_SAFE
            if( nValue < 0 || nValue > 20 ) {
               printf("Bad value for height of value %d\n", nValue);
            }
#endif

            nHeight = nValue;
            pK->m_pBoard->m_nHeight = nHeight;

            pK->m_pBoard->m_pItems = malloc(nWidth*nHeight*sizeof(struct Cell));

            if( pK->m_pBoard->m_pItems == NULL ) {//Out of memory
               Cleanup(&pK);
               return KENKENLIB_OUT_OF_MEMORY;
            }

            for(x=0; x<nWidth; x++) {
               for(y=0; y<nHeight; y++) {
                  struct Cell* pCell = GetAt(pK->m_pBoard, x, y);
                  pCell->m_nValue = 0;
                  pCell->m_pEquation = NULL;
               }
            }

            for(y=0; y<nHeight; y++) {
               for (x = 0; x<nWidth; x++) {
                  struct Cell* pCell = GetAt(pK->m_pBoard, x, y);

                  nSpotInBuffer = 0;
                  while(pstr != '\0') {
                     ch = *pstr; pstr++;
                     if( isdigit(ch) ) {
                        buffer[nSpotInBuffer++] = ch;
                     }
                     else {
                        if (ch != ' ' && ch != '\n' && ch != '\r' ) {
                           pstr--;
                           break;
                        }
                        if( nSpotInBuffer == 0 )
                           continue;

                        break;
                     }
                  }

                  buffer[nSpotInBuffer] = '\0';
                  nSpotInBuffer = 0;
                  nValue = atoi(buffer);

                  //printf("Value for cell: %d\n", nValue);

                  struct KenKenEquation* pEquation = GetEquation(pK->m_pEquations, nValue);
                  if (pEquation == NULL)
                  {
                     printf("Equation is null; shouldn't happen\n");
                  }
                  pCell->m_pEquation = pEquation;

              }
           }
         }
      }
   }

   pK->m_pUndoActions = NULL;
   pK->m_pRedoActions = NULL;

   pK->m_nLastError = KENKENLIB_OK;
   *api = pK;

   return KENKENLIB_OK;
}

int KenKenLibFree(KenKenLib* api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = *api;

   ClearUndos(*api);
   ClearRedos(*api);

   if( pK->m_pBoard != NULL ) {
      free(pK->m_pBoard);
      pK->m_pBoard = NULL;
   }

   free(pK);
   *api = NULL;
   return KENKENLIB_OK;
}

int GetKenKenLibError(KenKenLib api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   return pK->m_nLastError;
}

void ClearCrossLibError(KenKenLib api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   pK->m_nLastError = KENKENLIB_OK;
}

//KenKenLib related functions
int GetKenKenWidth(KenKenLib api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   return pK->m_pBoard->m_nWidth;
}

int GetKenKenHeight(KenKenLib api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   return pK->m_pBoard->m_nHeight;
}

int IsKenKenGameOver(KenKenLib api)
{
   struct KenKen* pK;
   int x, y;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   //Any spot without a value
   for (x = 0; x < pK->m_pBoard->m_nWidth; x++)
   {
      for (y = 0; y < pK->m_pBoard->m_nHeight; y++)
      {
         struct Cell* pCell = GetAt(pK->m_pBoard, x, y);
         if (pCell->m_nValue <= 0)
            return KENKENLIB_NOT_GAMEOVER;
      }
   }

   for (x = 0; x < pK->m_pBoard->m_nWidth; x++)
   {
      for (y = 0; y < pK->m_pBoard->m_nHeight; y++)
      {
         if (IsKenKenUniqueValueForRowColumn(api, x, y) == KENKENLIB_NOT_UNIQUE_VALUE)
            return KENKENLIB_NOT_GAMEOVER;
      }
   }

   //Check equations are satified
   for (x = 0; x < pK->m_nNumEquations; x++)
   {
      if (IsKenKenEquationSolved(api, x) == KENKENLIB_EQUATION_NOT_SOLVED)
         return KENKENLIB_NOT_GAMEOVER;
   }

   //printf("Game over\n");
   return KENKENLIB_GAMEOVER;
}

void ClearUndos(KenKenLib api)
{
   DEBUG_FUNC_NAME;

   struct KenKen* pK = (struct KenKen*)api;

   struct KenKenAction* pCurrent = pK->m_pUndoActions;
   while(pCurrent != NULL) {
      struct KenKenAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pK->m_pUndoActions = pCurrent;
   }
}

void ClearRedos(KenKenLib api)
{
   DEBUG_FUNC_NAME;

   struct KenKen* pK = (struct KenKen*)api;

   struct KenKenAction* pCurrent = pK->m_pRedoActions;
   while(pCurrent != NULL) {
      struct KenKenAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pK->m_pRedoActions = pCurrent;
   }
}

void AddUndo(KenKenLib api, int x, int y, int value)
{
   DEBUG_FUNC_NAME;

   struct KenKen* pK = (struct KenKen*)api;

   struct KenKenAction* pAction = malloc(sizeof(struct KenKenAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddUndo\n");
   //Assume allocated
   }

   pAction->x = x;
   pAction->y = y;
   pAction->value = value;

   struct KenKenAction* pRoot = pK->m_pUndoActions;
   pAction->m_pNext = pRoot;
   pK->m_pUndoActions = pAction;
}

void AddRedo(KenKenLib api, int x, int y, int value)
{
   DEBUG_FUNC_NAME;

   struct KenKen* pK = (struct KenKen*)api;

   struct KenKenAction* pAction = malloc(sizeof(struct KenKenAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddRedo\n");
      //Assume allocated
   }

   pAction->x = x;
   pAction->y = y;
   pAction->value = value;

   struct KenKenAction* pRoot = pK->m_pRedoActions;
   pAction->m_pNext = pRoot;
   pK->m_pRedoActions = pAction;
}

int KenKenUndo(KenKenLib api)
{
   DEBUG_FUNC_NAME;

   struct KenKen* pK = (struct KenKen*)api;

   struct KenKenAction* pRoot = pK->m_pUndoActions;
   if( pRoot == NULL )
      return KENKENLIB_CANNOT_UNDO;

   pK->m_pUndoActions = pRoot->m_pNext;
   AddRedo(api, pRoot->x, pRoot->y, GetKenKenSpotValue(api, pRoot->x, pRoot->y));
   GetAt(pK->m_pBoard, pRoot->x, pRoot->y)->m_nValue = pRoot->value;

   free(pRoot);
   pRoot = NULL;

   return KENKENLIB_OK;
}

int KenKenRedo(KenKenLib api)
{
   DEBUG_FUNC_NAME;

   struct KenKen* pK = (struct KenKen*)api;

   struct KenKenAction* pRoot = pK->m_pRedoActions;
   if( pRoot == NULL )
      return KENKENLIB_CANNOT_REDO;

   pK->m_pRedoActions = pRoot->m_pNext;
   AddUndo(api, pRoot->x, pRoot->y, GetKenKenSpotValue(api, pRoot->x, pRoot->y));
   GetAt(pK->m_pBoard, pRoot->x, pRoot->y)->m_nValue = pRoot->value;

   free(pRoot);
   pRoot = NULL;

   return KENKENLIB_OK;
}

int GetKenKenSpotValue(KenKenLib api, int x, int y)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;

   if (x < 0 || x >= pK->m_pBoard->m_nWidth || y < 0 || y >= pK->m_pBoard->m_nHeight)
      return KENKENLIB_BADARGUMENT;

   return GetAt(pK->m_pBoard, x, y)->m_nValue;
}

int SetKenKenSpotValue(KenKenLib api, int x, int y, int value)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;

   if (x < 0 || x >= pK->m_pBoard->m_nWidth || y < 0 || y >= pK->m_pBoard->m_nHeight)
      return KENKENLIB_BADARGUMENT;

   if (value < 0 || value > pK->m_pBoard->m_nWidth)
      return KENKENLIB_BADARGUMENT;

   ClearRedos(api);

   int nOldValue = GetAt(pK->m_pBoard, x, y)->m_nValue;
   AddUndo(api, x, y, nOldValue);
   GetAt(pK->m_pBoard, x, y)->m_nValue = value;

   return KENKENLIB_OK;
}

int KenKenSpotShareSameEquation(KenKenLib api, int x1, int y1, int x2, int y2)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;

   if (x1 < 0 || x2 < 0 || x1 >= pK->m_pBoard->m_nWidth || x2 >= pK->m_pBoard->m_nWidth ||
       y1 < 0 || y2 < 0 || y1 >= pK->m_pBoard->m_nHeight || y2 >= pK->m_pBoard->m_nHeight)
      return KENKENLIB_NOT_SHARE_EQUATION;

   if (GetAt(pK->m_pBoard, x1, y1)->m_pEquation != GetAt(pK->m_pBoard, x2, y2)->m_pEquation)
      return KENKENLIB_NOT_SHARE_EQUATION;

   return KENKENLIB_SHARE_EQUATION;
}

int GetKenKenEquationValue(KenKenLib api, int x, int y)
{
   struct KenKen* pK;
   struct KenKenEquation* pEquation;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;

   pEquation = GetAt(pK->m_pBoard, x, y)->m_pEquation;
   return pEquation ? pEquation->m_nValue : -1;
}

int IsKenKenUniqueValueForRowColumn(KenKenLib api, int nX, int nY)
{
   struct KenKen* pK;
   int x, y;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   for (x = 0; x < pK->m_pBoard->m_nWidth; x++)
   {
      if (x == nX)
         continue;

      if (GetAt(pK->m_pBoard, x, nY)->m_nValue == GetAt(pK->m_pBoard, nX, nY)->m_nValue)
      {
         return KENKENLIB_NOT_UNIQUE_VALUE;
      }
   }

   for (y = 0; y < pK->m_pBoard->m_nHeight; y++)
   {
      if (y == nY)
         continue;

      if (GetAt(pK->m_pBoard, nX, y)->m_nValue == GetAt(pK->m_pBoard, nX, nY)->m_nValue)
      {
         return KENKENLIB_NOT_UNIQUE_VALUE;
      }
   }

   return KENKENLIB_UNIQUE_VALUE;
}

int GetNumKenKenEquations(KenKenLib api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   return pK->m_nNumEquations;
}

int CheckSubtractionRecursive(int arr[], int count, int data[], int nSpot, int nDesiredResult)
{
   int arrNumbers[10 * 10];
   memcpy(arrNumbers, arr, 10 * 10);
   int n;
   for (n = 0; n < count; n++)
   {
      if (arrNumbers[n] <= 0)
         continue;

      data[nSpot] = arrNumbers[n];
      arrNumbers[n] = 0;

      if (nSpot+1 == count)
      {
         int spot;

         int value = data[0];
         for (spot = 1; spot < count; spot++)
            value -= data[spot];

         if (value == nDesiredResult)
            return 1;
      }
      else
      {
         if (CheckSubtractionRecursive(arrNumbers, count, data, nSpot + 1, nDesiredResult) == 1)
            return 1;

         arrNumbers[n] = data[nSpot];
      }
   }

   return 0;
}

int CheckDivisionRecursive(int arr[], int count, int data[], int nSpot, int nDesiredResult)
{
   int arrNumbers[10 * 10];
   memcpy(arrNumbers, arr, 10 * 10);
   int n;
   for (n = 0; n < count; n++)
   {
      if (arrNumbers[n] <= 0)
         continue;

      data[nSpot] = arrNumbers[n];
      arrNumbers[n] = 0;

      if (nSpot + 1 == count)
      {
         int spot;

         int value = data[0];
         for (spot = 1; spot < count; spot++)
         {
            if (data[spot] == 0)
               return 0;
            value /= data[spot];
         }

         if (value == nDesiredResult)
            return 1;
      }
      else
      {
         if (CheckDivisionRecursive(arrNumbers, count, data, nSpot + 1, nDesiredResult) == 1)
            return 1;

         arrNumbers[n] = data[nSpot];
      }
   }

   return 0;
}

int IsKenKenEquationSolved(KenKenLib api, int nIndex)
{
   struct KenKen* pK;
   struct KenKenEquation* pEquation;
   int x, y;
   int nCount = 0;
   static int arrSpotValues[10 * 10];
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;

   pEquation = GetEquation(pK->m_pEquations, nIndex);

   for (x = 0; x < pK->m_pBoard->m_nWidth; x++)
   {
      for (y = 0; y < pK->m_pBoard->m_nHeight; y++)
      {
         if (GetAt(pK->m_pBoard, x, y)->m_pEquation == pEquation)
         {
            int nSpotValue = GetKenKenSpotValue(api, x, y);
            if (nSpotValue <= 0)
               return KENKENLIB_EQUATION_NOT_SOLVED;

            arrSpotValues[nCount++] = nSpotValue;
         }
      }
   }

   if (pEquation->m_eOperation == Add)
   {
      int nSum = 0;
      for (x = 0; x < nCount; x++)
         nSum += arrSpotValues[x];

      if( nSum != pEquation->m_nValue )
         return KENKENLIB_EQUATION_NOT_SOLVED;
   }
   else if (pEquation->m_eOperation == Subtract)
   {
      int data[10 * 10];
      if( CheckSubtractionRecursive(arrSpotValues, nCount, data, 0, pEquation->m_nValue) == 0)
         return KENKENLIB_EQUATION_NOT_SOLVED;
   }
   else if (pEquation->m_eOperation == Multiply)
   {
      int nProduct = 1;
      for (x = 0; x < nCount; x++)
         nProduct *= arrSpotValues[x];

      if (nProduct != pEquation->m_nValue)
         return KENKENLIB_EQUATION_NOT_SOLVED;
   }
   else if (pEquation->m_eOperation == Divide)
   {
      int data[10 * 10];
      if (CheckDivisionRecursive(arrSpotValues, nCount, data, 0, pEquation->m_nValue) == 0)
         return KENKENLIB_EQUATION_NOT_SOLVED;
   }

   return KENKENLIB_EQUATION_SOLVED;
}

enum MathOperation GetKenKenEquationType(KenKenLib api, int x, int y)
{
   struct KenKen* pK;
   struct KenKenEquation* pEquation;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;

   pEquation = GetAt(pK->m_pBoard, x, y)->m_pEquation;

   return pEquation->m_eOperation;
}

enum LevelOperations GetKenKenOperations(KenKenLib api)
{
   struct KenKen* pK;
   int i;
   enum LevelOperations eRet = 0;
   DEBUG_FUNC_NAME;

   pK = (struct KenKen*)api;
   for (i = 0; i < pK->m_nNumEquations; i++) {
      enum MathOperation e = pK->m_pEquations[i].m_eOperation;
      if( e == Add )
         eRet |= AddOperation;
      if (e == Subtract)
         eRet |= SubtractOperation;
      if (e == Multiply)
         eRet |= MultiplyOperation;
      if (e == Divide)
         eRet |= DivideOperation;
   }

   return eRet;
}


