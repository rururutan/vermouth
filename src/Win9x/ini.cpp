#include	"compiler.h"
#include	<windowsx.h>
#include	<io.h>
#include	"strres.h"
#include	"profile.h"
#include	"dllmain.h"
#if defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif
#include	"dosio.h"
#include	"ini.h"


// ---- Use WinAPI

#if !defined(_UNICODE)
static void bitmapset(UINT8 *ptr, UINT pos, BOOL set) {

	UINT8	bit;

	ptr += (pos >> 3);
	bit = 1 << (pos & 7);
	if (set) {
		*ptr |= bit;
	}
	else {
		*ptr &= ~bit;
	}
}

static BOOL bitmapget(const UINT8 *ptr, UINT pos) {

	return((ptr[pos >> 3] >> (pos & 7)) & 1);
}

static void binset(UINT8 *bin, UINT binlen, const OEMCHAR *src) {

	UINT	i;
	UINT8	val;
	BOOL	set;
	OEMCHAR	c;

	for (i=0; i<binlen; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		bin[i] = val;
	}
}

static void binget(OEMCHAR *work, int size, const UINT8 *bin, UINT binlen) {

	UINT	i;
	OEMCHAR	tmp[8];

	if (binlen) {
		OEMSPRINTF(tmp, OEMTEXT("%.2x"), bin[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<binlen; i++) {
		OEMSPRINTF(tmp, OEMTEXT(" %.2x"), bin[i]);
		milstr_ncat(work, tmp, size);
	}
}

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

const PFTBL	*p;
const PFTBL	*pterm;
	OEMCHAR	work[512];
	UINT32	val;

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		switch(p->itemtype & PFTYPE_MASK) {
			case PFTYPE_STR:
				GetPrivateProfileString(title, p->item, (OEMCHAR *)p->value,
											(OEMCHAR *)p->value, p->arg, path);
				break;

			case PFTYPE_BOOL:
				GetPrivateProfileString(title, p->item,
									(*((UINT8 *)p->value))?str_true:str_false,
												work, NELEMENTS(work), path);
				*((UINT8 *)p->value) = (!milstr_cmp(work, str_true))?1:0;
				break;

			case PFTYPE_BITMAP:
				GetPrivateProfileString(title, p->item,
					(bitmapget((UINT8 *)p->value, p->arg))?str_true:str_false,
												work, NELEMENTS(work), path);
				bitmapset((UINT8 *)p->value, p->arg,
										(milstr_cmp(work, str_true) == 0));
				break;

			case PFTYPE_BIN:
				GetPrivateProfileString(title, p->item, str_null,
												work, NELEMENTS(work), path);
				binset((UINT8 *)p->value, p->arg, work);
				break;

			case PFTYPE_SINT8:
			case PFTYPE_UINT8:
				val = (UINT8)GetPrivateProfileInt(title, p->item,
												*(UINT8 *)p->value, path);
				*(UINT8 *)p->value = (UINT8)val;
				break;

			case PFTYPE_SINT16:
			case PFTYPE_UINT16:
				val = (UINT16)GetPrivateProfileInt(title, p->item,
												*(UINT16 *)p->value, path);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case PFTYPE_SINT32:
			case PFTYPE_UINT32:
				val = (UINT32)GetPrivateProfileInt(title, p->item,
												*(UINT32 *)p->value, path);
				*(UINT32 *)p->value = (UINT32)val;
				break;

			case PFTYPE_HEX8:
				OEMSPRINTF(work, str_x, *(UINT8 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, NELEMENTS(work), path);
				val = (UINT8)milstr_solveHEX(work);
				*(UINT8 *)p->value = (UINT8)val;
				break;

			case PFTYPE_HEX16:
				OEMSPRINTF(work, str_x, *(UINT16 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, NELEMENTS(work), path);
				val = (UINT16)milstr_solveHEX(work);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case PFTYPE_HEX32:
				OEMSPRINTF(work, str_x, *(UINT32 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, NELEMENTS(work), path);
				val = (UINT32)milstr_solveHEX(work);
				*(UINT32 *)p->value = (UINT32)val;
				break;
		}
		p++;
	}
}

void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

const PFTBL		*p;
const PFTBL		*pterm;
const OEMCHAR	*set;
	OEMCHAR		work[512];

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & PFFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					set = (OEMCHAR *)p->value;
					break;

				case PFTYPE_BOOL:
					set = (*((UINT8 *)p->value))?str_true:str_false;
					break;

				case PFTYPE_BITMAP:
					set = (bitmapget((UINT8 *)p->value, p->arg))?
														str_true:str_false;
					break;

				case PFTYPE_BIN:
					binget(work, NELEMENTS(work), (UINT8 *)p->value, p->arg);
					break;

				case PFTYPE_SINT8:
					OEMSPRINTF(work, str_d, *((SINT8 *)p->value));
					break;

				case PFTYPE_SINT16:
					OEMSPRINTF(work, str_d, *((SINT16 *)p->value));
					break;

				case PFTYPE_SINT32:
					OEMSPRINTF(work, str_d, *((SINT32 *)p->value));
					break;

				case PFTYPE_UINT8:
					OEMSPRINTF(work, str_u, *((UINT8 *)p->value));
					break;

				case PFTYPE_UINT16:
					OEMSPRINTF(work, str_u, *((UINT16 *)p->value));
					break;

				case PFTYPE_UINT32:
					OEMSPRINTF(work, str_u, *((UINT32 *)p->value));
					break;

				case PFTYPE_HEX8:
					OEMSPRINTF(work, str_x, *((UINT8 *)p->value));
					break;

				case PFTYPE_HEX16:
					OEMSPRINTF(work, str_x, *((UINT16 *)p->value));
					break;

				case PFTYPE_HEX32:
					OEMSPRINTF(work, str_x, *((UINT32 *)p->value));
					break;

				default:
					set = NULL;
					break;
			}
			if (set) {
				WritePrivateProfileString(title, p->item, set, path);
			}
		}
		p++;
	}
}

#else	// !defined(_UNICODE)

// ---- Use profile.c

static void pfread(const PFTBL *item, const OEMCHAR *string) {

	switch(item->itemtype & PFTYPE_MASK) {
		case PFTYPE_ARGS16:
			inirdargs16(string, item);
			break;

		case PFTYPE_BYTE3:
			inirdbyte3(string, item);
			break;

		case PFTYPE_KB:
			inirdkb(string, item);
			break;
	}
}

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

	profile_iniread(path, title, tbl, count, pfread);
}

void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

	profile_iniwrite(path, title, tbl, count, NULL);
}

#endif	// !defined(_UNICODE)


// ----

static const OEMCHAR ext_ini[] = OEMTEXT(".ini");

void initgetfile(OEMCHAR *path, UINT size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, ext_ini, size);
}

