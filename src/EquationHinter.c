#include "EquationHinter.h"
#include "SDL/SDL_gfxPrimitives.h"
#ifndef _TINSPIRE
#include <stdlib.h>
#endif
#include "Font.h"
#include "Replacements.h"

static Font* g_pFontHint = NULL;

void CreateHinter(struct EquationHinter** ppHinter, struct SDL_Surface* pScreen, struct Config* pConfig, KenKenLib kenken)
{
   *ppHinter = malloc(sizeof(struct EquationHinter));
   struct EquationHinter* pHinter = *ppHinter;
   pHinter->m_pScreen = pScreen;
   pHinter->m_KenKen = kenken;
   pHinter->m_pConfig = pConfig;
   pHinter->m_nX = pHinter->m_nY = -1;
   pHinter->m_eStatus = FreshEquation;

   if (g_pFontHint == NULL) {
      g_pFontHint = LoadFont("ARIAL.TTF", NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/, 12);
   }
}

void FreeHinter(struct EquationHinter** ppHinter)
{
   struct EquationHinter* pHinter = *ppHinter;
   pHinter->m_pScreen = NULL;//Does not own
   pHinter->m_KenKen = NULL;//Does not own
   pHinter->m_pConfig = NULL;//Does not own

   if (g_pFontHint != NULL) {
      FreeFont(g_pFontHint);
      g_pFontHint = NULL;
   }

   free(pHinter);
}

void StartEquationHinting(struct EquationHinter* pHinter, int x, int y, int nForced)
{
   if (nForced == 0 && KenKenSpotShareSameEquation(pHinter->m_KenKen, pHinter->m_nX, pHinter->m_nY, x, y) == KENKENLIB_SHARE_EQUATION)
   {
      pHinter->m_nX = x;
      pHinter->m_nY = y;
      return;
   }

   pHinter->m_nX = x;
   pHinter->m_nY = y;
   pHinter->m_eStatus = FreshEquation;
}

int DoesPurmutationHasValuesWithSpotValues(struct EquationHinter* pHinter, int arrValues[])
{
   int x, y;
   for (x = 0; x < GetKenKenWidth(pHinter->m_KenKen); x++)
   {
      for (y = 0; y < GetKenKenHeight(pHinter->m_KenKen); y++)
      {
         int nSpotValue = GetKenKenSpotValue(pHinter->m_KenKen, x, y);
         if (nSpotValue > 0)
         {
            if (KenKenSpotShareSameEquation(pHinter->m_KenKen, x, y, pHinter->m_nX, pHinter->m_nY) == KENKENLIB_SHARE_EQUATION)
            {
               int n;
               int nHasValue = 0;
               for (n = 0; n < VALUES_PER_POSSIBILITY; n++)
               {
                  if (arrValues[n] == nSpotValue)
                  {
                     nHasValue = 1;
                     break;
                  }
               }

               if (nHasValue == 0)
                  return 0;
            }
         }
      }
   }
   return 1;
}

int HasDuplicates(int arrValues[])
{
   int n1, n2;
   for (n1 = 0; n1 < VALUES_PER_POSSIBILITY; n1++)
   {
      int nValueCompare = arrValues[n1];
      if (nValueCompare <= 0)
         break;
      for (n2 = n1+1; n2 < VALUES_PER_POSSIBILITY; n2++)
      {
         int nValue = arrValues[n2];
         if (nValue <= 0)
            break;

         if (nValueCompare == nValue)
            return 1;
      }
   }
   return 0;
}

int IsDisallowedValue(struct EquationHinter* pHinter, int n)
{
   //n should never be 0
   if (pHinter->m_arrValuesNotAllowed[n - 1] == n)
      return 1;
   return 0;
}

void FigureEquations(struct EquationHinter* pHinter, enum MathOperation eOperation, int* pnSolutionIndex, int nNumber, int nTotalNumbers, int nTotalThusFar, int nDesired, int arrValues[])
{
   int amount;
   int n;

   if (*pnSolutionIndex >= NUM_POSSIBILITIES)
      return;

   if (nNumber >= nTotalNumbers)
   {
      if (nTotalThusFar == nDesired) {
         //Check if possibility uses numbers already in spots
         if (DoesPurmutationHasValuesWithSpotValues(pHinter, arrValues) == 0)
            return;

         if (pHinter->m_nDuplicatesAllowed == 0 && HasDuplicates(arrValues) == 1)
            return;

         //TODO: For operations where order doesn't matter (i.e add/multiplication)
         //Check for duplicate of earlier permutation

         //Add to possible solutions
         if (*pnSolutionIndex < NUM_POSSIBILITIES)
         {
            for (n = 0; n < VALUES_PER_POSSIBILITY; n++)
               pHinter->m_arrPossibilities[*pnSolutionIndex][n] = arrValues[n];
            *pnSolutionIndex = *pnSolutionIndex + 1;
         }
      }
      return;
   }

   for (n = min(GetKenKenWidth(pHinter->m_KenKen), 9); n > 0; n--)
   {
      if (IsDisallowedValue(pHinter, n) == 1)
         continue;

      if (nNumber == 0)
      {
         amount = n;
      }
      else
      {
         amount = nTotalThusFar;
         if (eOperation == Add)
         {
            amount += n;
         }
         else if (eOperation == Subtract)
         {
            amount -= n;
         }
         else if (eOperation == Multiply)
         {
            amount *= n;
         }
         else if (eOperation == Divide)
         {
            //n should never be 0
            amount /= n;
         }
      }

      if( eOperation == Add && amount > nDesired)
         continue;
      if (eOperation == Subtract && amount < nDesired)
         continue;
      if (eOperation == Multiply && amount > nDesired)
         continue;
      if (eOperation == Divide && amount < nDesired)
         continue;
      if (amount < 0)
         continue;
      if (eOperation == Add && (amount + (nTotalNumbers - (nNumber + 1))) > nDesired)
         continue;
      if (eOperation == Subtract && (amount - (nTotalNumbers - (nNumber + 1))) < nDesired)
         continue;

      arrValues[nNumber] = n;

      FigureEquations(pHinter, eOperation, pnSolutionIndex, nNumber+1, nTotalNumbers, amount, nDesired, arrValues);
   }
}

//This is called on loop updates so don't do much each time called but accumulate information
void ProcessHinter(struct EquationHinter* pHinter)
{
   if (pHinter->m_eStatus == FinishedEquation)
      return;

   else if (pHinter->m_eStatus == FreshEquation)
   {
      //For this equation figure; get operation and result
      pHinter->m_DisplayBuffer[0] = '\0';
      memset(pHinter->m_arrValuesNotAllowed, 0, sizeof(pHinter->m_arrValuesNotAllowed));
      pHinter->m_eOperation = GetKenKenEquationType(pHinter->m_KenKen, pHinter->m_nX, pHinter->m_nY);
      pHinter->m_nEquationResult = GetKenKenEquationValue(pHinter->m_KenKen, pHinter->m_nX, pHinter->m_nY);
      pHinter->m_eStatus = DeterminedOperationAndResult;
      return;
   }

   else if (pHinter->m_eStatus == DeterminedOperationAndResult)
   {
      int x, y;
      int nMultiRow = 0, nMultiCol = 0;
      int nFirstX = -1, nFirstY = -1;
      int numSpots = 0;
      for (x = 0; x < GetKenKenWidth(pHinter->m_KenKen); x++)
      {
         for (y = 0; y < GetKenKenHeight(pHinter->m_KenKen); y++)
         {
            if (KenKenSpotShareSameEquation(pHinter->m_KenKen, x, y, pHinter->m_nX, pHinter->m_nY) == KENKENLIB_SHARE_EQUATION)
            {
               numSpots++;

               if (nFirstX != -1 && x != nFirstX) {
                  nMultiCol = 1;
               }
               if (nFirstY != -1 && y != nFirstY) {
                  nMultiRow = 1;
               }

               if (nFirstX == -1)
               {
                  nFirstX = x;
               }
               if (nFirstY == -1)
               {
                  nFirstY = y;
               }
            }
         }
      }
      pHinter->m_nNumSpots = numSpots;
      pHinter->m_nDuplicatesAllowed = (nMultiRow && nMultiCol) ? 1 : 0;
      pHinter->m_nHorizontal = nMultiRow ? 0 : 1;

      pHinter->m_eStatus = DeterminedNumberOfSpots;
      return;
   }

   else if (pHinter->m_eStatus == DeterminedNumberOfSpots)
   {
      if (pHinter->m_nDuplicatesAllowed == 0)
      {
         int n;
         if (pHinter->m_nHorizontal == 1) {
            for (n = 0; n < GetKenKenWidth(pHinter->m_KenKen); n++) {
               int nValue = GetKenKenSpotValue(pHinter->m_KenKen, n, pHinter->m_nY);
               if (KenKenSpotShareSameEquation(pHinter->m_KenKen, n, pHinter->m_nY, pHinter->m_nX, pHinter->m_nY) == KENKENLIB_NOT_SHARE_EQUATION &&
                  nValue > 0)
               {
                  pHinter->m_arrValuesNotAllowed[nValue-1] = nValue;
               }
            }
         }
         else {
            for (n = 0; n < GetKenKenHeight(pHinter->m_KenKen); n++) {
               int nValue = GetKenKenSpotValue(pHinter->m_KenKen, pHinter->m_nX, n);
               if (KenKenSpotShareSameEquation(pHinter->m_KenKen, pHinter->m_nX, n, pHinter->m_nX, pHinter->m_nY) == KENKENLIB_NOT_SHARE_EQUATION &&
                  nValue > 0)
               {
                  pHinter->m_arrValuesNotAllowed[nValue - 1] = nValue;
               }
            }
         }
      }
      pHinter->m_eStatus = DeterminedUnallowedValues;
      return;
   }

   else if (pHinter->m_eStatus == DeterminedUnallowedValues)
   {
      int arrValues[VALUES_PER_POSSIBILITY] = { 0 };
      pHinter->m_nPurmutations = 0;
      FigureEquations(pHinter, pHinter->m_eOperation, &pHinter->m_nPurmutations, 0, pHinter->m_nNumSpots, 0, pHinter->m_nEquationResult, arrValues);
      pHinter->m_eStatus = DeterminedPurmutations;
      return;
   }

   else if (pHinter->m_eStatus == DeterminedPurmutations)
   {
      int n;
      for (n = 0; n < pHinter->m_nPurmutations; n++)
      {
         int spot;
         char buffer[10];
         int nSpotInBuffer = 0;
         if (n != 0)
         {
            buffer[nSpotInBuffer++] = ',';
            buffer[nSpotInBuffer++] = ' ';
         }
         for (spot = 0; spot < VALUES_PER_POSSIBILITY; spot++)
         {
            if (pHinter->m_arrPossibilities[n][spot] <= 0)
               break;
            if (spot != 0) {
               char ch = '+';
               if (pHinter->m_eOperation == Add)
                  ch = '+';
               if (pHinter->m_eOperation == Subtract)
                  ch = '-';
               if (pHinter->m_eOperation == Multiply)
                  ch = 'x';
               if (pHinter->m_eOperation == Divide)
                  ch = '/';

               buffer[nSpotInBuffer++] = ch;;
            }
            buffer[nSpotInBuffer++] = pHinter->m_arrPossibilities[n][spot] + '0';
         }
         buffer[nSpotInBuffer] = '\0';
         if (strlen(pHinter->m_DisplayBuffer) + strlen(buffer) < HINTER_DISPLAY_MAX_LENGTH) {
            StringAppend(pHinter->m_DisplayBuffer, HINTER_DISPLAY_MAX_LENGTH, buffer);
         }
         else
         {
            break;
         }
      }
      pHinter->m_eStatus = FinishedEquation;
      return;
   }
}

void DrawHinter(struct EquationHinter* pHinter)
{
   DrawText(pHinter->m_pScreen, g_pFontHint, 0, SCREEN_HEIGHT-15, pHinter->m_DisplayBuffer, 0, 0, 0);
}

