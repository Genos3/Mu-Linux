#pragma once

// MSVC conserva sus palabras clave. GCC/Clang x86 usan los atributos ABI
// equivalentes; no borrar convenciones en callbacks ni punteros a funciones.
// KOS/SH-4 no soporta __stdcall/__fastcall/__thiscall: se anulan.
#if defined(__DREAMCAST__)
// KOS/SH-4 no soporta fastcall/stdcall/thiscall; newlib ya define __fastcall
// como __attribute__((__fastcall__)), que GCC ignora con -Wattributes. Silenciar.
#pragma GCC diagnostic ignored "-Wattributes"
#undef __fastcall
#define __cdecl
#define __stdcall
#define __fastcall
#define __thiscall
#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif
#elif defined(__GNUC__) && !defined(_MSC_VER)

#ifndef __forceinline
#define __forceinline inline __attribute__((always_inline))
#endif
#ifndef __cdecl
#define __cdecl __attribute__((cdecl))
#endif
#ifndef __stdcall
#define __stdcall __attribute__((stdcall))
#endif
#ifndef __fastcall
#define __fastcall __attribute__((fastcall))
#endif
#ifndef __thiscall
#define __thiscall __attribute__((thiscall))
#endif
#endif
