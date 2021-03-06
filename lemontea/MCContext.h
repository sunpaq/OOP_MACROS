#include "monkc.h"
#include "string.h"

#ifndef MCContext_
#define MCContext_

class(MCContext, MCObject,
	int argc;
	char** argv;
	char selectionChar;
)

utility(MCContext, MCContext*, newWithArgs, int argc, char** argv);
utility(MCContext, char, showMenuAndGetSelectionChar, int count, ...);
utility(MCContext, MCBool, showConfirmAndGetBOOL, const char* confirm);

method(MCContext, void, bye, voida);
method(MCContext, void, dumpParas, voida);
method(MCContext, char*, getPara, int index);
method(MCContext, int, isIndexedParaEqualTo, int index, char* para);
method(MCContext, int, isHavePara, char* para);
method(MCContext, void, getUserInputString, char resultString[]);
method(MCContext, char*, getEnvironmentVar, const char* key);
method(MCContext, int, setEnvironmentVar, const char* key, const char* value, int isOverwrite);
method(MCContext, int, clearEnvironmentVar, const char* key);

extern char **environ;

#endif

/*
#include <stdlib.h>
char *getenv(const char *name);
int putenv(char *str); //need "key=value"
int setenv(const char *name, const char *value, int rewrite);
int unsetenv(const char *name);
*/

