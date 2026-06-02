#include	"compiler.h"
#include	"dllmain.h"
#include	"winloc.h"
#include	"ini.h"
#include	"subwind.h"
#include	"keydisp.h"
#include	"darkmode.h"


// ---- key display

#if defined(SUPPORT_KEYDISP)

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	HDC			hdc;
	HBITMAP		dib;
	HBITMAP		oldbmp;
	CMNVRAM		vram;
	UINT		dpi;
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


static UINT32 kdgetpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] & 0xffffff);
	}
	return(0);
}

static void kddrawkeys(HDC hdc, BOOL redraw) {

	keydisp_paint(&kdispwin.vram, TRUE);

	if (kdispwin.dpi == 96) {
		BitBlt(
		hdc,
		0,
		0,
		KEYDISP_WIDTH,
		KEYDISP_HEIGHT,
		kdispwin.hdc,
		0,
		0,
		SRCCOPY);
	} else {
		StretchBlt(
		hdc,
		0,
		0,
		KEYDISP_WIDTH * kdispwin.dpi / 96,
		KEYDISP_HEIGHT * kdispwin.dpi / 96,
		kdispwin.hdc,
		0,
		0,
		KEYDISP_WIDTH,
		KEYDISP_HEIGHT,
		SRCCOPY);
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
	winloc_setclientsize(kdispwin.hwnd, width * kdispwin.dpi / 96, height * kdispwin.dpi / 96);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

static void kdpaintmsg(HWND hWnd) {

	PAINTSTRUCT	ps;

	BeginPaint(hWnd, &ps);
	kddrawkeys(ps.hdc, TRUE);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK kdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			SetTimer(hWnd, ID_KDTIMER, 33, NULL);
			break;

		case WM_TIMER:
		{
			UINT8 flag;

			flag = keydisp_process(2);

			if (flag & KEYDISP_FLAGSIZING) {
				kdsetwinsize();
			}

			if (flag & (KEYDISP_FLAGDRAW | KEYDISP_FLAGREDRAW)) {
				InvalidateRect(hWnd, NULL, FALSE);
			}
		}
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
			if (kdispwin.hdc) {
				SelectObject(kdispwin.hdc, kdispwin.oldbmp);
				DeleteObject(kdispwin.dib);
				DeleteDC(kdispwin.hdc);
			}
			kdispwin.hwnd = NULL;
			break;

		case WM_SIZE:
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case WM_ERASEBKGND:
			return 1;

#if WINVER >= 0xa00
		case WM_SETTINGCHANGE:
			if (lp && lstrcmp((wchar_t const*)lp, L"ImmersiveColorSet") == 0)
			{
				darkmode_set(hWnd);
				SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
			}
			break;

		case WM_DPICHANGED:
			{
				kdispwin.dpi = HIWORD(wp);
				InvalidateRect(hWnd, NULL, TRUE);

				RECT* rc = (RECT*)lp;
				SetWindowPos(
					hWnd,
					NULL,
					rc->left,
					rc->top,
					rc->right - rc->left,
					rc->bottom - rc->top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
			break;
#endif // WINVER

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}

BOOL kdispwin_initialize(void) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style =  CS_DBLCLKS;
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

#if WINVER >= 0xa00
	DPI_AWARENESS_CONTEXT oldContext = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif // WINVER
	ZeroMemory(&kdispwin, sizeof(kdispwin));
	hwnd = CreateWindow(kdispclass, title,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						kdispcfg.posx, kdispcfg.posy,
						KEYDISP_WIDTH, KEYDISP_HEIGHT,
						NULL, NULL, hInst, NULL);
#if WINVER >= 0xa00
	SetThreadDpiAwarenessContext(oldContext);
#endif // WINVER
	kdispwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto kdcre_err1;
	}
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);

#if WINVER >= 0xa00
	darkmode_set(hwnd);
	kdispwin.dpi = GetDpiForWindow(hwnd);
#else	// WINVER
	kdispwin.dpi = 96;
#endif // WINVER

	BITMAPINFO bmi;
	void *bits;

	ZeroMemory(&bmi, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = KEYDISP_WIDTH;
	bmi.bmiHeader.biHeight = -KEYDISP_HEIGHT;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	kdispwin.hdc = CreateCompatibleDC(NULL);

	kdispwin.dib = CreateDIBSection(
		kdispwin.hdc,
		&bmi,
		DIB_RGB_COLORS,
		&bits,
		NULL,
		0);

	kdispwin.oldbmp = (HBITMAP)SelectObject(kdispwin.hdc, kdispwin.dib);
	if (kdispwin.oldbmp == NULL) {
		goto kdcre_err2;
	}

	kdispwin.vram.ptr = (UINT8 *)bits;
	kdispwin.vram.width = KEYDISP_WIDTH;
	kdispwin.vram.height = KEYDISP_HEIGHT;

	kdispwin.vram.bpp = 32;
	kdispwin.vram.xalign = 4;
	kdispwin.vram.yalign = KEYDISP_WIDTH * 4;

	palfn.get8 = NULL;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = NULL;
	palfn.userdata = (long)NULL;
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
#endif // SUPPORT_KEYDISP

