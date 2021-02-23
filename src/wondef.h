/* wondef.h --- Wonders API by katahiromz */
/**************************************************************************/

#ifndef WONDEF_H
#define WONDEF_H    5  /* Version 5 */

#if defined(_WIN32) && !defined(WONVER)
    #include <windows.h>
#else

#include "wonnt.h"

#define MAX_PATH 260

#ifndef NULL
    #ifdef __cplusplus
        #ifdef _WIN64
            #define NULL 0LL
        #else
            #define NULL 0
        #endif
    #else
        #define NULL ((void *)0)
    #endif
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef MAKEWORD
    #define MAKEWORD(a,b) \
        ((WORD) (((BYTE) (((DWORD_PTR) (a)) & 0xff)) | \
            ((WORD) ((BYTE) (((DWORD_PTR) (b)) & 0xff))) << 8))
    #define MAKELONG(a, b) \
        ((LONG) (((WORD) (((DWORD_PTR) (a)) & 0xffff)) | \
            ((DWORD) ((WORD) (((DWORD_PTR) (b)) & 0xffff))) << 16))
    #define LOWORD(l) ((WORD) (((DWORD_PTR) (l)) & 0xffff))
    #define HIWORD(l) ((WORD) ((((DWORD_PTR) (l)) >> 16) & 0xffff))
    #define LOBYTE(w) ((BYTE) (((DWORD_PTR) (w)) & 0xff))
    #define HIBYTE(w) ((BYTE) ((((DWORD_PTR) (w)) >> 8) & 0xff))
    #define MAKELANGID(p, s) \
        ((LANGID)((((WORD)(p)) << 10) | ((WORD)(s))))
    #define MAKELCID(langid, sortid) \
        ((LCID)((((WORD)(langid)) << 16) | ((WORD)(sortid))))
    #define PRIMARYLANGID(langid) (((WORD)(langid)) & 0x3FF)
    #define SUBLANGID(langid) ((WORD)((langid) >> 10))
#endif

#endif  /* !(defined(_WIN32) && !defined(WONVER)) */
#endif  /* ndef WONDEF_H */
