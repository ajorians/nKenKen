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
   /*int m_nID;*/
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

struct KenKen
{
   int m_nLastError;
   struct KenKenBoard* m_pBoard;
   struct KenKenEquation* m_pEquations;
};

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

   int nSpotInMessage = 0;

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

   pK->m_nLastError = KENKENLIB_OK;
   *api = pK;

   return KENKENLIB_OK;
}

int CrossLibFree(KenKenLib* api)
{
   struct KenKen* pK;
   DEBUG_FUNC_NAME;

   pK = *api;

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
         if (pCell->m_nValue == 0)
            return KENKENLIB_NOT_GAMEOVER;
      }
   }

   //printf("Game over\n");
   return KENKENLIB_GAMEOVER;
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


