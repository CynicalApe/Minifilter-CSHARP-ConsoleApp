
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Fltuser.h>
#include "..\UserApp\Header.h"

const PWSTR ScannerPortName = L"\\ScannerPort";

int _cdecl
main
(
)
{
	HRESULT hr;
	HANDLE port;	
	PJOE message;
	PRJOE replyMessage;
	DWORD buf = 1024;
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

	message = malloc(sizeof(JOE));
	replyMessage = malloc(sizeof(RJOE));

	if (IS_ERROR(hr))
	{
		printf("ERROR: Connecting to filter port: 0x%p\n", port);
		return 2;
	}

	while (1)
	{
		printf("Connected to port: 0x%p\n", port);
		FilterGetMessage(port, &message->header, buf, NULL);

		/* config reply */
		replyMessage->reply.replyCode = '1';
		replyMessage->header.MessageId = message->header.MessageId;

		NTSTATUS res = FilterReplyMessage(port, &replyMessage->header, sizeof(RJOE));

		printf("reply id: %llu \n", replyMessage->header.MessageId);
		printf("reply length: %lu \n", message->header.ReplyLength);
		
		printf("status 0x%X\n", res);
	}
	CloseHandle(port);
	free(message);

	return 0;
}