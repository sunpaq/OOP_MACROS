#include "MCContext.h"
//gcc permit use #import in C/C++ language
#ifndef _VTableSuper
#define _VTableSuper _MCObject;\
	int a;\
	int b;\
	int c;\
	char* info;\
	char* main_color;\

constructor(VTableSuper, xxx);

method(VTableSuper, bye, xxx);
method(VTableSuper, show, BOOL isPrint, char* str);
method(VTableSuper, bmethod, int a, double b, char* c);
method(VTableSuper, cmethod, int a, double b, char* c);

#define METHOD
#include "DrawableProtocol.h"
#endif