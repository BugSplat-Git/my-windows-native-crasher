//
//        This sample project illustrates how to capture crashes (unhandled exceptions) in native Windows applications using BugSplat.
//
//        The shared sample database 'Fred' is used in this example.
//        You may view crashes for the Fred account by logging in at https://www.bugsplat.com:
//        Account (Email Address): Fred 
//                       Password: Flintstone
//
//        In order to assure that crashes sent to the BugSplat website yield exception stack traces with file/line # information, 
//        just rebuild this project.  A Visual Studio post build event is configured to send the resulting .exe and .pdb files
//        to BugSplat via the SendPdbs utility.  If you wish to use your own account and database, you will need to modify the post build
//        event accordingly.  If you do not care about file/line # info or for any reason you do not want  to send these files, 
//        simply disable the post build event.
//
//        More information is available online at https://www.bugsplat.com


#pragma optimize( "", off) // prevent optimizer from interfering with our crash-producing code

#include "stdafx.h"
#include <vector>
#include <new.h>
#include <windows.h>
#include "BugSplat.h"

void MemoryException();
void StackOverflow(void *p);
void DivideByZero();
void ExhaustMemory(); 
void ThrowByUser();
void ThreadException();
void CallAbort();
void InvalidParameters();
void OutOfBoundsVectorCrash();
void VirtualFunctionCallCrash();
void CustomSEHException();
bool ExceptionCallback(UINT nCode, LPVOID lpVal1, LPVOID lpVal2);
MiniDmpSender *mpSender;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExp) {
	if (pExp->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION) {
		mpSender->createReport(pExp);
		TerminateProcess(GetCurrentProcess(), pExp->ExceptionRecord->ExceptionCode);
	} return EXCEPTION_CONTINUE_SEARCH;
}

int wmain(int argc, wchar_t **argv)
{
	if (IsDebuggerPresent())
	{
		wprintf(L"Run this application without the debugger attached to enable BugSplat exception handling.\n");
		DebugBreak();
		exit(0);
	}

	// BugSplat initialization.  Post crash reports to the "Fred" database for application "MyWindowsNativeCrasher" version "1.0"
	mpSender = new MiniDmpSender(L"Fred", L"MyWindowsNativeCrasher", L"1.0", NULL, MDSF_USEGUARDMEMORY | MDSF_LOGFILE | MDSF_PREVENTHIJACKING);

	// The following calls add support for collecting crashes for abort(), vectored exceptions, out of memory,
	// pure virtual function calls, and for invalid parameters for OS functions.
	// These calls should be used for each module that links with a separate copy of the CRT.
	SetGlobalCRTExceptionBehavior();
	SetPerThreadCRTExceptionBehavior();  // This call needed in each thread of your app

	//AddVectoredExceptionHandler(1, VectoredExceptionHandler); 

	// A guard buffer of 20mb is needed to catch OutOfMemory crashes
	mpSender->setGuardByteBufferSize(20*1024*1024);

	// Set optional default values for user, email, and user description of the crash.
	mpSender->setDefaultUserName(_T("Fred"));
	mpSender->setDefaultUserEmail(_T("fred@bedrock.com"));
	mpSender->setDefaultUserDescription(_T("This is the default user crash description."));

	// Process command line args that we need prior to crashing
	for (int i = 1; i < argc; i++) {

		if (!_wcsicmp(argv[i], L"/AttachFiles")) {
			mpSender->setCallback(ExceptionCallback); // files are attached in the callback after the exception occurs
		}
	}

	// Force a crash, in a variety of ways
	for (int i = 1; i < argc; i++) {

		if (!_wcsicmp(argv[i], L"/Crash")) {
			// Don't let the BugSplat dialog appear
			mpSender->setFlags(MDSF_NONINTERACTIVE | mpSender->getFlags());
			MemoryException();
		}

		if (!_wcsicmp(argv[i], L"/MemoryException")) {
			MemoryException();
		}

		else if (!_wcsicmp(argv[i], L"/StackOverflow")) {
			StackOverflow(NULL);
		}

		else if (!_wcsicmp(argv[i], L"/DivByZero")) {
			DivideByZero();
		}

		else if (!_wcsicmp(argv[i], L"/OutOfMemory")) {
			ExhaustMemory();
		}

		else if (!_wcsicmp(argv[i], L"/Throw")) {
			ThrowByUser();
		}

		else if (!_wcsicmp(argv[i], L"/Thread")) {
			ThreadException();
		}

		else if (!_wcsicmp(argv[i], L"/Abort")) {
			CallAbort();
		}

		else if (!_wcsicmp(argv[i], L"/VectorOutOfBounds")) {
			OutOfBoundsVectorCrash();
		}

		else if (!_wcsicmp(argv[i], L"/InvalidParameters")) {
			InvalidParameters();
		}

		else if (!_wcsicmp(argv[i], L"/PureVirtual")) {
			VirtualFunctionCallCrash();
		}

		else if (!_wcsicmp(argv[i], L"/SEH")) {
			mpSender->setFlags(MDSF_NONINTERACTIVE | mpSender->getFlags());
			mpSender->setDefaultUserDescription(_T("BugSplat ALERT - execution continues!"));
			CustomSEHException();
			wprintf(L"Application normal exit.\n");
			exit(0);
		}
	}

	// Default if no crash resulted from command line args
	MemoryException();

	return 0;
}


void MemoryException()
{
	// Dereferencing a null pointer results in a memory exception
	wprintf(L"MemoryException!\n");
	//*(volatile int *)0 = 0;

	char* c = static_cast<char*>(malloc(10)); // allocate
	wprintf(L"DivideByZero!\n");

	memset(c - 1, 0, 10); // underwrite to stomp the CRT structure
	wprintf(L"DivideByZero 2\n");
	free(c); // this may produce Critical error detected c0000374
	wprintf(L"DivideByZero 3\n");
}

void DivideByZero()
{
	// Calling a recursive function with no exit results in a stack overflow
	wprintf(L"DivideByZero!\n");
	volatile int x, y, z;
	x = 1;
	y = 0;
	z = x / y;
}

void StackOverflow(void *p)
{
	// Calling a recursive function with no exit results in a stack overflow
	wprintf(L"StackOverflow!\n");
	volatile char q[10000];
	while (true) {
		StackOverflow((void *)q);
	}
}

void ExhaustMemory()
{
	wprintf(L"ExhaustMemory!\n");

	// Loop until memory exhausted
	while (true)
	{
		char* a = new char[1024 * 1024];
		a[0] = 'X';
	}
}

void ThrowByUser()
{
	wprintf(L"Throw user generated exception!\n");
	throw("User generated exception!");
}

DWORD WINAPI MyThreadCrasher( LPVOID )
{
	wprintf(L"MyThreadCrasher!\n");
	MemoryException();
	return 0;
}

void ThreadException()
{
#define MAX_THREADS 1
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS];

	// Create MAX_THREADS worker threads.
	for (int i = 0; i < MAX_THREADS; i++)
	{
		// Create the thread to begin execution on its own.
		hThreadArray[i] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			MyThreadCrasher,        // thread function name
			NULL,					// argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[i]);   // returns the thread identifier 

		if (hThreadArray[i] == NULL)
		{
			wprintf(L"CreateThread failed");
			ExitProcess(3);
		}
	} // End of main thread creation loop.

	// Wait until all threads have terminated.
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
}

void CallAbort()
{
	wprintf(L"abort()!\n");
	abort();
}

void OutOfBoundsVectorCrash()
{
	wprintf(L"std::vector out of bounds!\n");
	std::vector<int> v;
	v[0] = 5;
}

void InvalidParameters()
{
	wprintf(L"Invalid parameters!\n");
	char *fmt = NULL;
	printf(fmt);
}

void VirtualFunctionCallCrash()
{
	struct Base {
		Base()
		{
			wprintf(L"Pure Virtual Function Call crash!");
			BaseFunc();
		}

		virtual void DerivedFunc() = 0;

		void BaseFunc()
		{
			DerivedFunc();
		}
	};

	struct Derived : public Base
	{
		void DerivedFunc() {}
	};

	Base* instance = new Derived;
	instance->DerivedFunc();
}

DWORD SEHFilterFunction(EXCEPTION_POINTERS *exp)
{
	mpSender->createReport(exp);
	return EXCEPTION_EXECUTE_HANDLER;
}

void CustomSEHException()
{
	__try
	{
		// Use to create a BugSplat report without exiting.
		RaiseException( 
			0x123,         // exception code 
			0,             // continuable exception 
			0, NULL);      // no arguments ;
	}
	__except (SEHFilterFunction(GetExceptionInformation()))
	{
		return;
	}
}


// BugSplat exception callback
bool ExceptionCallback(UINT nCode, LPVOID lpVal1, LPVOID lpVal2)
{

	switch (nCode)
	{
	case MDSCB_EXCEPTIONCODE:
	{
		EXCEPTION_RECORD *p = (EXCEPTION_RECORD *)lpVal1;
		DWORD code = p ? p->ExceptionCode : 0;

		// Create some files in the %temp% directory and attach them
		wchar_t cmdString[2 * MAX_PATH];
		wchar_t filePath[MAX_PATH];
		wchar_t tempPath[MAX_PATH];
		GetTempPathW(MAX_PATH, tempPath);

		wsprintf(filePath, L"%sfile1.txt", tempPath);
		wsprintf(cmdString, L"echo Exception Code = 0x%08x > %s", code, filePath);
		_wsystem(cmdString);
		mpSender->sendAdditionalFile(filePath);

		wsprintf(filePath, L"%sfile2.txt", tempPath);

		wchar_t buf[_MAX_PATH];
		mpSender->getMinidumpPath(buf, _MAX_PATH);

		wsprintf(cmdString, L"echo Crash reporting is so clutch!  minidump path = %s > %s", buf, filePath);
		_wsystem(cmdString);
		mpSender->sendAdditionalFile(filePath);
	}
	break;
	}

	return false;
}