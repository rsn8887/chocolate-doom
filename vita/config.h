#pragma once

#ifndef __CONFIG_H
#define __CONFIG_H

/* Define to 1 if you have the declaration of `strcasecmp', and to 0 if you
   don't. */
#define HAVE_DECL_STRCASECMP 1

/* Define to 1 if you have the declaration of `strncasecmp', and to 0 if you
   don't. */
#define HAVE_DECL_STRNCASECMP 1

/* Define to 1 if you have the <dev/isa/spkrio.h> header file. */
/* #undef HAVE_DEV_ISA_SPKRIO_H */

/* Define to 1 if you have the <dev/speaker/speaker.h> header file. */
/* #undef HAVE_DEV_SPEAKER_SPEAKER_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `ioperm' function. */
/* #undef HAVE_IOPERM */

/* Define to 1 if you have the `amd64' library (-lamd64). */
/* #undef HAVE_LIBAMD64 */

/* Define to 1 if you have the `i386' library (-li386). */
/* #undef HAVE_LIBI386 */

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* libpng installed */
#define HAVE_LIBPNG 1

/* libsamplerate installed */
/* #undef HAVE_LIBSAMPLERATE */

/* Define to 1 if you have the <linux/kd.h> header file. */
/* #undef HAVE_LINUX_KD_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mmap' function. */
/* #undef HAVE_MMAP */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Name of package */
#define PACKAGE "chocolate-doom"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "chocolate-doom-dev-list@chocolate-doom.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Chocolate Doom"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Chocolate Doom 3.0.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "chocolate-doom"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.0.0"

/* Change this when you create your awesome forked version */
#define PROGRAM_PREFIX "chocolate-"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "3.0.0"

/* Vita-specific stuff */
#include <vitasdk.h>

#define VITA_BASEDIR "ux0:/data/chocolate"
#define VITA_CWD VITA_BASEDIR
#define VITA_TMPDIR VITA_BASEDIR "/tmp"

#undef mkdir
#define mkdir(path, mode) sceIoMkdir(path, mode)

void Vita_Init(void);

#endif
