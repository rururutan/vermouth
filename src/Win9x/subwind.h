
#if defined(SUPPORT_KEYDISP)
BOOL kdispwin_initialize(void);
BOOL kdispwin_deinitialize(void);
void kdispwin_create(void);
void kdispwin_destroy(void);
HWND kdispwin_gethwnd(void);
void kdispwin_draw(BYTE cnt);
void kdispwin_readini(void);
void kdispwin_writeini(void);
#else
#define kdispwin_initialize()	(SUCCESS)
#define kdispwin_deinitialize()	(SUCCESS)
#define	kdispwin_create()
#define	kdispwin_destroy()
#define	kdispwin_gethwnd()		(NULL)
#define	kdispwin_draw(c)
#define	kdispwin_readini()
#define	kdispwin_writeini()
#endif

