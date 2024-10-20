#ifndef OS_H
#define OS_H

#define OS_WINDOWS "windows"
#define OS_LINUX "linux"
#define OS_MAC "osx"

#define OS_NULL ""

#if defined(_WIN32)
#define PLATFORM_NAME OS_WINDOWS // Windows
#elif defined(_WIN64)
#define PLATFORM_NAME OS_WINDOWS // Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_NAME OS_WINDOWS // Windows (Cygwin POSIX under Microsoft Window)
#elif defined(__linux__)
#define PLATFORM_NAME                                                          \
    OS_LINUX // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and
            // other
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
#elif TARGET_OS_MAC == 1
#define PLATFORM_NAME OS_MAC // Apple OSX
#else
#define PLATFORM_NAME OS_NULL
#endif

const char *get_platform_name();

#endif
