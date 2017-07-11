#pragma once
typedef struct messyReply
{
	char replyCode;
}MESSY_REPLY, *PMESSY_REPLY;

typedef struct ReplyJoe
{
	FILTER_REPLY_HEADER header;
	MESSY_REPLY reply;
}RJOE, *PRJOE;

