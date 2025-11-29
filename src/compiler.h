// compiler.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

typedef signed int			SINT;
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef signed short		SINT16;
typedef unsigned short		UINT16;
typedef signed int			SINT32;
typedef unsigned int		UINT32;
typedef signed __int64		SINT64;
typedef unsigned __int64	UINT64;

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include "misc\vc6macros.h"
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;

#define BRESULT				BOOL
#define OEMCHAR				TCHAR
