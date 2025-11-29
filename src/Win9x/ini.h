
#include	"profile.h"

#ifdef __cplusplus
extern "C" {
#endif

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count);
void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count);


void initgetfile(OEMCHAR *path, UINT size);

#ifdef __cplusplus
}
#endif

