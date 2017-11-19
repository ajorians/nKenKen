#include "Levels.h"
#include "Levels.h"
#include "Replacements.h"

#define LEVEL(x) case x: StringCopy(pstr, 2048, LEVEL_ ## x ); break;

void LevelLoad(char* pstr, int nLevelNum)
{
   switch( nLevelNum ) {
   LEVEL(1);
   
   }
}

