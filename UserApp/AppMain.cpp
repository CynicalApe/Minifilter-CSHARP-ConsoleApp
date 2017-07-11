
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Fltuser.h>
#include "Header.h"
#include <fstream>
#include <iostream>
#include <string>

using std::ofstream;
using std::string;
/* Port name */
const PWSTR FilterPortName = L"\\ScannerPort";


bool foo(const char *input)
{
	std::ifstream app("C:\\rest.txt", std::ofstream::in);
	char buffer[256]{ '\0' };
	while (app.getline(buffer, 256))
	{
		if (strstr(input, buffer))
			return TRUE;
		memset(&buffer, 0, sizeof(buffer));
	}
	return false;
}

int _cdecl
main
(
)
{
	printf("Scanner: Connecting to the filter ...\n");

	/* Connect to the server port -if exists- */
	HANDLE port;
	HRESULT hr = FilterConnectCommunicationPort(
		FilterPortName,
		0,
		NULL,
		0,
		NULL,
		&port
	);

	/* Connection failed */
	if (IS_ERROR(hr))
	{
		printf("ERROR: Couldn't to filter port: 0x%p\n", port);
		return 2;
	}

	/* Prepare a buffer for the incoming message */
	PJOE message;
	DWORD buf = 1024;
	message = new JOE;

	/* Prepare a buffer for sending the reply */
	PFILTER_REPLY replyMessage;
	replyMessage = new FILTER_REPLY;

	printf("Connected to port: 0x%p\n", port);
	char mesBuff[1024];

	for (int i = 0; i < 1024; i++)
		mesBuff[i] = '\0';

	while (1)
	{
		printf("Listening... \n");

		/* Get the kernel message */
		FilterGetMessage(port, &message->header, buf, NULL);

		char *ptr = message->message.buffer;
		int i = 0;
		while (*ptr != '\0')
		{
			mesBuff[i] = *ptr;
			ptr++;
			i++;
		}

		/* config the reply according to the sent message */
		replyMessage->reply.replyCode = '9';
		replyMessage->header.MessageId = message->header.MessageId;

		if (foo(mesBuff))
		{
			replyMessage->reply.replyCode = '2';
		}

		/* Reply to the sent message */
		NTSTATUS res = FilterReplyMessage(
			port,
			&replyMessage->header,
			sizeof(FILTER_REPLY)
		);

		/* For debugging */
		printf("reply id: %llu \n", replyMessage->header.MessageId);
		printf("reply length: %lu \n", message->header.ReplyLength);
		printf("status 0x%X\n", res);
	}

	/* Close the communication port */
	CloseHandle(port);

	/* Free the allocated resources */
	delete message;
	delete replyMessage;

	return 0;
}

