#include "fe.h"

void fe_invert(fe out,const fe z)
{
    fe t0;
    fe t1;
    fe t2;
    fe t3;
    int i;

    fe_sq(t0, z);
    for (i = 1; i < 1; ++i) {
        fe_sq(t0, t0);
    }
    fe_sq(t1, t0);
    for (i = 1; i < 2; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(t1, z, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t2, t0);
    for (i = 1; i < 1; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t1, t2);
    fe_sq(t2, t1);
    for (i = 1; i < 5; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t2, t1);
    for (i = 1; i < 10; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t2, t2, t1);
    fe_sq(t3, t2);
    for (i = 1; i < 20; ++i) {
        fe_sq(t3, t3);
    }
    fe_mul(t2, t3, t2);
    fe_sq(t2, t2);
    for (i = 1; i < 10; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t2, t1);
    for (i = 1; i < 50; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t2, t2, t1);
    fe_sq(t3, t2);
    for (i = 1; i < 100; ++i) {
        fe_sq(t3, t3);
    }
    fe_mul(t2, t3, t2);
    fe_sq(t2, t2);
    for (i = 1; i < 50; ++i) {
        fe_sq(t2, t2);
    }
    fe_mul(t1, t2, t1);
    fe_sq(t1, t1);
    for (i = 1; i < 5; ++i) {
        fe_sq(t1, t1);
    }
    fe_mul(out, t1, t0);
}
