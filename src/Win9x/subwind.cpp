#include	"compiler.h"
#include	"dllmain.h"
#include	"winloc.h"
#include	"ini.h"
#include	"dd2.h"
#include	"subwind.h"
#include	"keydisp.h"


// ---- key display

#if defined(SUPPORT_KEYDISP)

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
} KDISPWIN;

typedef struct {
	int		posx;
	int		posy;
} KDISPCFG;

#define	ID_KDTIMER	3000

static	KDISPWIN	kdispwin;
static	KDISPCFG	kdispcfg;

static const TCHAR kdisptitle[] = _T("Vermouth");
static const TCHAR kdispclass[] = _T("VermouthKeyDispWin");

static const UINT32 kdisppal[KEYDISP_PALS] =
									{0x00000000, 0xffffffff, 0xf9ff0000};

#if defined(OSLANG_UTF8)
static const OEMCHAR kdispapp[] = OEMTEXT("Key Display");
#else
#define	kdispapp	kdisptitle
#endif
static const PFTBL kdispini[] = {
				PFVAL("WindposX", PFTYPE_SINT32,	&kdispcfg.posx),
				PFVAL("WindposY", PFTYPE_SINT32,	&kdispcfg.posy)};


static UINT8 kdgetpal8(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] >> 24);
	}
	return(0);
}

static UINT32 kdgetpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] & 0xffffff);
	}
	return(0);
}

static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32) {

	return(dd2_get16pal((DD2HDL)self->userdata, pal32));
}

static void kddrawkeys(HWND hWnd, BOOL redraw) {

	RECT	rect;
	RECT	draw;
	CMNVRAM	*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(KEYDISP_WIDTH, rect.right - rect.left);
	draw.bottom = min(KEYDISP_HEIGHT, rect.bottom - rect.top);
	if ((draw.right <= 0) || (draw.bottom <= 0)) {
		return;
	}
	vram = dd2_bsurflock(kdispwin.dd2hdl);
	if (vram) {
		keydisp_paint(vram, redraw);
		dd2_bsurfunlock(kdispwin.dd2hdl);
		dd2_blt(kdispwin.dd2hdl, NULL, &draw);
	}
}

static WINLOCEX winlocexallwin(HWND base) {

	UINT	cnt;
	HWND	list[2];
	UINT	i;

	cnt = 0;
	list[cnt++] = hWndMain;
	list[cnt++] = kdispwin.hwnd;
	for (i=0; i<cnt; i++) {
		if (list[i] == base) {
			list[i] = NULL;
		}
	}
	if (base != hWndMain) {
		base = NULL;
	}
	return(winlocex_create(base, list, cnt));
}

static void kdsetwinsize(void) {

	int			width;
	int			height;
	WINLOCEX	wlex;

	wlex = winlocexallwin(hWndMain);
	winlocex_setholdwnd(wlex, kdispwin.hwnd);
	keydisp_getsize(&width, &height);
	winloc_setclientsize(kdispwin.hwnd, width, height);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

static void kdpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	kddrawkeys(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK kdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			SetTimer(hWnd, ID_KDTIMER, 33, NULL);
			break;

		case WM_TIMER:
			kdispwin_draw(2);
			break;

		case WM_COMMAND:
			break;

		case WM_PAINT:
			kdpaintmsg(hWnd);
			break;
#if 0
		case WM_ACTIVATE:
			if (LOWORD(wp) != WA_INACTIVE) {
				keydisps_reload();
				kddrawkeys(hWnd, TRUE);
			}
			break;
#endif
		case WM_ENTERSIZEMOVE:
			winlocex_destroy(kdispwin.wlex);
			kdispwin.wlex = winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(kdispwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(kdispwin.wlex);
			kdispwin.wlex = NULL;
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				kdispcfg.posx = rc.left;
				kdispcfg.posy = rc.top;
			}
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			if (hWndMain) {
				PostMessage(hWndMain, msg, wp, lp);
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			KillTimer(hWnd, ID_KDTIMER);
			dd2_release(kdispwin.dd2hdl);
			kdispwin.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}

BOOL kdispwin_initialize(void) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = kdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = kdispclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	keydisp_initialize();
	return(SUCCESS);
}

BOOL kdispwin_deinitialize(void) {

	kdispwin_destroy();
	UnregisterClass(kdispclass, hInst);
	return(SUCCESS);
}

void kdispwin_create(void) {

	const TCHAR	*title;
	TCHAR		_title[256];
	HWND		hwnd;
	CMNPALFN	palfn;

	if (kdispwin.hwnd != NULL) {
		return;
	}
	title = kdisptitle;
	if (hWndMain) {
		UINT pos = NELEMENTS(kdisptitle) - 1;
		CopyMemory(_title, kdisptitle, pos * sizeof(TCHAR));
		_title[pos + 0] = ' ';
		_title[pos + 1] = '-';
		_title[pos + 2] = ' ';
		pos += 3;
		GetWindowText(hWndMain, _title + pos, NELEMENTS(_title) - pos);
		title = _title;
	}
	ZeroMemory(&kdispwin, sizeof(kdispwin));
	hwnd = CreateWindow(kdispclass, title,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						kdispcfg.posx, kdispcfg.posy,
						KEYDISP_WIDTH, KEYDISP_HEIGHT,
						NULL, NULL, hInst, NULL);
	kdispwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto kdcre_err1;
	}
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	kdispwin.dd2hdl = dd2_create(hwnd, KEYDISP_WIDTH, KEYDISP_HEIGHT);
	if (kdispwin.dd2hdl == NULL) {
		goto kdcre_err2;
	}
	palfn.get8 = kdgetpal8;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = kdcnvpal16;
	palfn.userdata = (long)kdispwin.dd2hdl;
	keydisp_setpal(&palfn);
	kdispwin_draw(0);
	if (hWndMain) SetForegroundWindow(hWndMain);
	return;

kdcre_err2:
	DestroyWindow(hwnd);

kdcre_err1:
	return;
}

void kdispwin_destroy(void) {

	if (kdispwin.hwnd != NULL) {
		DestroyWindow(kdispwin.hwnd);
	}
}

void kdispwin_draw(UINT8 cnt) {

	UINT8	flag;

	if (kdispwin.hwnd) {
		if (!cnt) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			kdsetwinsize();
		}
		kddrawkeys(kdispwin.hwnd, FALSE);
	}
}

void kdispwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = CW_USEDEFAULT;
	kdispcfg.posy = CW_USEDEFAULT;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, kdispapp, kdispini, NELEMENTS(kdispini));
}

void kdispwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, kdispapp, kdispini, NELEMENTS(kdispini));
}
#endif

