/* Default arguments you want when running my: 86_64-trazelos-gcc toolchain */
#undef LIB_SPEC
#define LIB_SPEC "-lc" /* link against C standard library */

/* Files that are linked before user code.
   The %s tells GCC to look for these files in the library directory. */
#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0.o%s crti.o%s crtbegin.o%s"

/* Files that are linked after user code. */
#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

/* Additional predefined macros. */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()        \
  do {                                  \
    builtin_define ("__trazelos_");     \
    builtin_define ("__unix__");        \
    builtin_assert ("system=trazelos"); \
    builtin_assert ("system=unix");     \
    builtin_assert ("system=posix");    \
  } while(0);
