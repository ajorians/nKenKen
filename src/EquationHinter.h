#ifndef EQUATIONHINTER_H
#define EQUATIONHINTER_H

#ifdef _TINSPIRE
#include <os.h>
#endif
#include <KenKenLib/KenKenLib.h>

enum HintingStatus
{
   FreshEquation,
   DeterminedOperationAndResult,
   DeterminedNumberOfSpots,
   DeterminedPurmutations,
   FinishedEquation
};

#define NUM_POSSIBILITIES  (10)
#define VALUES_PER_POSSIBILITY  (10)
#define HINTER_DISPLAY_MAX_LENGTH (40)

struct EquationHinter
{
   KenKenLib m_KenKen;//Does not own
   struct Config* m_pConfig;//Does not own
   struct SDL_Surface* m_pScreen;//Does not own
   int m_nX, m_nY;
   enum HintingStatus m_eStatus;

   int m_nEquationResult;
   enum MathOperation m_eOperation;

   int m_nNumSpots;

   int m_arrPossibilities[NUM_POSSIBILITIES][VALUES_PER_POSSIBILITY];
   int m_nPurmutations;

   char m_DisplayBuffer[HINTER_DISPLAY_MAX_LENGTH];
};

void CreateHinter(struct EquationHinter** ppHinter, struct SDL_Surface* pScreen, struct Config* pConfig, KenKenLib kenken);
void FreeHinter(struct EquationHinter** ppHinter);
void StartEquationHinting(struct EquationHinter* pHinter, int x, int y);
void ProcessHinter(struct EquationHinter* pHinter);
void DrawHinter(struct EquationHinter* pHinter);


#endif

