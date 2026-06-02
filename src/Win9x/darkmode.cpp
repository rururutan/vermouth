#if WINVER >= 0xa00

#include "compiler.h"
#include "darkmode.h"
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE  20
#endif

static void darkmode_refreshtitlebar(HWND _hWnd, BOOL _enable)
{
	BOOL dark = _enable;
	DwmSetWindowAttribute(_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
}

static BOOL darkmode_check()
{
	DWORD light = 1;
	DWORD size = sizeof(light);
	LSTATUS s = RegGetValueW(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"AppsUseLightTheme",
		RRF_RT_REG_DWORD, NULL, &light, &size);
	if (s != ERROR_SUCCESS) return FALSE;
	return light == 0;
}

static BOOL darkmode_ishighcontrast()
{
	HIGHCONTRASTW highContrast = { sizeof(highContrast) };
	if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
		return highContrast.dwFlags & HCF_HIGHCONTRASTON;
	return FALSE;
}

BOOL darkmode_set( HWND _hWnd )
{
	if (darkmode_ishighcontrast()) return FALSE;

	const BOOL darkMode = darkmode_check();

	// Change Titlebar
	darkmode_refreshtitlebar(_hWnd, darkMode);

	// System Menu Theme切替不具合回避
	// https://mntone.hateblo.jp/entry/2020/07/27/172228
//	if (_SetPreferredAppMode) {
//		_SetPreferredAppMode(ForceDark);
//		_RefreshImmersiveColorPolicyState();
//		if (!darkMode) {
//			_SetPreferredAppMode(ForceLight);
//			_RefreshImmersiveColorPolicyState();
//		}
//	}

	// Change Menu Theme
//	if (_FlushMenuThemes) (*_FlushMenuThemes)();
//	RedrawWindow(_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

	return TRUE;
}

#endif // WINVER
