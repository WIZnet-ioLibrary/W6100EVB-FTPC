#ifndef _FTPC_H_
#define _FTPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "Application.h"
#include "socket.h"

#define F_APP_FTPC

/* If your target support a file system, you have to activate this feature and implement. */
//#define F_FILESYSTEM

/* Change to your Chipset Uart function, you have to activate this feature and implement.
 * Change!! -> Board_UARTGetCharBlocking()
 * Below is an example of a function of lpc_chip library. */
#define ftp_getc()  _getchar_b()


#ifndef	ftp_getc()
#define Need_UARTGetCharBlocking_func
#else
/* Change library
 * Change!! -> board_api.h,
 * Below is an example of a function of lpc_chip library. */
//#include "board_api.h"
#endif


#define _FTP_FILENAME_LEN_      100
#define _FTP_MAX_MSG_SIZE_      512
#define SOCK_MAX_NUM            8

/* FTP Responses */
#define R_125	125		/* Data connection already open; transfer starting */
#define R_150	150		/* File status ok; opening data conn */
#define R_200	200		/* 'Generic' command ok */
#define R_220	220		/* Service ready for new user. */
#define R_226	226		/* Closing data connection.  File transfer/abort successful */
#define R_227	227		/* Entering passive mode (h1,h2,h3,h4,p1,p2) */
#define R_229	229		/* Entering extended passive mode (|||port|) */
#define R_230	230		/* User logged in, proceed */
#define R_331	331		/* User name okay, need password. */

#define TransferAscii       'A'
#define TransferBinary      'I'

enum ftpc_datasock_state{
    DATASOCK_IDLE,
    DATASOCK_READY,
    DATASOCK_START
};

enum Commandtype {
    cmd_nocmd,
    cmd_dir,
    cmd_put,
    cmd_get,
};

struct Command {
    enum Commandtype First;
    enum Commandtype Second;
};

struct ftpc {
    uint8_t     control;			/* FTP Control Socket */
	uint8_t     data;				/* FTP Data Socket */
	uint8_t     as_type;  			/* FTP IP version */
	uint8_t     destip[16];
	uint16_t    destport;
	uint8_t     is_ascii;		/* Transfer Type 1 : ASCII, 0 : Binary*/
	uint8_t     is_active;      /* Transfer Type 1 : ASCII, 0 : Binary*/
	enum ftpc_datasock_state dsock_state;
	char workingdir[_FTP_FILENAME_LEN_];
	char filename[_FTP_FILENAME_LEN_];
};


uint8_t ftpc_init(uint8_t * FTP_destip, uint16_t FTP_destport, uint8_t astype);
uint8_t ftpc_run(uint8_t * dbuf);
uint8_t proc_ftpc(char * buf);
int pportc(char * arg);
int pportc_extend(char * arg);
uint8_t* User_Keyboard_MSG();
void ftpc_ready(uint8_t* dbuf);
void ftpc_menu(uint8_t *dbuf);
int ftpc_putget(uint8_t* dbuf);


#ifdef __cplusplus
}
#endif

#endif // _FTPC_H_
