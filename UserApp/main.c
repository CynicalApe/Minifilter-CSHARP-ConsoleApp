
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Fltuser.h>

const PWSTR ScannerPortName = L"\\ScannerPort";
#define buffersize 1024
typedef struct joe
{
	FILTER_MESSAGE_HEADER header;
	char buffer[buffersize];
} JOE, *PJOE;

int _cdecl
main
(
)
{
	HRESULT hr;
	HANDLE port;	
	HRESULT kernelmessage;
	JOE message;
	
	printf("Scanner: Connecting to the filter ...\n");
	hr = FilterConnectCommunicationPort
	(
		ScannerPortName,
		0,
		NULL,
		0,
		NULL,
		&port
	);

	if (IS_ERROR(hr))
	{
		printf("ERROR: Connecting to filter port: 0x%08x\n", port);
		return 2;
	}

	while (1)
	{
		printf("Connected to port: 0x%08x\n", port);
		Sleep(2000);
	}
	CloseHandle(port);
	/*else
	{
		printf("Connected to filter port: 0x%08x\n", port);
		Sleep(5000);
		printf("Disconnected to filter port: 0x%08x\n", port);
		CloseHandle(port);
	}*/
	return 0;
}