//Torlus - some parts are taken from JaysOS

#include <stdarg.h>
#include <stdlib.h>

#define printf printf
#define fprintf fprintf
#include <stdio.h>

#ifdef SIMU
int putchar_md(int c) {
  putchar(c);
}
#endif

#ifdef GBA
extern void write_char(int ch);
int putchar_md(int c) {
  write_char( c );
}
#endif

void vprintf_md(const char *, va_list);

void
fprintf_md(FILE *stream, const char *format, ...) {
  va_list ap;

  if ( (stream == stdout) || (stream == stderr) ) {
    va_start(ap, format);
    vprintf_md(format, ap);
    va_end(ap);
  }
}

void
printf_md(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);

    vprintf_md(fmt, ap);
    va_end(ap);
}


int
snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list ap;
    int len;
    va_start(ap, fmt);
    len = vsnprintf_md(buf, size, fmt, ap);
    va_end(ap);
    return len;

}

int
sprintf_md(char *buf, const char *fmt, ...) {

    va_list ap;
    int len;
    va_start(ap, fmt);
    len = vsnprintf_md(buf, 1024, fmt, ap);
    va_end(ap);
    return len;

}

/*******************!!! DOES NOT SUPPORT %f !!!***************************/


/*	$NetBSD: subr_prf.c,v 1.3 2000/03/30 12:19:49 augustss Exp $	*/

/*-
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)printf.c	8.1 (Berkeley) 6/11/93
 */

/*
 * Scaled down version of printf(3).
 *
 * One additional format:
 *
 * The format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	printf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 */



static void kprintn (void (*)(char), unsigned long, int);
static void sputchar (char);
static void kdoprnt (void (*)(char), const char *, va_list);


static char *sbuf, *ebuf;

static void
sputchar(c)
	char c;
{
	if (sbuf < ebuf)
		*sbuf++ = c;
}

void
vprintf_md(const char *fmt, va_list ap)
{

	kdoprnt(putchar_md, fmt, ap);
}

int
vsnprintf_md(char *buf, size_t size, const char *fmt, va_list ap)
{

	sbuf = buf;
	ebuf = buf + size - 1;
	kdoprnt(sputchar, fmt, ap);
	*sbuf = '\0';
	return (sbuf - buf);
}

void
kdoprnt(void (*put)(char), const char *fmt, va_list ap)
{
	char *p;
	int ch, n;
	unsigned long ul;
	int lflag, set;

	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0')
				return;
			put(ch);
		}
		lflag = 0;
reswitch:	switch (ch = *fmt++) {
		case '\0':
			/* XXX print the last format character? */
			return;
		case 'l':
			lflag = 1;
			goto reswitch;
		case 'b':
			ul = va_arg(ap, int);
			p = va_arg(ap, char *);
			kprintn(put, ul, *p++);

			if (!ul)
				break;

			for (set = 0; (n = *p++);) {
				if (ul & (1 << (n - 1))) {
					put(set ? ',' : '<');
					for (; (n = *p) > ' '; ++p)
						put(n);
					set = 1;
				} else
					for (; *p > ' '; ++p);
			}
			if (set)
				put('>');
			break;
		case 'c':
			ch = va_arg(ap, int);
				put(ch & 0x7f);
			break;
		case 's':
			p = va_arg(ap, char *);
			while ((ch = *p++))
				put(ch);
			break;
		case 'd':
			ul = lflag ?
			    va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				put('-');
				ul = -(long)ul;
			}
			kprintn(put, ul, 10);
			break;
		case 'o':
			ul = lflag ?
			    va_arg(ap, unsigned long) : va_arg(ap, unsigned long);
			kprintn(put, ul, 8);
			break;
		case 'u':
			ul = lflag ?
			    va_arg(ap, unsigned long) : va_arg(ap, unsigned long);
			kprintn(put, ul, 10);
			break;
		case 'p':
			put('0');
			put('x');
			lflag = 1;
			/* fall through */
		case 'x':
			ul = lflag ?
			    va_arg(ap, unsigned long) : va_arg(ap, unsigned long);
			kprintn(put, ul, 16);
			break;
		default:
			put('%');
			if (lflag)
				put('l');
			put(ch);
		}
	}
	va_end(ap);
}

static void
kprintn(put, ul, base)
	void (*put)(char);
	unsigned long ul;
	int base;
{
					/* hold a long in base 8 */
	char *p, buf[(sizeof(long) * 8 / 3) + 1];

	p = buf;
	do {
		*p++ = "0123456789abcdef"[ul % base];
	} while (ul /= base);
	do {
		put(*--p);
	} while (p > buf);
}




//--------------------------------------------------------------------------------





