#ifndef wwwc_h
#define wwc_h


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#define L_CHAR	wchar_t
#define l_charint	wint_t

#define uchar(c)	(c)


#define l_c(x)	L##x
#define l_saux(x)	L##x
#define l_s(x)		l_saux(x)

#undef EOF
#define EOF		WEOF

#define	strcspn		wcscspn
#define fgetc		fgetwc
#define fgets		fgetws
#define fprintf		fwprintf
#define fputs		fputws
#define fscanf		fwscanf
#define printf		wprintf
#define sprintf		swprintf
#define strchr		wcschr
#define strcmp		wcscmp
#define strcoll		wcscoll
#define strcpy		wcscpy
#define strftime	wcsftime
#define strlen		wcslen
#define strncpy		wcsncpy
#define strpbrk		wcspbrk
#define strtod		wcstod
#define strtol		wcstol
#define strtoul		wcstoul
#define vsprintf	vswprintf


#define fopen		_wfopen
#define strerror(x)	L"system error"
#define system		_wsystem
#define remove		_wremove
#define rename		_wrename
#define tmpnam		_wtmpnam
#define getenv		_wgetenv
#define setlocale	_wsetlocale
#define perror		_wperror

#undef isalnum
#define	isalnum		iswalnum
#undef isalpha
#define	isalpha		iswalpha
#undef iscntrl
#define	iscntrl		iswcntrl
#undef isdigit
#define	isdigit		iswdigit
#undef islower
#define	islower		iswlower
#undef ispunct
#define	ispunct		iswpunct
#undef isspace
#define	isspace		iswspace
#undef isupper
#define	isupper		iswupper
#undef isxdigit
#define	isxdigit	iswxdigit

#define main	wmain

#endif

