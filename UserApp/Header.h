#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex>


/* Port name */
const PWSTR FilterPortName = L"\\ScannerPort";

#pragma pack(1)

#define buffersize 1024
typedef struct messyMessage
{
	char buffer[buffersize];
}MESSY_MESSAGE;

typedef struct _PERM_CODE
{
	USHORT PermissionLevel;
}PERM_CODE, *PPERM_CODE;

typedef struct _REPLY_MESSAGE
{
	PERM_CODE permission;
}FILTER_REPLY_MESSAGE;

typedef struct joe
{
	FILTER_MESSAGE_HEADER header;
	MESSY_MESSAGE message;
} JOE, *PJOE;

typedef struct _REPLY_STRUCTURE
{
	FILTER_REPLY_HEADER header;
	FILTER_REPLY_MESSAGE reply;
}FILTER_REPLY, *PFILTER_REPLY;

