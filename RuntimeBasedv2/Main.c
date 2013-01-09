#include "MCContext.h"

#include "VTable.h"
#include "VTableSuper.h"
#include "Bird.h"
#include "MCClock.h"
#include "MCProcess.h"
#include "MCException.h"
#include "MCString.h"
#include "MCThread.h"

void mocha_syntex_test(Handle(MCContext) const context);
void menu_drive_test(Handle(MCContext) const context);
void mocha_lib_test();
void test(Handle(MCContext) const context);

extern void function();

/* main */
int MCContext_runloop(Handle(MCContext) const context){
	test(context);

	//static function can not be called even have "extern" declearation
	//function();
}

void test(Handle(MCContext) const context)
{
	//pre load some classes
	debug_log("%s\n", "preload some classes:");
	Handle(VTable) vt = VTable_getInstance();
	ff(vt, MK(whatIsYourClassName));
	VTable_releaseInstance();//do not call fr() on singleton class!
	fr(new(VTableSuper, nil), MK(whatIsYourClassName));
	fr(new(MCProcess, nil), MK(whatIsYourClassName));
	fr(new(MCClock, nil), MK(whatIsYourClassName));

	int selection = ff(context, 
		MK(showMenuAndGetSelectionChar), 
		3, "syntex_test", "menu_drive_test", "lib_test");

	switch(selection){
		case '1':mocha_syntex_test(context);break;
		case '2':menu_drive_test(context);break;
		case '3':mocha_lib_test();break;
	}
}

// use thread pool to management them ?

MCMutexLockNew(lock) = MCMutexLockStaticInitializer;
void *init_routine()
{
	MCMutexLock(lock);
	printf("%s\n", "init the thread only once!");
	MCMutexUnlock(lock);
}

MCMutexLockNew(withCond) = MCMutexLockStaticInitializer;
MCCondLockNew(cond) = MCCondLockStaticInitializer;
MCSpinLockNew(spin); int test_spin_count=0;
void *wait_routine()
{
	MCSpinLockInit(spin ,YES);
		
	printf("%s\n", "wait for signal");
	MCCondWait(cond, withCond);

	printf("%s\n", "received signal!!!");

	MCSpinLock(spin);
	printf("test_spin_count is:%d\n", test_spin_count);
	MCSpinUnlock(spin);
}

void *signal_routine()
{
	int i;
	for (i = 0; i < 10; ++i)
	{
		printf("%d seconds to signal\n", 10-i);
		sleep(1);
	}
	
	printf("%s\n", "signal!!!");
	MCCondSignal(cond);

	MCSpinLock(spin);
	test_spin_count++;
	MCSpinUnlock(spin);

}

/* inner runnable class */
#ifndef _MyRunnable
#define _MyRunnable _MCRunnable
class(MyRunnable);
method(MyRunnable, run, xxx)
{
	int i;
	for (i = 0; i < 10; ++i)
	{
		printf("%s\n", "i am a MCThread!");
		sleep(1);
	}
}
constructor(MyRunnable, _FunctionPointer(my_init_routine))
{
	super_init(this, MCRunnable, my_init_routine);
	if (set_class(this, "MyRunnable", "MCRunnable"));
	{
		//override the super run
		override(this, MK(run), MV(MyRunnable, run));
	}
	return this;
}
#endif

void test_MCClock()
{
	printf("---- test_MCClock START ----\n");
	//test MCClock
	Handle(MCClock) myclock = new(MCClock, nil);
	CString gmtnow = ff(myclock, MK(getCurrentGMTTimeString));
	CString now =    ff(myclock, MK(getCurrentTimeString));

	debug_log("\nGMT time is:%s", gmtnow);
	debug_log("\njapan time is:%s", now);

	debug_log("%s\n", "time works fine");
	ff(myclock, MK(printCurrentGMTTime));
	ff(myclock, MK(printCurrentTime));
	
	clock_t clocks_per_second, clocks_since_start;
	ff(myclock, MK(getCPUClocksPerSecond), &clocks_per_second);
	ff(myclock, MK(getCPUClocksSinceStart), &clocks_since_start);
	debug_log("%ld\n", clocks_per_second);
	debug_log("%ld\n", clocks_since_start);

	//blank time
	ff(myclock, MK(printTime));
	//set time
	ff(myclock, MK(setRawtimeFields), 59, 10, 21, 19, DEC, 2012, 3, 0, 0);
	ff(myclock, MK(printTime));
	//adjust time
	ff(myclock, MK(setTimeToNow));
	ff(myclock, MK(printTime));
	ff(myclock, MK(adjustTime), 0, 0, 2, 0, 0, 0, 0);
	ff(myclock, MK(printTime));
	release(myclock);
	printf("---- test_MCClock END ----\n");
}

void test_MCProcess()
{
	printf("---- test_MCProcess START ----\n");
	//test MCProcess
	Handle(MCProcess) p = new(MCProcess, nil);
		ff(p, MK(printPID));
		ff(p, MK(printPPID));
	release(p);
	printf("---- test_MCProcess END ----\n");
}

void test_MCString()
{
	printf("---- test_MCString START ----\n");
	MCString* newstr = new(MCString, "a new string a");
	int ix;
	for (ix = 0; ix < 20; ++ix)
	{
		ff(newstr, MK(add), " + with append info");
	}
	ff(newstr, MK(print));
	printf("length:%d size:%d\n", newstr->length, newstr->size);

	char csbuff[newstr->size];
	ff(newstr, MK(toCString), csbuff);
	printf("the CString is:\n%s\n", csbuff);
	release(newstr);

	MCString* newstr2 = new(MCString, "a new string b");
	ff(newstr2, MK(add), " + with append info");
	ff(newstr2, MK(print));
	release(newstr2);

	MCString* astr = new(MCString, "string");
	if (fr(new(MCString, "string"), MK(equalTo), astr))
	{
		printf("two string is equal!!!\n");
	}
	release(astr);

	fr(MCString_newForHttp("www.google.com",NO), MK(print));

	printf("size is: %d\n", strlen("size of string"));
	printf("---- test_MCString END ----\n");
}

void test_MCThread()
{
	printf("---- test_MCThread START ----\n");
	//test MCThread
	MCThread* m_thread = new(MCThread, new(MyRunnable, init_routine));
	m_thread->isRunOnce = YES;
	//start
	ff(m_thread, MK(start), nil);
	ff(m_thread, MK(start), nil);
	ff(m_thread, MK(start), nil);

	debug_log("tid is:%lu\n", m_thread->self);
	debug_log("this is Main thread!\n");
	//equal
	if (ff(m_thread, MK(equal), m_thread))
	{
		debug_log("m_thread is equal to m_thread!\n");
	}

	//the Mocha is not thread safe now!
	MCThread* m_thread2 = new(MCThread, new(MyRunnable, nil));
	ff(m_thread2, MK(start), nil);

	//join the thread2 to thread1
	int rescode;
	if(rescode=ff(m_thread, MK(join), m_thread2, nil))
	{
		error_log("the thread join err is: %d", rescode);
	}

	//test condition lock and spin lock
	MCThread* m_wait = new(MCThread, new(MyRunnable, wait_routine));
	MCThread* m_signal = new(MCThread, new(MyRunnable, signal_routine));
	ff(m_wait, MK(start), nil);
	ff(m_signal, MK(start), nil);

	printf("---- test_MCThread END ----\n");
}

void mocha_lib_test()
{
	test_MCClock();
	test_MCProcess();
	test_MCString();
	test_MCThread();
}

void menu_drive_test(Handle(MCContext) const context)
{
	int selection = ff(context, MK(showMenuAndGetSelectionChar), 3, "male", "female", "double");
	//printf("selection is: %c\n", putchar(selection));
	//printf("context->selectionChar is: %c\n", context->selectionChar);

	while(ff(context, MK(showConfirmAndGetBOOL), "are you sure")!=YES){
		ff(context, MK(showMenuAndGetSelectionChar), 3, "male", "female", "double");
	}
	//printf("%d\n", bb);
	printf("%s\n", "your name please:");
	char name[100];
	ff(context, MK(getUserInputString), name);
	
	CString sex;
	switch(selection){
		case '1':
			sex="Mr";
		break;
		case '2':
			sex="Miss";
		break;
		case '3':
			sex="Hi";
		break;
	}

	printf("%s %s %s\n", sex, name, "welcome to Mars");
}

void doSomething();
void doSomething2();

void mocha_syntex_test(MCContext* const context)
{
	//output all cmdline parameters
	ff(context, MK(dumpParas));

	//check whether have cmdline parameter "-c"
	BOOL res;
	if(res = ff(context, MK(isHavePara), "-c"))
		debug_log("%d %s\n", res,ff(context, MK(getPara), 1));

	//check whether have cmdline parameter "-w"
	if(ff(context, MK(isHavePara), "-w"))
		debug_log("%s\n", "context have -w para");

	new(VTableSuper, nil);

	//twice new test
	new(VTable, nil);
	new(VTable, nil);

	//get singleton instance test
	Handle(VTable) ret2= VTable_getInstance();
	Handle(VTable) ret = VTable_getInstance();

	//call by string
	ff(VTable_getInstance(),"draw");

	//protocol method call
	ff(ret, MK(erase));
	ff(ret, MK(redraw));

	Handle(VTableSuper) ret_father = new(VTableSuper, nil);
	ff(ret_father, MK(draw));
	ff(ret_father, MK(erase));
	ff(ret_father, MK(redraw));

	//nested method call
	ff(ret, MK(bmethod), 
		ff(ret,MK(amethod)),
		3.1415, "this is amethod");

	//inherit test
	ff(ret, MK(show), YES, "this is a super method called by child:VTable");
	ret2->info="Changed Info By VTable";
	ff(ret2, MK(show), YES, "this is a super method called by child:VTable ret2");

	//ff-release
	fr(new(VTableSuper, nil), MK(draw));

	//polymorphism test
	Handle(Bird) birdArray[3]={new(Bird, DUCK_TYPE), new(Bird, CHICKEN_TYPE), new(Bird, NONE)};
	int i;
	for (i = 0; i < 3; ++i)
	{
		ff(birdArray[i], MK(fly));
	}

	fr(new(Bird, DUCK_TYPE),    MK(fly));
	fr(new(Bird, CHICKEN_TYPE), MK(fly));
	fr(new(Bird, NONE),         MK(fly));

	//side effect: class method list change dynamically
	Handle(Bird) b1 = new(Bird, DUCK_TYPE);
	Handle(Bird) b2 = new(Bird, CHICKEN_TYPE);
	Handle(Bird) b3 = new(Bird, NONE);

	ff(b1, MK(fly));
	ff(b2, MK(fly));
	ff(b3, MK(fly));

	//clean up controlled by cmdline parameter
	//if(ff(context, MK(isHavePara), "--release")){
		release(nil);
		release(ret);
		release(ret2);
		release(ret_father);
	//}

	//exception support
	try{
		doSomething();
		//doSomething2();
	}catch(MCRuntimeException){
		printf("%s\n", "MCRuntimeException raised");

	}catch(MCIOException){
		MCString* str = get_exception_data(MK(MCIOException));
		ff(str, MK(add), "@this is append info");
		ff(str, MK(print));
		ff(str, MK(print));
		printf("%s\n", "MCIOException raised");
		release(str);

	}catch(MyException){
		printf("%s\n", "MyException raised");

	}catch(MCIOException){
		printf("%s\n", "my defined E(MCIOException) raised");

	}catch_unknown{
		printf("%s\n", "default handling");

	}finally{
		printf("%s\n", "finally");

	}endtry

	//response
	Handle(Bird) abird = new(Bird, DUCK_TYPE);
	if (response(abird, MK(whatIsYourClassName)))
		ff(abird, MK(whatIsYourClassName));

}

void doSomething() throws(MCIOException) {
	set_exception_data(MK(MCIOException), new(MCString, "this is a MCIOException reason"));
	throw(MCIOException);
	printf("%s\n", "doSomething this should never show");
}

void doSomething2() throws(XXX) {
	throw(XXX);
	printf("%s\n", "doSomething2 this should never show");
}