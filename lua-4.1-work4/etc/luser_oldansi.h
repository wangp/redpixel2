/*
** $Id: $
** Compatibility module for "non-that-ansi systems"
** Real ISO (ANSI) systems do not need these definitions;
** but some systems (Sun OS) are not that ISO...
** See Copyright Notice in lua.h
*/


#ifndef oldansi_h
#define oldansi_h

#define OLD_ANSI


#define l_malloc(s)		malloc(s)
#define l_realloc(b,os,s)       ((b) == NULL ? malloc(s) : realloc(b, s))
#define l_free(b,s)             if (b) free(b)


/* no support for locale and for strerror: fake them */
#define setlocale(a,b)  ((void)a, strcmp((b),"C")==0?"C":NULL)
#define LC_ALL          0
#define LC_COLLATE      0
#define LC_CTYPE        0
#define LC_MONETARY     0
#define LC_NUMERIC      0
#define LC_TIME         0
#define strerror(e)     "I/O error"
#define errno           (-1)


/* no support for strcoll */
#define strcoll(a,b)    strcmp(a,b)


#endif
