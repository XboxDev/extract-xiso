#ifndef CP1252_INCLUDED
#define CP1252_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

uint32_t getUnicodeFromCP1252(char input);
char getCP1252FromUnicode(uint32_t input);
int getUTF8CodePointSize(uint32_t input);
char* getUTF8Sequence(uint32_t input, char* buf);
size_t strcplen(const char* buf, bool utf8);
const char* getCodePointFromUTF8Sequence(const char* buf, uint32_t* out);
char* getUTF8String(const char* input);
char* getCP1252String(const char* input);
char toupperCP1252(char c);

uint32_t getUnicodeFromCP1252(char input) {
	switch ((unsigned char)input) {
	case 0x80u:
		return 0x20ACu;	// EURO SIGN
	case 0x82u:
		return 0x201Au;	// SINGLE LOW-9 QUOTATION MARK
	case 0x83u:
		return 0x0192u;	// LATIN SMALL LETTER F WITH HOOK
	case 0x84u:
		return 0x201Eu;	// DOUBLE LOW-9 QUOTATION MARK
	case 0x85u:
		return 0x2026u;	// HORIZONTAL ELLIPSIS
	case 0x86u:
		return 0x2020u;	// DAGGER
	case 0x87u:
		return 0x2021u;	// DOUBLE DAGGER
	case 0x88u:
		return 0x02C6u;	// MODIFIER LETTER CIRCUMFLEX ACCENT
	case 0x89u:
		return 0x2030u;	// PER MILLE SIGN
	case 0x8Au:
		return 0x0160u;	// LATIN CAPITAL LETTER S WITH CARON
	case 0x8Bu:
		return 0x2039u;	// SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	case 0x8Cu:
		return 0x0152u;	// LATIN CAPITAL LIGATURE OE
	case 0x8Eu:
		return 0x017Du;	// LATIN CAPITAL LETTER Z WITH CARON
	case 0x91u:
		return 0x2018u;	// LEFT SINGLE QUOTATION MARK
	case 0x92u:
		return 0x2019u;	// RIGHT SINGLE QUOTATION MARK
	case 0x93u:
		return 0x201Cu;	// LEFT DOUBLE QUOTATION MARK
	case 0x94u:
		return 0x201Du;	// RIGHT DOUBLE QUOTATION MARK
	case 0x95u:
		return 0x2022u;	// BULLET
	case 0x96u:
		return 0x2013u;	// EN DASH
	case 0x97u:
		return 0x2014u;	// EM DASH
	case 0x98u:
		return 0x02DCu;	// SMALL TILDE
	case 0x99u:
		return 0x2122u;	// TRADE MARK SIGN
	case 0x9Au:
		return 0x0161u;	// LATIN SMALL LETTER S WITH CARON
	case 0x9Bu:
		return 0x203Au;	// SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	case 0x9Cu:
		return 0x0153u;	// LATIN SMALL LIGATURE OE
	case 0x9Eu:
		return 0x017Eu;	// LATIN SMALL LETTER Z WITH CARON
	case 0x9Fu:
		return 0x0178u;	// LATIN CAPITAL LETTER Y WITH DIAERESIS
	default:
		return (unsigned char)input;
	}
}

char getCP1252FromUnicode(uint32_t input) {
	switch (input) {
	case 0x20ACu:
		return '\x80';	// EURO SIGN
	case 0x201Au:
		return '\x82';	// SINGLE LOW-9 QUOTATION MARK
	case 0x0192u:
		return '\x83';	// LATIN SMALL LETTER F WITH HOOK
	case 0x201Eu:
		return '\x84';	// DOUBLE LOW-9 QUOTATION MARK
	case 0x2026u:
		return '\x85';	// HORIZONTAL ELLIPSIS
	case 0x2020u:
		return '\x86';	// DAGGER
	case 0x2021u:
		return '\x87';	// DOUBLE DAGGER
	case 0x02C6u:
		return '\x88';	// MODIFIER LETTER CIRCUMFLEX ACCENT
	case 0x2030u:
		return '\x89';	// PER MILLE SIGN
	case 0x0160u:
		return '\x8A';	// LATIN CAPITAL LETTER S WITH CARON
	case 0x2039u:
		return '\x8B';	// SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	case 0x0152u:
		return '\x8C';	// LATIN CAPITAL LIGATURE OE
	case 0x017Du:
		return '\x8E';	// LATIN CAPITAL LETTER Z WITH CARON
	case 0x2018u:
		return '\x91';	// LEFT SINGLE QUOTATION MARK
	case 0x2019u:
		return '\x92';	// RIGHT SINGLE QUOTATION MARK
	case 0x201Cu:
		return '\x93';	// LEFT DOUBLE QUOTATION MARK
	case 0x201Du:
		return '\x94';	// RIGHT DOUBLE QUOTATION MARK
	case 0x2022u:
		return '\x95';	// BULLET
	case 0x2013u:
		return '\x96';	// EN DASH
	case 0x2014u:
		return '\x97';	// EM DASH
	case 0x02DCu:
		return '\x98';	// SMALL TILDE
	case 0x2122u:
		return '\x99';	// TRADE MARK SIGN
	case 0x0161u:
		return '\x9A';	// LATIN SMALL LETTER S WITH CARON
	case 0x203Au:
		return '\x9B';	// SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	case 0x0153u:
		return '\x9C';	// LATIN SMALL LIGATURE OE
	case 0x017Eu:
		return '\x9E';	// LATIN SMALL LETTER Z WITH CARON
	case 0x0178u:
		return '\x9F';	// LATIN CAPITAL LETTER Y WITH DIAERESIS
	case 0x0081u:
	case 0x008Du:
	case 0x008Fu:
	case 0x0090u:
	case 0x009Du:
		return (char)input;
	default:
		if (input <= 0x007Fu || (input >= 0x00A0u && input <= 0x00FFu)) {
			return (char)input;
		}
		else {
			return '\x3F';	// QUESTION MARK
		}
	}
}

int getUTF8CodePointSize(uint32_t input) {
	if (input < 0x80u) {
		return 1;
	}
	else if (input < 0x800u) {
		return 2;
	}
	else if (input < 0x10000u) {
		return 3;
	}
	else if (input < 0x110000u) {
		return 4;
	}
	else {
		return 0;
	}
}

char* getUTF8Sequence(uint32_t input, char* buf) {
	if (input < 0x80u) {
		*buf++ = input & (0xFF >> 1);
	}
	else if (input < 0x800u) {
		*buf++ = 0xC0 | ((input >> 6) & (0xFF >> 3));
		*buf++ = 0x80 | (input & 0x3F);
	}
	else if (input < 0x10000u) {
		*buf++ = 0xE0 | ((input >> 12) & (0xFF >> 4));
		*buf++ = 0x80 | ((input >> 6) & 0x3F);
		*buf++ = 0x80 | (input & 0x3F);
	}
	else if (input < 0x110000u) {
		*buf++ = 0xF0 | ((input >> 18) & (0xFF >> 5));
		*buf++ = 0x80 | ((input >> 12) & 0x3F);
		*buf++ = 0x80 | ((input >> 6) & 0x3F);
		*buf++ = 0x80 | (input & 0x3F);
	}

	return buf;
}

size_t strcplen(const char* buf, bool utf8) {
	unsigned char c;
	size_t count = 0;
	if (!utf8) return strlen(buf);
	while (buf[0] != '\0') {
		c = (unsigned char)buf[0];
		count += 1;
		if ((c >> 7) == 0x00u) {
			buf += 1;
		}
		else if ((c >> 5) == 0x06u) {
			buf += 2;
		}
		else if ((c >> 4) == 0x0Eu) {
			buf += 3;
		}
		else if ((c >> 3) == 0x1Eu) {
			buf += 4;
		}
		else {
			buf += 1;	// Skip invalid char and don't count it
			count -= 1;
		}
	}
	return count;
}

const char* getCodePointFromUTF8Sequence(const char* buf, uint32_t* out) {
	unsigned char c = (unsigned char)buf[0];
	if ((c >> 7) == 0x00) {
		if (out) *out = buf[0] & 0x7F;
		buf += 1;
	}
	else if ((c >> 5) == 0x06) {
		if (out) *out = ((buf[0] & 0x1F) << 6) | (buf[1] & 0x3F);
		buf += 2;
	}
	else if ((c >> 4) == 0x0E) {
		if (out) *out = ((buf[0] & 0xF) << 12) | ((buf[1] & 0x3F) << 6) | (buf[2] & 0x3F);
		buf += 3;
	}
	else if ((c >> 3) == 0x1E) {
		if (out) *out = ((buf[0] & 0x7) << 18) | ((buf[1] & 0x3F) << 12) | ((buf[2] & 0x3F) << 6) | (buf[3] & 0x3F);
		buf += 4;
	}
	else {
		if (out) *out = '\0';
		buf += 1;	// Skip invalid char
	}

	return buf;
}

char* getUTF8String(const char* input) {
	const char* p = input;
	char *ret = NULL, *q;
	size_t len = 0;
	while (*p != '\0') {
		len += getUTF8CodePointSize(getUnicodeFromCP1252(*p++));
	}
	ret = (char*)malloc(len + 1);
	if (!ret) return NULL;
	q = ret;
	while (*input != '\0') {
		q = getUTF8Sequence(getUnicodeFromCP1252(*input++), q);
	}
	*q = '\0';
	return ret;
}

char* getCP1252String(const char* input) {
	size_t len = strcplen(input, true);
	char* ret = malloc(len + 1);
	if (!ret) return NULL;
	char* p = ret;
	uint32_t cp;
	while (*input != '\0') {
		input = getCodePointFromUTF8Sequence(input, &cp);
		if (cp != '\0') *p++ = getCP1252FromUnicode(cp);
	}
	*p = '\0';
	return ret;
}

char toupperCP1252(char c) {
	if (c >= 'a' && c <= 'z') {
		c -= 32;
	}
	else if (c == '\x9A' || c == '\x9C' || c == '\x9E') {
		c -= 16;
	}
	else if ((c >= '\xE0' && c <= '\xF6') || (c >= '\xF8' && c <= '\xFE')) {
		c -= 32;
	}
	else if (c == '\xFF') {
		c = '\x9F';
	}
	return c;
}

#endif //CP1252_INCLUDED
