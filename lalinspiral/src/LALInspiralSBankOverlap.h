#include <stdlib.h>
#include <lal/LALAtomicDatatypes.h>
#include <lal/ComplexFFT.h>
#include <lal/FrequencySeries.h>
#include <sys/types.h>

typedef struct tagWS {
    size_t n;
    COMPLEX16FFTPlan *plan;
    COMPLEX16Vector *zf;
    COMPLEX16Vector *zt;
} WS;

WS *XLALCreateSBankWorkspaceCache(void);
void XLALDestroySBankWorkspaceCache(WS *workspace_cache);
REAL8 XLALInspiralSBankComputeMatch(const COMPLEX16FrequencySeries *inj, const COMPLEX16FrequencySeries *tmplt, WS *workspace_cache);

REAL8 XLALInspiralSBankComputeRealMatch(const COMPLEX16FrequencySeries *inj, const COMPLEX16FrequencySeries *tmplt, WS *workspace_cache);

REAL8 XLALInspiralSBankComputeMatchMaxSkyLoc(const COMPLEX16FrequencySeries *hp, const COMPLEX16FrequencySeries *hc, const REAL8 hphccorr, const COMPLEX16FrequencySeries *proposal, WS *workspace_cache1, WS *workspace_cache2);

REAL8 XLALInspiralSBankComputeMatchMaxSkyLocNoPhase(const COMPLEX16FrequencySeries *hp, const COMPLEX16FrequencySeries *hc, const REAL8 hphccorr, const COMPLEX16FrequencySeries *proposal, WS *workspace_cache1, WS *workspace_cache2);
