#ifndef JASMIN_INTERNAL_ATTRIBUTES
#define JASMIN_INTERNAL_ATTRIBUTES

// JASMIN_TAIL_CALL is an attribute indicating that the annotated return
// statement must be tail-call optimized.
#if defined(__clang__)
#define JASMIN_INTERNAL_TAIL_CALL [[clang::musttail]]
#elif defined(__GNUC__)
#define JASMIN_INTERNAL_TAIL_CALL __attribute__((musttail))
#else
#error "Jasmin requires enforced tail-call optimization to function properly."
#endif

#endif  // JASMIN_INTERNAL_ATTRIBUTES
