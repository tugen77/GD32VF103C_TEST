#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "uart.h"

void xitoa(long val, int radix, int len)
{
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[20], i = 0;
	unsigned long v;


	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
		uartPutchar(s[--i]);
	while (i);
}

void xitoab(unsigned char *buf, long val, int radix, int len)
{
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[20], i = 0;
	unsigned long v;
	
	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
		*buf++ = s[--i];
	while (i);
}

void qprintf(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;


	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			uartPutchar(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (s) w = -w;
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			uartPutstring((unsigned char *)va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			uartPutchar((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X') r = 16;
		if (d == 'x') r = 16;
		if (d == 'b') r = 2;
		if (!r) break;
		if (l) {
			xitoa((long)va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				xitoa((unsigned long)va_arg(arp, int), r, w);
			else
				xitoa((long)va_arg(arp, int), r, w);
		}
	}
	va_end(arp);
}


void qsprintf(unsigned char* ostr, const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;
	unsigned char buf[8], *ptr;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		memset(buf, 0x00, sizeof(buf));
		if (d != '%') {
			*ostr++ = d; 
			continue;
		}
		d = *str++; w = r = s = l = 0;
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (s) w = -w;
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			ptr = (unsigned char *)va_arg(arp, char*);
			while(*ptr)
					*ostr++ = *ptr++;
			continue;
		}
		if (d == 'c') {
			*ostr++ = (char)va_arg(arp, int); 
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X') r = 16;
		if (d == 'x') r = 16;
		if (d == 'b') r = 2;
		if (!r) break;
		if (l) {
			xitoab(buf, (long)va_arg(arp, long), r, w);
			ptr = (unsigned char *)&buf;
			while(*ptr)
				*ostr++ = *ptr++;
		} else {
			if (r > 0)
			{
				xitoab(buf, (unsigned long)va_arg(arp, int), r, w);
				ptr = (unsigned char *)&buf;
				while(*ptr)
					*ostr++ = *ptr++;
			} else {
				xitoab(buf, (long)va_arg(arp, int), r, w);
				ptr = (unsigned char *)&buf;
				while(*ptr)
					*ostr++ = *ptr++;
			}
		}
	}

	va_end(arp);
}

unsigned char bccCheck(unsigned char *dat, unsigned char length)
{
    unsigned char       bcc, l;

    bcc = 0;
    if(length)
    {
        l = length;
        while(l)
        {
            bcc ^= dat[length - l];
            l--;
        }
    }

    return bcc;
}



