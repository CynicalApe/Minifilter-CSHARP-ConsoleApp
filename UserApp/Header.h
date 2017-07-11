#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#pragma pack(1)

#define buffersize 1024
typedef struct messyMessage
{
	char buffer[buffersize];
}MESSY_MESSAGE;

typedef struct messyReply
{
	char replyCode;
}MESSY_REPLY;

typedef struct joe
{
	FILTER_MESSAGE_HEADER header;
	MESSY_MESSAGE message;
} JOE, *PJOE;

typedef struct ReplyJoe
{
	FILTER_REPLY_HEADER header;
	MESSY_REPLY reply;
}RJOE, *PRJOE;