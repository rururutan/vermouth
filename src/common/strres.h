
#ifdef __cplusplus
extern "C" {
#endif

extern const UINT8 str_utf8[3];
extern const UINT16 str_ucs2[1];

extern const OEMCHAR str_null[];
extern const OEMCHAR str_space[];
extern const OEMCHAR str_dot[];

extern const OEMCHAR str_cr[];
extern const OEMCHAR str_crlf[];
#define	str_lf	(str_crlf + 1)

#if defined(OSLINEBREAK_CR)
#define	str_oscr	str_cr
#elif defined(OSLINEBREAK_CRLF)
#define	str_oscr	str_crlf
#else
#define	str_oscr	str_lf
#endif

extern const OEMCHAR str_d[];
extern const OEMCHAR str_u[];
extern const OEMCHAR str_x[];
extern const OEMCHAR str_2d[];
extern const OEMCHAR str_2x[];
extern const OEMCHAR str_4x[];
extern const OEMCHAR str_4X[];

extern const OEMCHAR str_false[];
extern const OEMCHAR str_true[];

#ifdef __cplusplus
}
#endif

