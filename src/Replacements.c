#include "Replacements.h"
#include <string.h>

#ifdef _TINSPIRE
#include <os.h>
#else
#include <stdio.h>
#endif

void IntToA(char* pstrBuffer, int bufferSize, int n)
{
#ifdef _TINSPIRE
   sprintf(pstrBuffer, "%d", n);
#else
   sprintf_s(pstrBuffer, bufferSize, "%d", n);
#endif
}

void StringCopy(char* pstrOutput, int bufferSize, char* pstrInput)
{
#ifdef _TINSPIRE
   strcpy(pstrOutput, pstrInput);
#else
   strcpy_s(pstrOutput, bufferSize, pstrInput);
#endif
}

void StringAppend(char* pstrBuffer, int nSize, char* pstrAddition)
{
#ifdef _TINSPIRE
   strcat(pstrBuffer, pstrAddition);
#else
   strcat_s(pstrBuffer, nSize, pstrAddition);
#endif
}

