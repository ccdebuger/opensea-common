//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012 - 2017 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common_nix.c
// \brief Implements functions that are common to (u)nix like platforms code.
//
#include "common.h"
#include "common_nix.h"
#include <sys/types.h>
//The defines below are to enable fstat64 which i used in the function to read a file size (for large files, 64bit numbers may be required)
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#include <sys/stat.h>
#include <sys/param.h> //hopefully this is available on all 'nix systems
#include <sys/utsname.h>
#include <unistd.h>
#include <ctype.h>

//freeBSD doesn't have the 64 versions of these functions...so I'm defining things this way to make it work. - TJE
#if defined(__FreeBSD__)
#define stat64 stat
#define fstat64 fstat
#endif

bool os_Directory_Exists(const char * const pathToCheck)
{
    struct stat st;
    if (stat(pathToCheck, &st) == SUCCESS)
    {
        return (st.st_mode & S_IFDIR) != 0;
    }
    return false;
}

bool os_File_Exists(const char * const filetoCheck)
{
    struct stat st;
    return (stat(filetoCheck, &st) == SUCCESS);
}

int get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    char *resolvedPath = realpath(pathAndFile, fullPath);
    if (resolvedPath)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

static uint16_t get_Console_Default_Color()
{
    //This is a dummy function. We cannot actually get the default colors in linux. The ANSI escape sequence has a "default" or "remove formatting" that we use instead.
    //This is here to match what we have in Windows, where we actually need to store this value to return the console back to it's default value later. - TJE
    //This is currently only in here
    return 0;
}

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    //linux/BSD use the ANSI escape sequences to change colors http://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html
    if (foregroundBackground)//change foreground color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            printf("\033[0;34m");
            break;
        case BLUE:
            printf("\033[1;34m");
            break;
        case DARK_GREEN:
            printf("\033[0;32m");
            break;
        case GREEN:
            printf("\033[1;32m");
            break;
        case DARK_RED:
            printf("\033[0;31m");
            break;
        case RED:
            printf("\033[1;31m");
            break;
        case BLACK:
            printf("\033[0;30m");
            break;
        case BROWN:
            printf("\033[0;33m");
            break;
        case YELLOW:
            printf("\033[1;33m");
            break;
        case TEAL:
            printf("\033[0;36m");
            break;
        case CYAN:
            printf("\033[1;36m");
            break;
        case PURPLE:
            printf("\033[0;35m");
            break;
        case MAGENTA:
            printf("\033[1;35m");
            break;
        case WHITE:
            printf("\033[1;37m");
            break;
        case DARK_GRAY:
            printf("\033[1;30m");
            break;
        case GRAY:
            printf("\033[0;37m");
            break;
        case DEFAULT://fall through to default (which is white)
        default:
            printf("\033[0m");
        }
    }
    else//change background color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            printf("\033[7;34m");
            break;
        case BLUE:
            printf("\033[7;1;34m");
            break;
        case DARK_GREEN:
            printf("\033[7;32m");
            break;
        case GREEN:
            printf("\033[7;1;32m");
            break;
        case DARK_RED:
            printf("\033[7;31m");
            break;
        case RED:
            printf("\033[7;1;31m");
            break;
        case BLACK:
            printf("\033[7;30m");
            break;
        case BROWN:
            printf("\033[7;33m");
            break;
        case YELLOW:
            printf("\033[7;1;33m");
            break;
        case TEAL:
            printf("\033[7;36m");
            break;
        case CYAN:
            printf("\033[7;1;36m");
            break;
        case PURPLE:
            printf("\033[7;35m");
            break;
        case MAGENTA:
            printf("\033[7;1;35m");
            break;
        case WHITE:
            printf("\033[7;1;37m");
            break;
        case DARK_GRAY:
            printf("\033[7;1;30m");
            break;
        case GRAY:
            printf("\033[7;37m");
            break;
        case DEFAULT://fall through to default (which is white)
        default:
            printf("\033[0m");
        }
    }
}

eArchitecture get_Compiled_Architecture(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    #if defined __INTEL_COMPILER //Intel's C/C++ compiler
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_ARCH_X86_64;
        #elif defined _M_ALPHA
            return OPENSEA_ARCH_ALPHA;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_ARCH_ARM;
        #elif defined _M_IX86
            return OPENSEA_ARCH_X86;
        #elif defined _M_IA64
            return OPENSEA_ARCH_IA_64;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_ARCH_POWERPC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __CYGWIN__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm most unsure about)
        #if defined __alpha__
            return OPENSEA_ARCH_ALPHA;
        #elif defined __amd64__ || defined __x86_64__
            return OPENSEA_ARCH_X86_64;
        #elif defined __arm__ || defined __thumb__
            return OPENSEA_ARCH_ARM;
        #elif defined __aarch64__
            return OPENSEA_ARCH_ARM64;
        #elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__
            return OPENSEA_ARCH_X86;
        #elif defined __ia64__ || defined __IA64__
            return OPENSEA_ARCH_IA_64;
        #elif defined __powerpc64__ || defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64
            return OPENSEA_ARCH_POWERPC64;
        #elif defined __powerpc__ || defined __POWERPC__ || defined __PPC__ || defined __ppc__ || defined _ARCH_PPC
            return OPENSEA_ARCH_POWERPC;
        #elif defined __sparc__
            return OPENSEA_ARCH_SPARC;
        #elif defined __s390__ || defined __s390x__ || defined __zarch__
            return OPENSEA_ARCH_SYSTEMZ;
        #elif defined __mips__
            return OPENSEA_ARCH_MIPS;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __SUNPRO_C || defined __SUNPRO_CC //SUN/Oracle compilers (unix)
        #if defined __amd64__ || defined __x86_64__
            return OPENSEA_ARCH_X86_64;
        #elif defined __i386
            return OPENSEA_ARCH_X86;
        #elif defined __sparc
            return OPENSEA_ARCH_SPARC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __IBMC__ || defined __IBMCPP__ //IBM compiler (unix, linux)
        #if defined __370__ || defined __THW_370__
            return OPENSEA_ARCH_SYSTEMZ;
        #elif defined __THW_INTEL__
            return OPENSEA_ARCH_X86;
        #elif defined _ARCH_PPC64
            return OPENSEA_ARCH_POWERPC64;
        #elif defined _ARCH_PPC
            return OPENSEA_ARCH_POWERPC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #else
        return OPENSEA_ARCH_UNKNOWN;
    #endif
}
//https://sourceforge.net/p/predef/wiki/Endianness/
eEndianness calculate_Endianness(void)
{
    static eEndianness endian = OPENSEA_UNKNOWN_ENDIAN;//using static so that it should only need to run this code once...not that it takes a long time, but this may help optimise this.
    if (endian == OPENSEA_UNKNOWN_ENDIAN)
    {
        union
        {
            uint32_t value;
            uint8_t data[sizeof(uint32_t)];
        } number;

        number.data[0] = 0x00;
        number.data[1] = 0x01;
        number.data[2] = 0x02;
        number.data[3] = 0x03;

        switch (number.value)
        {
        case UINT32_C(0x00010203):
            endian = OPENSEA_BIG_ENDIAN;
            break;
        case UINT32_C(0x03020100):
            endian = OPENSEA_LITTLE_ENDIAN;
            break;
        case UINT32_C(0x02030001):
            endian = OPENSEA_BIG_WORD_ENDIAN;
            break;
        case UINT32_C(0x01000302):
            endian = OPENSEA_LITTLE_WORD_ENDIAN;
            break;
        default:
            endian = OPENSEA_UNKNOWN_ENDIAN;
            break;
        }
    }
    return endian;
}

eEndianness get_Compiled_Endianness(void)
{
    #if defined (__BYTE_ORDER__)
        #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            return OPENSEA_BIG_ENDIAN;
        #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            return OPENSEA_LITTLE_ENDIAN;
        #else
            return calculate_Endianness();
        #endif
    #else
        #if defined (__BIG_ENDIAN__)
            return OPENSEA_BIG_ENDIAN;
        #elif defined (__LITTLE_ENDIAN__)
            return OPENSEA_LITTLE_ENDIAN;
        #else
            //check architecture specific defines...
            #if defined (__ARMEB__) || defined (__THUMBEB__) || defined (__AARCH64EB__) || defined (_MIPSEB) || defined (__MIPSEB) || defined (__MIPSEB__)
                return OPENSEA_BIG_ENDIAN;
            #elif defined (__ARMEL__) || defined (__THUMBEL__) || defined (__AARCH64EL__) || defined (_MIPSEL) || defined (__MIPSEL) || defined (__MIPSEL__)
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #endif
    #endif
    return calculate_Endianness();
}

//code is written based on the table in this link https://en.wikipedia.org/wiki/Uname
//This code is not complete for all operating systems. I only added in support for the ones we are most interested in or are already using today. -TJE
int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    int ret = SUCCESS;
    //check what is filled in by the uname call in the utsname struct (BUT DON"T CHECK THE DOMAIN NAME since that is GNU specific)
    struct utsname unixUname;
    memset(&unixUname, 0, sizeof(struct utsname));
    if (0 == uname(&unixUname))
    {
        convert_String_To_Upper_Case(unixUname.sysname);
        if (strcmp("LINUX", unixUname.sysname) == 0)//some OSs like Android or ESX may fall into here, so this section may need expansion if that happens
        {
            bool linuxOSNameFound = false;
            versionNumber->osVersioningIdentifier = OS_LINUX;
            //linux kernels are versioned as kernel.major.minor-securityAndBugFixes-SomeString
            //older linux kernels don't have the -securityAndBugFixes on the end
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"-%"SCNu16"%*s",&versionNumber->versionType.linuxVersion.kernelVersion, &versionNumber->versionType.linuxVersion.majorVersion, &versionNumber->versionType.linuxVersion.minorVersion, &versionNumber->versionType.linuxVersion.securityAndBugFixesNumber))
            {
                ret = FAILURE;
            }
            //set the operating system name from /etc/issue (if available, otherwise set "Unknown Linux OS")
            if (os_File_Exists("/etc/issue"))
            {
                //read this file and get the linux name
                FILE *issue = fopen("/etc/issue","r");
                if (issue)
                {
                    //read it
                    fseek(issue,ftell(issue),SEEK_END);
                    long int issueSize = ftell(issue);
                    rewind(issue);
                    char *issueMemory = (char*)calloc(issueSize,sizeof(char));
                    if (fread(issueMemory, sizeof(char), issueSize, issue))
                    {
                        linuxOSNameFound = true;
                        if (operatingSystemName)
                        {
                            strncpy(&operatingSystemName[0], issueMemory, OS_NAME_SIZE - 1);
                            operatingSystemName[OS_NAME_SIZE-1] = '\0';
                        }
                    }
                    safe_Free(issueMemory);
                    fclose(issue);
                }
            }
            //if we couldn't find a name, set unknown linux os
            if (!linuxOSNameFound && operatingSystemName)
            {
                sprintf(&operatingSystemName[0],"Unknown Linux OS");
            }
        }
        else if (strcmp("FREEBSD", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_FREEBSD;
            //FreeBSD version is stored as Major.Minor-SomeString
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.freeBSDVersion.majorVersion, &versionNumber->versionType.freeBSDVersion.minorVersion))
            {
                ret = FAILURE;
            }
            //set the OS Name
            if (operatingSystemName)
            {
                if (ret != FAILURE)
                {
                    sprintf(&operatingSystemName[0], "FreeBSD %"PRIu16".%"PRIu16"", versionNumber->versionType.freeBSDVersion.majorVersion, versionNumber->versionType.freeBSDVersion.minorVersion);
                }
                else
                {
                    sprintf(&operatingSystemName[0], "Unknown FreeBSD OS Version");
                }
            }
        }
        else if (strcmp("SUNOS", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_SOLARIS;
            //Solaris stores the SunOS version in release
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.solarisVersion.sunOSMajorVersion, &versionNumber->versionType.solarisVersion.sunOSMinorVersion, &versionNumber->versionType.solarisVersion.sunOSRevision))
            {
                ret = FAILURE;
            }
            //set OS name as Solaris "unixUname.version" for the OS Name
            if (operatingSystemName)
            {
                sprintf(&operatingSystemName[0], "Solaris %s", unixUname.version);
            }
            //The Solaris Version/name is stored in version
            if (isdigit(unixUname.version[0]))
            {
                //set OS name as Solaris x.x
                if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.solarisVersion.solarisMajorVersion, &versionNumber->versionType.solarisVersion.solarisMinorVersion, &versionNumber->versionType.solarisVersion.solarisRevision))
                {
                    //do nothing for now - TJE
                }
            }
        }
        else if (strcmp("DARWIN", unixUname.sysname) == 0)//Mac OSX
        {
            versionNumber->osVersioningIdentifier = OS_MACOSX;
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.macOSVersion.majorVersion, &versionNumber->versionType.macOSVersion.minorVersion, &versionNumber->versionType.macOSVersion.revision))
            {
                ret = FAILURE;
            }
            //set the OS Name from the major numbers starting with "Puma"
            //https://en.wikipedia.org/wiki/Darwin_(operating_system)
            if (operatingSystemName)
            {
                switch (versionNumber->versionType.macOSVersion.majorVersion)
                {
                case 5://puma
                    sprintf(&operatingSystemName[0], "Mac OS X 10.1 Puma");
                    break;
                case 6://jaguar
                    sprintf(&operatingSystemName[0], "Mac OS X 10.2 Jaguar");
                    break;
                case 7://panther
                    sprintf(&operatingSystemName[0], "Mac OS X 10.3 Panther");
                    break;
                case 8://tiger
                    sprintf(&operatingSystemName[0], "Mac OS X 10.4 Tiger");
                    break;
                case 9://leopard
                    sprintf(&operatingSystemName[0], "Mac OS X 10.5 Leopard");
                    break;
                case 10://snow leopard
                    sprintf(&operatingSystemName[0], "Mac OS X 10.6 Snow Leopard");
                    break;
                case 11://lion
                    sprintf(&operatingSystemName[0], "Mac OS X 10.7 Lion");
                    break;
                case 12://Mountain Lion
                    sprintf(&operatingSystemName[0], "OS X 10.8 Mountain Lion");
                    break;
                case 13://mavericks
                    sprintf(&operatingSystemName[0], "OS X 10.9 Mavericks");
                    break;
                case 14://Yosemite
                    sprintf(&operatingSystemName[0], "OS X 10.10 Yosemite");
                    break;
                case 15://el capitan
                    sprintf(&operatingSystemName[0], "OS X 10.11 El Capitan");
                    break;
                default:
                    sprintf(&operatingSystemName[0], "Unknown Mac OS X Version");
                    break;
                }
            }
        }
        else if (strcmp("AIX", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_AIX;
            versionNumber->versionType.aixVersion.majorVersion = (uint16_t)atoi(unixUname.version);
            versionNumber->versionType.aixVersion.minorVersion = (uint16_t)atoi(unixUname.release);
            if (operatingSystemName)
            {
                sprintf(&operatingSystemName[0], "AIX %"PRIu16".%"PRIu16"", versionNumber->versionType.aixVersion.majorVersion, versionNumber->versionType.aixVersion.minorVersion);
            }
        }
        else if (strcmp("DRAGONFLY", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_DRAGONFLYBSD;
            if (EOF == sscanf(unixUname.release, "%"SCNu16".%"SCNu16"%*s", &versionNumber->versionType.dragonflyVersion.majorVersion, &versionNumber->versionType.dragonflyVersion.minorVersion))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "Unknown Dragonfly BSD Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "Dragonfly BSD %"PRIu16".%"PRIu16"", versionNumber->versionType.dragonflyVersion.majorVersion, versionNumber->versionType.dragonflyVersion.minorVersion);
                }
            }
        }
        else if (strcmp("OPENBSD", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_OPENBSD;
            versionNumber->versionType.openBSDVersion.majorVersion = (uint16_t)atoi(unixUname.version);
            versionNumber->versionType.openBSDVersion.minorVersion = (uint16_t)atoi(unixUname.release);
            if (operatingSystemName)
            {
                sprintf(&operatingSystemName[0], "OpenBSD %"PRIu16".%"PRIu16"", versionNumber->versionType.openBSDVersion.majorVersion, versionNumber->versionType.openBSDVersion.minorVersion);
            }
        }
        else if (strcmp("NETBSD", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_NETBSD;
            if (EOF == sscanf(unixUname.release, "%"SCNu16".%"SCNu16".%"SCNu16"%*s", &versionNumber->versionType.netBSDVersion.majorVersion, &versionNumber->versionType.netBSDVersion.minorVersion, &versionNumber->versionType.netBSDVersion.revision))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "Unknown NetBSD Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "NetBSD %s", unixUname.release);
                }
            }
        }
        else if (strcmp("OSF1", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_TRU64;
            if (EOF == sscanf(unixUname.release, "V%"SCNu16".%"SCNu16"", &versionNumber->versionType.tru64Version.majorVersion, &versionNumber->versionType.tru64Version.minorVersion))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "Unknown Tru64 Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "Tru64 %s", unixUname.release);
                }
            }
        }
        else if (strcmp("HP-UX", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_HPUX;
            if (EOF == sscanf(unixUname.release, "B.%"SCNu16".%"SCNu16"", &versionNumber->versionType.hpuxVersion.majorVersion, &versionNumber->versionType.hpuxVersion.minorVersion))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "Unknown HP-UX Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    sprintf(&operatingSystemName[0], "HP-UX %"PRIu16".%"PRIu16"", versionNumber->versionType.hpuxVersion.majorVersion, versionNumber->versionType.hpuxVersion.minorVersion);
                }
            }
        }
        else //don't know what unix this is so return not supported
        {
            ret = NOT_SUPPORTED;
        }
    }
    else
    {
        ret = FAILURE;
    }
    return ret;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    struct stat64 st;
    memset(&st, 0, sizeof(struct stat64));
    if (0 == fstat64(fileno(filePtr), &st))
    {
        return st.st_size;
    }
    else
    {
        return -1;
    }
}

void start_Timer(seatimer_t *timer)
{
    struct timespec startTimespec;
    int ret = 0;
    memset(&startTimespec, 0, sizeof(struct timespec));
    ret = clock_gettime(CLOCK_MONOTONIC, &startTimespec);
    if (0 == ret)//hopefully this always works...-TJE
    {
//        printf("Start Time:  %lu\n", startTimespec.tv_nsec);
        timer->timerStart = (uint64_t) (startTimespec.tv_sec * (uint64_t) 1000000000) + startTimespec.tv_nsec;
    }
//    else
//    {
//       printf("Bad start_timer Ret:  %d\n",ret);
//    }
}

void stop_Timer(seatimer_t *timer)
{
    struct timespec stopTimespec;
    int ret = 0;
    memset(&stopTimespec, 0, sizeof(struct timespec));
    ret = clock_gettime(CLOCK_MONOTONIC, &stopTimespec);
    if (0 == ret)//hopefully this always works...-TJE
    {
//        printf("Stop Time:  %lu\n", stopTimespec.tv_nsec);
        timer->timerStop = (uint64_t) (stopTimespec.tv_sec * (uint64_t) 1000000000) + stopTimespec.tv_nsec;
    }
//    else
//    {
//       printf("Bad stop_timer Ret:  %d\n",ret);
//    }
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    return timer.timerStop - timer.timerStart;
}

double get_Micro_Seconds(seatimer_t timer)
{
    uint64_t nanoseconds = get_Nano_Seconds(timer);
    return ((double)nanoseconds / 1000.00);
}

double get_Milli_Seconds(seatimer_t timer)
{
    return (get_Micro_Seconds(timer) / 1000.00);
}

double get_Seconds(seatimer_t timer)
{
    return (get_Milli_Seconds(timer) / 1000.00);
}
