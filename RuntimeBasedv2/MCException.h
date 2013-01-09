#include "MCContext.h"
#include <setjmp.h>

/*
this is a wrapper of setjmp.h
to support exception handling
*/

extern jmp_buf exception_env;
extern int exception_type;
#define __exception_try_not_called -1
#define __MCNoneException 0
#define __ECODE(name) __get_exception_code(#name)
unsigned __get_exception_code(char* key);

/* public API */

#define throws(etype)
#define throw(etype)  if(exception_type!=__exception_try_not_called)longjmp(exception_env, __ECODE(etype))

#define try           if((exception_type=setjmp(exception_env))==__MCNoneException)
#define catch(etype)  else if(exception_type==__ECODE(etype))
#define catch_unknown else
#define finally       do
#define endtry        while(0);

#define  MAX_EXCEPTION_NUM 500

id       get_exception_data(char* key);
void     set_exception_data(char* key, id e);