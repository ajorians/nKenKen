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
#elif defined(_WIN32)
   sprintf_s(pstrBuffer, bufferSize, "%d", n);
#else
   sprintf(pstrBuffer, "%d", n);
#endif
}

void StringCopy(char* pstrOutput, int bufferSize, char* pstrInput)
{
#ifdef _TINSPIRE
   strcpy(pstrOutput, pstrInput);
#elif defined(_WIN32)
   strcpy_s(pstrOutput, bufferSize, pstrInput);
#else
   strcpy(pstrOutput, pstrInput);
#endif
}

void StringAppend(char* pstrBuffer, int nSize, char* pstrAddition)
{
#ifdef _TINSPIRE
   strcat(pstrBuffer, pstrAddition);
#elif defined(_WIN32)
   strcat_s(pstrBuffer, nSize, pstrAddition);
#else
   strcat(pstrBuffer, pstrAddition);
#endif
}

