
#define	_WIN32_IE	0x0200

#include	<windows.h>
#include	<tchar.h>
#include	<stdio.h>
#include	<stddef.h>
#include	<setjmp.h>
#if defined(TRACE)
#include	<assert.h>
#endif

#define	BYTESEX_LITTLE
#if !defined(OSLANG_UTF8)
#if !defined(_UNICODE)
#define	OSLANG_SJIS
#else
#define	OSLANG_UCS2
#endif
#endif
#define	OSLINEBREAK_CRLF

#ifndef __GNUC__
typedef	signed int			SINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
typedef	signed __int64		SINT64;
typedef	unsigned __int64	UINT64;
#define	INLINE				__inline
#define	QWORD_CONST(v)		((UINT64)(v))
#define	SQWORD_CONST(v)		((SINT64)(v))
#define	snprintf			_snprintf
#define	vsnprintf			_vsnprintf
#else
#include	<stdlib.h>
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	signed __int64		SINT64;
#define	INLINE				inline
#endif
#define	FASTCALL			__fastcall

// for x86
#define	LOADINTELDWORD(a)		(*((UINT32 *)(a)))
#define	LOADINTELWORD(a)		(*((UINT16 *)(a)))
#define	STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define	STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)

#define	STRCALL		__stdcall

#define	BRESULT				UINT8
#if defined(OSLANG_UCS2)
#define	OEMCHAR				wchar_t
#define	_OEMTEXT(x)			L ## x
#define	OEMTEXT(string)		_OEMTEXT(string)
#if defined(_UNICODE)
#define	OEMSPRINTF			wsprintf
#define	OEMSTRLEN			lstrlen
#else
#define	OEMSPRINTF			swprintf
#define	OEMSTRLEN			wcslen
#endif
#else
#define	OEMCHAR				char
#define	OEMTEXT(string)		string
#if defined(_UNICODE)
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen
#else
#define	OEMSPRINTF			wsprintf
#define	OEMSTRLEN			lstrlen
#endif
#endif


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()			GetTickCount()
#if defined(TRACE)
#define	__ASSERT(s)			assert(s)
#else
#define	__ASSERT(s)
#endif
#if defined(_UNICODE)
#define	SPRINTF				sprintf
#define	STRLEN				strlen
#else
#define	SPRINTF				wsprintf
#define	STRLEN				lstrlen
#endif

#define	LABEL				__declspec(naked)
#define	RELEASE(x) 			if (x) {(x)->Release(); (x) = NULL;}

#if defined(OSLANG_SJIS)
#define	SUPPORT_SJIS
#elif defined(OSLANG_UTF8)
#define	SUPPORT_UTF8
#else
#define	SUPPORT_ANK
#endif

#define	SUPPORT_8BPP
#define	SUPPORT_16BPP
#define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define	SUPPORT_KEYDISP
#define	SUPPORT_TEXTCNV
#define	SUPPORT_ARC
#define	SUPPORT_ZLIB

