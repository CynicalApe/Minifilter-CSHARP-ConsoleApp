#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Fltuser.h>
#include "Header.h"
#include <fstream>
#include <iostream>
#include <string>
#include <regex>

#define SUCCESS_FOUND 00
#define SUCCESS_NOT_FOUND 01
#define WRONG_FILE_FORMAT 10
#define PERMISSION_FILE_NOT_FOUND 11
#define BUFFER_SIZE 256

#define NO_ACCESS 0
#define READ_ONLY 1
#define WRITE_ONLY 5
#define COMPLETE_ACCESS 7



using std::ofstream;
using std::string;


UINT foo(const char *input, PPERM_CODE perm)
{
	/* Grammar format*/
	std::regex PERMISSION_GRAMMAR(":[0157]:[^:;<>\\/?#*;]+[;]");

	std::ifstream app;
	app.open("C:\\rest.txt", std::ofstream::in);


	
	if (!app)
		return PERMISSION_FILE_NOT_FOUND;

	char buffer[BUFFER_SIZE]{ '\0' };
	while (app.getline(buffer, BUFFER_SIZE))
	{
		if (!std::regex_match(buffer, PERMISSION_GRAMMAR))
		{
			perm->PermissionLevel = 7;
			return WRONG_FILE_FORMAT;
		}

		if (strstr(input,&buffer[3]))
		{
			printf("buffer: %s \n", buffer);
			perm->PermissionLevel = buffer[1] - '0';
			return SUCCESS_FOUND;
		}
		memset(buffer, 0, BUFFER_SIZE);
		
	}
	perm->PermissionLevel = 7;
	return SUCCESS_NOT_FOUND;
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
		printf("mesBuff: %s \n", mesBuff);
		/* config the reply according to the sent message */
		replyMessage->header.MessageId = message->header.MessageId;

		UINT exit = foo(mesBuff, &replyMessage->reply.permission);
		printf("foo returned: %u \n", exit);
		printf("Permission level: %u \n", replyMessage->reply.permission.PermissionLevel);

		/* Reply to the sent message */
		NTSTATUS res = FilterReplyMessage(
			port,
			&replyMessage->header,
			sizeof(FILTER_REPLY)
		);


		memset(mesBuff, 0, 1024);
		/* For debugging */
		printf("status 0x%X\n", res);
	}

	/* Close the communication port */
	CloseHandle(port);

	/* Free the allocated resources */
	delete message;
	delete replyMessage;

	return 0;
}

