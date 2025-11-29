
#ifndef TRACE

#define	TRACEINIT()
#define	TRACETERM()
#define	TRACEOUT(a)
#define	VERBOSE(a)
#define	APPDEVOUT(a)

#else

#ifdef __cplusplus
extern "C" {
#endif

void trace_initialize(void);
void trace_deinitialize(void);
void trace_fmt(const char *str, ...);
void trace_fmt2(const char *str, ...);
void trace_char(char c);
void trace_fileout(const OEMCHAR *fname);

#ifdef __cplusplus
}
#endif

#define	TRACEINIT()		trace_initialize()
#define	TRACETERM()		trace_deinitialize()
#define	TRACEOUT(arg)	trace_fmt arg
#define	VERBOSE(arg)	trace_fmt2 arg
#define	APPDEVOUT(arg)	trace_char(arg)

#endif

