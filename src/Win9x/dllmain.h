
#define	DLLAPI		__declspec(dllexport)

extern	HWND		hWndMain;
extern	HINSTANCE	hInst;
extern	OEMCHAR		modulefile[MAX_PATH];

#ifdef __cplusplus
extern "C" {
#endif

DLLAPI int WINAPI vermouth_Open(UINT sample_rate);
DLLAPI int WINAPI vermouth_Close(void);
DLLAPI int WINAPI vermouth_Write(UINT8 data);
DLLAPI int WINAPI vermouth_ShortMsg(UINT32 data);
DLLAPI int WINAPI vermouth_LongMsg(const UINT8 *sysex, UINT32 len);
DLLAPI int WINAPI vermouth_Get16(SINT16 *pcm, UINT32 size);
DLLAPI int WINAPI vermouth_Get32(SINT32 *pcm, UINT32 size);

DLLAPI HWND WINAPI vermouth_SetParent(HWND hWnd);


// ----



#ifdef __cplusplus
}
#endif

