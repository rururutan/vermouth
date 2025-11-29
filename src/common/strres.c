#include	"compiler.h"
#include	"strres.h"


const UINT8 str_utf8[3] = {0xef, 0xbb, 0xbf};
const UINT16 str_ucs2[1] = {0xfeff};


const OEMCHAR str_null[] = OEMTEXT("");
const OEMCHAR str_space[] = OEMTEXT(" ");
const OEMCHAR str_dot[] = OEMTEXT(".");

const OEMCHAR str_cr[] = OEMTEXT("\r");
const OEMCHAR str_crlf[] = OEMTEXT("\r\n");

const OEMCHAR str_d[] = OEMTEXT("%d");
const OEMCHAR str_u[] = OEMTEXT("%u");
const OEMCHAR str_x[] = OEMTEXT("%x");
const OEMCHAR str_2d[] = OEMTEXT("%.2d");
const OEMCHAR str_2x[] = OEMTEXT("%.2x");
const OEMCHAR str_4x[] = OEMTEXT("%.4x");
const OEMCHAR str_4X[] = OEMTEXT("%.4X");

const OEMCHAR str_false[] = OEMTEXT("false");
const OEMCHAR str_true[] = OEMTEXT("true");

