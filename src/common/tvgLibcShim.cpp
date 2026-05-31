/*
 * tvgLibcShim.cpp — bare-metal libc replacements (str-family)
 *
 * Compiled into thorvg's static library only on bare-metal builds
 * (`target_os == "none"`) by the thorvg-sys build.rs.  Provides
 * implementations of the C `str*` / `memchr` functions thorvg uses so
 * the cross toolchain's libc.a never needs to be linked.
 *
 * Every symbol is `__attribute__((weak))`: when a consumer (HAL,
 * picolibc, custom shim, …) provides its own definition, theirs wins;
 * otherwise ours satisfies the link.  This makes the shim
 * collision-immune in both directions and lets users override
 * individual functions for performance without forking thorvg.
 *
 * Implementations are intentionally simple, byte-wise, ASCII-only —
 * correctness over throughput.  thorvg's call patterns (short
 * tag/attribute names, RGB triplets) don't benefit from word-at-a-time
 * tricks.
 *
 * Hosted builds do not compile this file; their libc provides these
 * symbols at strength STRONG, so even if it were compiled, the weak
 * shims would lose.
 */

#include <stddef.h>

#define TVG_SHIM_WEAK __attribute__((weak))

extern "C" {

/* ── <ctype.h> ASCII implementations ───────────────────────────────── *
 *
 * ISO C requires the argument to be representable as `unsigned char`
 * or equal to `EOF`; other values are UB.  The patterns below are
 * intentionally robust against those UB inputs anyway:
 *
 *   * The arithmetic-style functions wrap `c` through `(unsigned)…`
 *     after a subtraction that always produces a value outside
 *     [0, 10|26|6) for any negative or > 255 input, so they reliably
 *     return 0.
 *   * `tolower` / `toupper` only transform when `isupper` / `islower`
 *     succeeds — which it cannot for out-of-range `c` — so they pass
 *     such inputs through unchanged, matching `tolower(EOF) == EOF`
 *     as required by ISO C 7.4.2.1.
 *
 * Do not "harden" these with an `(unsigned char)c` cast: that would
 * change behaviour for `c > 255` (wrapping it back into the ASCII
 * range, which is the opposite of robust), and adds nothing for the
 * negative-int case the conditionals already handle.
 */

TVG_SHIM_WEAK int isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\n'
        || c == '\r' || c == '\v' || c == '\f';
}

TVG_SHIM_WEAK int isdigit(int c)
{
    return (unsigned)(c - '0') < 10u;
}

TVG_SHIM_WEAK int isalpha(int c)
{
    return ((unsigned)((c | 0x20) - 'a')) < 26u;
}

TVG_SHIM_WEAK int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

TVG_SHIM_WEAK int isxdigit(int c)
{
    return isdigit(c) || ((unsigned)((c | 0x20) - 'a')) < 6u;
}

TVG_SHIM_WEAK int isupper(int c)
{
    return (unsigned)(c - 'A') < 26u;
}

TVG_SHIM_WEAK int islower(int c)
{
    return (unsigned)(c - 'a') < 26u;
}

TVG_SHIM_WEAK int tolower(int c)
{
    return isupper(c) ? c | 0x20 : c;
}

TVG_SHIM_WEAK int toupper(int c)
{
    return islower(c) ? c & ~0x20 : c;
}

/* ── <stdlib.h> parsers ────────────────────────────────────────────── */

/* atoi: signed decimal, leading whitespace + sign tolerated. */
TVG_SHIM_WEAK int atoi(const char* s)
{
    int v = 0, neg = 0;
    while (*s == ' ' || *s == '\t') ++s;
    if (*s == '-') { neg = 1; ++s; }
    else if (*s == '+') ++s;
    while (*s >= '0' && *s <= '9') v = v * 10 + (*s++ - '0');
    return neg ? -v : v;
}

/* strtol: covers the bases thorvg uses (10 and 16).  Optional `end`
 * pointer is updated to point at the first non-converted byte. */
TVG_SHIM_WEAK long strtol(const char* s, char** end, int base)
{
    long v = 0;
    while (*s == ' ' || *s == '\t') ++s;
    int neg = 0;
    if (*s == '-') { neg = 1; ++s; }
    else if (*s == '+') ++s;
    if (base == 16) {
        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
        for (;;) {
            int c = (unsigned char)*s, d;
            if (c >= '0' && c <= '9') d = c - '0';
            else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
            else break;
            v = v * 16 + d;
            ++s;
        }
    } else {
        while (*s >= '0' && *s <= '9') v = v * 10 + (*s++ - '0');
    }
    if (end) *end = (char*)s;
    return neg ? -v : v;
}

/* ── <string.h> str-family ─────────────────────────────────────────── */

TVG_SHIM_WEAK size_t strlen(const char* s)
{
    const char* p = s;
    while (*p) ++p;
    return (size_t)(p - s);
}

TVG_SHIM_WEAK int strcmp(const char* a, const char* b)
{
    while (*a && *a == *b) { ++a; ++b; }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

TVG_SHIM_WEAK int strncmp(const char* a, const char* b, size_t n)
{
    while (n && *a && *a == *b) { ++a; ++b; --n; }
    if (!n) return 0;
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

TVG_SHIM_WEAK int strcasecmp(const char* a, const char* b)
{
    while (*a && *b) {
        int ca = (unsigned char)*a, cb = (unsigned char)*b;
        if (ca >= 'A' && ca <= 'Z') ca |= 0x20;
        if (cb >= 'A' && cb <= 'Z') cb |= 0x20;
        if (ca != cb) return ca - cb;
        ++a; ++b;
    }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

TVG_SHIM_WEAK int strncasecmp(const char* a, const char* b, size_t n)
{
    while (n && *a && *b) {
        int ca = (unsigned char)*a, cb = (unsigned char)*b;
        if (ca >= 'A' && ca <= 'Z') ca |= 0x20;
        if (cb >= 'A' && cb <= 'Z') cb |= 0x20;
        if (ca != cb) return ca - cb;
        ++a; ++b; --n;
    }
    if (!n) return 0;
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

TVG_SHIM_WEAK char* strchr(const char* s, int c)
{
    char ch = (char)c;
    for (;; ++s) {
        if (*s == ch) return (char*)s;
        if (*s == '\0') return NULL;
    }
}

TVG_SHIM_WEAK char* strrchr(const char* s, int c)
{
    char ch = (char)c;
    const char* last = NULL;
    for (; *s; ++s) if (*s == ch) last = s;
    if (ch == '\0') return (char*)s;
    return (char*)last;
}

TVG_SHIM_WEAK char* strstr(const char* h, const char* n)
{
    if (!*n) return (char*)h;
    for (; *h; ++h) {
        const char *hp = h, *np = n;
        while (*hp && *np && *hp == *np) { ++hp; ++np; }
        if (!*np) return (char*)h;
    }
    return NULL;
}

TVG_SHIM_WEAK char* strcpy(char* d, const char* s)
{
    char* r = d;
    while ((*d++ = *s++) != 0) {}
    return r;
}

TVG_SHIM_WEAK char* strncpy(char* d, const char* s, size_t n)
{
    char* r = d;
    while (n && (*d++ = *s++) != 0) --n;
    if (n) while (--n) *d++ = '\0';
    return r;
}

TVG_SHIM_WEAK char* strcat(char* d, const char* s)
{
    char* r = d;
    while (*d) ++d;
    while ((*d++ = *s++) != 0) {}
    return r;
}

TVG_SHIM_WEAK char* strncat(char* d, const char* s, size_t n)
{
    char* r = d;
    while (*d) ++d;
    while (n && (*d = *s) != 0) { ++d; ++s; --n; }
    *d = '\0';
    return r;
}

TVG_SHIM_WEAK void* memchr(const void* s, int c, size_t n)
{
    const unsigned char* p = (const unsigned char*)s;
    unsigned char ch = (unsigned char)c;
    while (n--) {
        if (*p == ch) return (void*)p;
        ++p;
    }
    return NULL;
}

/* strtok_r: thread-safe tokeniser used by tvgSvgLoader.cpp. */
TVG_SHIM_WEAK char* strtok_r(char* s, const char* delim, char** save)
{
    if (!s) s = *save;
    if (!s) return NULL;
    /* skip leading delimiters */
    for (; *s; ++s) {
        const char* d = delim;
        for (; *d && *d != *s; ++d) {}
        if (!*d) break;
    }
    if (!*s) { *save = NULL; return NULL; }
    char* tok = s;
    /* find end of token */
    for (; *s; ++s) {
        const char* d = delim;
        for (; *d && *d != *s; ++d) {}
        if (*d) { *s++ = '\0'; *save = s; return tok; }
    }
    *save = NULL;
    return tok;
}

/* strtoul: pulled in by libsupc++'s eh_alloc (GLIBCXX_TUNABLES). */
TVG_SHIM_WEAK unsigned long strtoul(const char* s, char** end, int base)
{
    unsigned long v = 0;
    while (*s == ' ' || *s == '\t') ++s;
    if (*s == '+') ++s;
    if (base == 16) {
        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
        for (;;) {
            int c = (unsigned char)*s, d;
            if (c >= '0' && c <= '9') d = c - '0';
            else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
            else break;
            v = v * 16 + (unsigned long)d;
            ++s;
        }
    } else {
        while (*s >= '0' && *s <= '9') v = v * 10 + (unsigned long)(*s++ - '0');
    }
    if (end) *end = (char*)s;
    return v;
}

/* getenv: bare-metal has no environment; libsupc++'s eh_alloc calls
 * it once to read `GLIBCXX_TUNABLES`.  Returning NULL is the documented
 * "variable not set" response and disables the tunables path. */
TVG_SHIM_WEAK char* getenv(const char*)
{
    return NULL;
}

/* ── pthread no-op stubs ───────────────────────────────────────────────
 *
 * The riscv32-esp-elf (and most other GNU embedded) toolchains ship a
 * libstdc++ built with `-pthread` enabled, so libsupc++.a's
 * `eh_globals.o` / `eh_alloc.o` reference the full pthread API even
 * when our own code is built `-fno-exceptions` and never throws.  At
 * runtime, libstdc++'s `gthr-default.h` checks `__gthread_active_p()`
 * (which is true only if `pthread_create` resolves to a real symbol)
 * before calling any of these — so as long as the stubs exist for the
 * linker, they are never actually invoked.
 *
 * `pthread_create` is intentionally absent: its absence is what tells
 * libstdc++ "threads inactive" at runtime.  All other entry points
 * are weak no-ops returning 0 / NULL. */
typedef int tvg_pthread_key_t;
typedef int tvg_pthread_mutex_t;
typedef int tvg_pthread_once_t;

TVG_SHIM_WEAK int  pthread_key_create(tvg_pthread_key_t*, void (*)(void*)) { return 0; }
TVG_SHIM_WEAK int  pthread_key_delete(tvg_pthread_key_t)                   { return 0; }
TVG_SHIM_WEAK void* pthread_getspecific(tvg_pthread_key_t)                 { return NULL; }
TVG_SHIM_WEAK int  pthread_setspecific(tvg_pthread_key_t, const void*)     { return 0; }
TVG_SHIM_WEAK int  pthread_once(tvg_pthread_once_t*, void (*)(void))       { return 0; }
TVG_SHIM_WEAK int  pthread_mutex_init(tvg_pthread_mutex_t*, const void*)   { return 0; }
TVG_SHIM_WEAK int  pthread_mutex_destroy(tvg_pthread_mutex_t*)             { return 0; }
TVG_SHIM_WEAK int  pthread_mutex_lock(tvg_pthread_mutex_t*)                { return 0; }
TVG_SHIM_WEAK int  pthread_mutex_unlock(tvg_pthread_mutex_t*)              { return 0; }
TVG_SHIM_WEAK int  pthread_mutex_trylock(tvg_pthread_mutex_t*)             { return 0; }

/* ── newlib / libstdc++ misc stubs ──────────────────────────────────── */

/* __errno: newlib's `errno` macro expands to `(*__errno())`.  libm's
 * wrapper functions (wf_sqrt, wf_fmod, …) set errno on domain errors.
 * A per-binary thread-unsafe slot is fine: bare-metal builds are
 * single-threaded by construction and nobody reads thorvg's errno. */
TVG_SHIM_WEAK int* __errno(void)
{
    static int e = 0;
    return &e;
}

/* getentropy: pulled by libstdc++'s `random.cc` (std::random_device's
 * default constructor probes the host for entropy).  Bare-metal has
 * none; return failure so the caller falls through to the next
 * source.  thorvg itself never instantiates random_device. */
TVG_SHIM_WEAK int getentropy(void* buf, size_t len)
{
    unsigned char* p = (unsigned char*)buf;
    while (len--) *p++ = 0;
    return -1;
}

/* arc4random: same story — fallback PRNG probed by random.cc.  Zero
 * is a legal output; the caller treats it as a regular sample. */
TVG_SHIM_WEAK unsigned int arc4random(void)
{
    return 0;
}

/* __xpg_strerror_r: POSIX/XSI strerror_r variant referenced by
 * libstdc++'s `generic_error_category::message`.  We have no error
 * string table on bare metal — return success with an empty buffer. */
TVG_SHIM_WEAK int __xpg_strerror_r(int, char* buf, size_t buflen)
{
    if (buf && buflen) buf[0] = '\0';
    return 0;
}

} /* extern "C" */
