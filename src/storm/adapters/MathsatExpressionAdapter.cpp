#include "src/storm/adapters/MathsatExpressionAdapter.h"

#ifdef STORM_HAVE_MSAT
bool operator==(msat_decl decl1, msat_decl decl2) {
    return decl1.repr == decl2.repr;
}
#endif
