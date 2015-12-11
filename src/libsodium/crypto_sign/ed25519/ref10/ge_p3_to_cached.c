#include "ge.h"

/*
r = p
*/

static const fe d2 = {
    -21827239,-5839606,-30745221,13898782,229458,15978800,-12551817,-6495438,29715968,9444199
};

extern void ge_p3_to_cached(ge_cached *r,const ge_p3 *p)
{
  fe_add(r->YplusX,p->Y,p->X);
  fe_sub(r->YminusX,p->Y,p->X);
  fe_copy(r->Z,p->Z);
  fe_mul(r->T2d,p->T,d2);
}
