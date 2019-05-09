/*********************************************************************
                        基本数据类型定义
*********************************************************************/

#ifndef _BASETYPE_H_20110727
#define _BASETYPE_H_20110727

#ifdef _WIN32
#	define PATH_SEPARATOR '\\'
#   define PATH_SEPARATOR2 '/'
#else
#	define PATH_SEPARATOR '/'
#   define PATH_SEPARATOR2 '\\'
#endif

#include <vector>
using std::vector;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef _MSC_VER  
#if _MSC_VER <= 1500
    typedef __int8 int8_t;
    typedef unsigned __int8 uint8_t;
    typedef __int16 int16_t;
    typedef unsigned __int16 uint16_t;
    typedef __int32 int32_t; 
    typedef unsigned __int32 uint32_t; 
    typedef __int64 int64_t; 
    typedef unsigned __int64 uint64_t;  
#endif
#else 
#   include <stdint.h> 
    typedef int BOOL;
#endif

typedef struct _TIME_STRU
{
	uint16_t wYear;
	uint16_t wMonth;
	uint16_t wDayOfWeek;
	uint16_t wDay;
	uint16_t wHour;
	uint16_t wMinute;
	uint16_t wSecond;
	uint16_t wMilliseconds;

} TIME_STRU, *PTIME_STRU, *LPTIME_STRU;
typedef vector<TIME_STRU> STVECTOR;

#ifndef _WIN32

typedef struct _DCB
{
	uint32_t DCBlength;      /* sizeof(DCB)                     */
	uint32_t BaudRate;       /* Baudrate at which running       */
	uint32_t fBinary: 1;     /* Binary Mode (skip EOF check)    */
	uint32_t fParity: 1;     /* Enable parity checking          */
	uint32_t fOutxCtsFlow:1; /* CTS handshaking on output       */
	uint32_t fOutxDsrFlow:1; /* DSR handshaking on output       */
	uint32_t fDtrControl:2;  /* DTR Flow control                */
	uint32_t fDsrSensitivity:1; /* DSR Sensitivity              */
	uint32_t fTXContinueOnXoff: 1; /* Continue TX when Xoff sent */
	uint32_t fOutX: 1;       /* Enable output X-ON/X-OFF        */
	uint32_t fInX: 1;        /* Enable input X-ON/X-OFF         */
	uint32_t fErrorChar: 1;  /* Enable Err Replacement          */
	uint32_t fNull: 1;       /* Enable Null stripping           */
	uint32_t fRtsControl:2;  /* Rts Flow control                */
	uint32_t fAbortOnError:1; /* Abort all reads and writes on Error */
	uint32_t fDummy2:17;     /* Reserved                        */
	uint16_t wReserved;       /* Not currently used              */
	uint16_t XonLim;          /* Transmit X-ON threshold         */
	uint16_t XoffLim;         /* Transmit X-OFF threshold        */
	uint8_t ByteSize;        /* Number of bits/byte, 4-8        */
	uint8_t Parity;          /* 0-4=None,Odd,Even,Mark,Space    */
	uint8_t StopBits;        /* 0,1,2 = 1, 1.5, 2               */
	char XonChar;         /* Tx and Rx X-ON character        */
	char XoffChar;        /* Tx and Rx X-OFF character       */
	char ErrorChar;       /* Error replacement char          */
	char EofChar;         /* End of Input character          */
	char EvtChar;         /* Received Event character        */
	uint16_t wReserved1;      /* Fill for now.                   */

} DCB, *LPDCB;

#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#endif //_WIN32

#define SDDLFLOATZERO 1e-6

#endif