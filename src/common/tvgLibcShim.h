/*
 * tvgLibcShim.h — bare-metal libc replacements (declarations)
 *
 * Force-included into every translation unit on bare-metal builds
 * (`target_os == "none"`) by the thorvg-sys build.rs via
 * `cc::Build::flag("-include …/tvgLibcShim.h")`.
 *
 * Why this shape (declarations only, no macros):
 *
 *   Earlier iterations tried to redirect names like `isspace`,
 *   `atoi`, `strtol` via `#define`.  That breaks C++: libstdc++'s
 *   `<cstdlib>` / `<cctype>` declare those names with `using ::name;`
 *   inside `namespace std`, and any prior `#define name(…) …`
 *   rewrites the `name` token *inside* those `using` declarations,
 *   causing compile errors.  The C++ standard reserves these names
 *   exactly for this reason (`[macro.names]`).
 *
 *   Instead, this header makes two minimally-invasive moves:
 *
 *     1. Trip newlib's `<ctype.h>` include guard (`_CTYPE_H_` /
 *        `_CTYPE_H`).  Newlib is the only header in our compile
 *        path that defines libc function names as macros referencing
 *        a libc.a-resident lookup table (`_ctype_`).  By short-
 *        circuiting that header before any TU can include it, the
 *        macros never get a chance to fire, and `_ctype_` never
 *        becomes an undefined reference.
 *
 *     2. Declare the ctype functions ourselves, in the global
 *        namespace, with C linkage.  TUs that would have included
 *        `<ctype.h>` for these prototypes get them from here
 *        instead.  Implementations live in `tvgLibcShim.cpp` with
 *        weak linkage so consumer overrides win.
 *
 *   `<stdlib.h>` and `<string.h>` are *not* short-circuited — they
 *   declare names libstdc++ depends on (`std::malloc`, `std::strlen`,
 *   etc.).  The shim's `atoi` / `strtol` / `str*` definitions in
 *   `tvgLibcShim.cpp` match those system declarations and satisfy
 *   the link without any header tricks.
 *
 * Hosted builds do not include this header.
 */

#ifndef _TVG_LIBC_SHIM_H_
#define _TVG_LIBC_SHIM_H_

/* Short-circuit newlib's <ctype.h>.  Both guard spellings observed in
 * the wild: `_CTYPE_H_` (newlib, BSD) and `_CTYPE_H` (glibc, musl). */
#define _CTYPE_H_ 1
#define _CTYPE_H  1

#ifdef __cplusplus
extern "C" {
#endif

/* Implemented in tvgLibcShim.cpp (weak).  thorvg only calls the
 * first nine; the rest exist purely so `<cctype>`'s `using ::name;`
 * declarations resolve — libstdc++ pulls all of them in via
 * `<bits/localefwd.h>` even when none are used.  Unreferenced
 * symbols don't get pulled at link time, so the cpp doesn't need to
 * define them. */
int isspace(int c);
int isdigit(int c);
int isalpha(int c);
int isalnum(int c);
int isxdigit(int c);
int isupper(int c);
int islower(int c);
int tolower(int c);
int toupper(int c);
int iscntrl(int c);
int isgraph(int c);
int isprint(int c);
int ispunct(int c);
int isblank(int c);

#ifdef __cplusplus
}
#endif

#endif /* _TVG_LIBC_SHIM_H_ */
