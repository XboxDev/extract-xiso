/*
 * Copyright (C) 2001 Federico Di Gregorio <fog@debian.org> 
 * Copyright (C) 1991, 1994-1999, 2000, 2001 Free Software Foundation, Inc.
 *
 * This code has been derived from an example in the glibc2 documentation.
 * This file is part of the psycopg module.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * asprintf.c -- asprintf() implementation for braindamaged operating systems
 */

#ifndef _WIN32
#include <unistd.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
#include <stdlib.h>

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

int asprintf(char **buffer, char *fmt, ...);
int asprintf(char **buffer, char *fmt, ...)
{
    /* Guess we need no more than 200 chars of space. */
    int size = 200;
    int nchars;
    va_list ap;
    
    *buffer = (char*)malloc(size);
    if (*buffer == NULL) return -1;
          
    /* Try to print in the allocated space. */
    va_start(ap, fmt);
    nchars = vsnprintf(*buffer, size, fmt, ap);
    va_end(ap);

    if (nchars >= size)
    {
        char *tmpbuff;
        /* Reallocate buffer now that we know how much space is needed. */
        size = nchars+1;
        tmpbuff = (char*)realloc(*buffer, size);
        
          
        if (tmpbuff == NULL) { /* we need to free it*/
            free(*buffer);
            return -1;
        }
        
        *buffer=tmpbuff;
        /* Try again. */
        va_start(ap, fmt);
        nchars = vsnprintf(*buffer, size, fmt, ap);
        va_end(ap);
    }

    if (nchars < 0) return nchars;
    return size;
}
