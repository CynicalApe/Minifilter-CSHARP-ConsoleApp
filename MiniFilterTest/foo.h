#pragma once
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <Fltkernel.h>
#include <stdio.h>

#define SECONDS_TO_WAIT 1
#define HUNDERED_NANOSEC_TO_SEC 10000000

typedef struct _FILTER_DATA {

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

} FILTER_DATA, *PFILTER_DATA;
extern FILTER_DATA FilterData;

const PWSTR FilterPortName = L"\\ScannerPort";

FLT_PREOP_CALLBACK_STATUS
CreateRequestCallback
(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

VOID
portDisconnect
(
	_In_opt_ PVOID ConnectionCookie
);

NTSTATUS
portConnect
(
	_In_ PFLT_PORT ClientPort,
	_In_opt_ PVOID ServerPortCookie,
	_In_reads_bytes_opt_(SizeOfContext) PVOID ConnectionContext,
	_In_ ULONG SizeOfContext,
	_Outptr_result_maybenull_ PVOID *ConnectionCookie
);

typedef struct _REPLY_MESSAGE
{
	char replyCode;
}FILTER_REPLY_MESSAGE, *PFILTER_REPLY_MESSAJ;

typedef struct _REPLY_STRUCTURE
{
	FILTER_REPLY_HEADER header;
	FILTER_REPLY_MESSAGE reply;
}FILTER_REPLY, *PFILTER_REPLY;

