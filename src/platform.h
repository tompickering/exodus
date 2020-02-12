#ifndef GUARD_PLATFORM_H
#define GUARD_PLATFORM_H

/*
 * Here, we can select a particular implementation of a class for use on
 * a per-platform basis. By encapsulating any subsystems which might need
 * different treatment on other potential target platforms, we can keep
 * this code easily-portable and compartmentalised at the same time.
 *
 * Unfortunately, swapping out classes is a two-step process:
 *
 * 1. Update this file to point each type at the correct implementation.
 *
 * 2. Update the generic header of that class type to #include the
 *    appropriate specific header for this platform.
 */

#ifdef SDL
#define EXODUS ExodusSDL
#define AUDIOMANAGER AudioManagerSDL
#define DRAWMANAGER DrawManagerSDL
#define INPUTMANAGER InputManagerSDL
#endif

#ifdef LINUX
#define TIMER TimerChrono
#endif

#endif
