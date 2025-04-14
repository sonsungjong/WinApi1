#ifndef LZR920_DEFINE_H_
#define LZR920_DEFINE_H_

#define BAUD_RATE								460800
#define HEADER_SYNC_VAL			0xFFFEFDFC

#define BUFFER_SIZE								65507
#define MIN_MSG_SIZE							8					// SYNC(4) + SIZE(2) + FOOTER(2)
#define HEADER_SYNC_SIZE					4
#define HEADER_MESSAGE_SIZE			2
#define MESSAGE_CMD_SIZE					2
#define FOOTER_CHK_SIZE					2


#endif
