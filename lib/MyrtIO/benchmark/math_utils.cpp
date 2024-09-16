#include "math_utils.h"

#define ELEM_SWAP(a, b)         \
    {                           \
        register float t = (a); \
        (a) = (b);              \
        (b) = t;                \
    }

uint32_t calcMedianWirth(uint32_t a[], uint8_t n, uint8_t k) {
    int i, j, l, m;
    int x;
    l = 0;
    m = n - 1;
    while (l < m) {
        x = a[k];
        i = l;
        j = m;
        do {
            while (a[i] < x)
                i++;
            while (x < a[j])
                j--;
            if (i <= j) {
                ELEM_SWAP(a[i], a[j]);
                i++;
                j--;
            }
        } while (i <= j);
        if (j < k)
            l = i;
        if (k < i)
            m = j;
    }
    return a[k];
}
