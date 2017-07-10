#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <Fltkernel.h>
#include <stdio.h>
FLT_PREOP_CALLBACK_STATUS
foo
(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

VOID
ScannerPortDisconnect(
	_In_opt_ PVOID ConnectionCookie
);

NTSTATUS
ScannerPortConnect(
	_In_ PFLT_PORT ClientPort,
	_In_opt_ PVOID ServerPortCookie,
	_In_reads_bytes_opt_(SizeOfContext) PVOID ConnectionContext,
	_In_ ULONG SizeOfContext,
	_Outptr_result_maybenull_ PVOID *ConnectionCookie
);

typedef struct _SCANNER_DATA {

	//
	//  The object that identifies this driver.
	//

	PDRIVER_OBJECT DriverObject;

	//
	//  The filter handle that results from a call to
	//  FltRegisterFilter.
	//

	PFLT_FILTER Filter;

	//
	//  Listens for incoming connections
	//

	PFLT_PORT ServerPort;

	//
	//  User process that connected to the port
	//

	PEPROCESS UserProcess;

	//
	//  Client port for a connection to user-mode
	//

	PFLT_PORT ClientPort;

} SCANNER_DATA, *PSCANNER_DATA;
extern SCANNER_DATA ScannerData;
const PWSTR ScannerPortName = L"\\ScannerPort";
