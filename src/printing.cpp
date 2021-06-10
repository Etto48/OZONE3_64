#include "include/printing.h"

uint8_t curx = 0, cury = 0;
uint8_t default_color = 0x07;
void put_char(char c, uint8_t color, uint8_t x, uint8_t y)
{
	((char *)0xb8000)[(x % 80 + y * 80) * 2] = c;
	((char *)0xb8000)[(x % 80 + y * 80) * 2 + 1] = color;
}
void clear(uint8_t color)
{
	default_color = color;
	for (uint16_t y = 0; y < 25; y++)
		for (uint16_t x = 0; x < 80; x++)
			put_char(' ', color, x, y);
	curx = 0;
	cury = 0;
}
void kscroll()
{
	for (uint32_t i = 0; i < 80 * 24 * 2; i++)
	{
		((char *)0xb8000)[i] = ((char *)0xb8000)[i + 80 * 2];
	}
	for (uint32_t i = 80 * 24 * 2; i < 80 * 25 * 2; i += 2)
	{
		((char *)0xb8000)[i] = ' ';
		((char *)0xb8000)[i + 1] = default_color;
	}
}

void parse_ansi(const char *&str)
{//wip
	switch (*++str)
	{
	case '0':
		switch (*++str)
		{
			case 'J'://clear from cursor to endscreen
				break;
			case 'K'://clear from cursor to end of line
				break;
			case 'm'://reset modes
				break;
		}
		break;
	case 'm':
		break;
	case ';':
		parse_ansi(str);
		break;
	}
}

void kprint(const char *string)
{
	uint8_t color = default_color;
	while (*string != 0)
	{
		switch (*string)
		{
		case '\n':
			do
			{
				put_char(' ', color, curx++, cury);
			} while (curx < 80);
			curx = 0;
			cury++;
			string++;
			break;
		case '\r':
			curx = 0;
			string++;
			break;
		case '\033':
			switch (*++string)
			{
			case '[':
				//ansi escape sequence
				parse_ansi(string);
				break;
			case 'c':
				color = *++string;
				break;
			case '0':
				color = default_color;
			default:
				break;
			}
			++string;
			break;
		default:
			put_char(*string++, color, curx++, cury);
			cury += curx / 80;
			curx %= 80;
			break;
		}
		if (cury == 25)
		{
			cury--;
			kscroll();
		}
	}
}



void println(const char *str)
{
	if (str) //super wrong, check if str is in process memory
		kprint(str);
	kprint("\n");
}

int printf(const char *fmt, ...)
{
	va_list ap;
	constexpr size_t size = 2048;
	char buf[size];
	int l;

	va_start(ap, fmt);
	l = vsnprintf(buf, size, fmt, ap);
	va_end(ap);

	kprint(buf);

	return l;
}
#define DEC_BUFSIZE 65
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
	size_t in = 0, out = 0, tmp;
	char *aux, buf[DEC_BUFSIZE];
	void *addr;
	int cifre;
	bool islong;
	bool isunsigned;
	size_t cur_size;

	while (out < size - 1 && fmt[in])
	{
		switch (fmt[in])
		{
		case '%':
			islong = false;
			isunsigned = false;
			cifre = -1;
			cur_size = size - 1;
			// numero di cifre
			if (fmt[in + 1] >= '1' && fmt[in + 1] <= '9')
			{
				in++;
				cifre = fmt[in] - '0';
			}
			// limite per %s
			if (fmt[in + 1] == '.' && fmt[in + 2] == '*')
			{
				cur_size = out + va_arg(ap, size_t);
				if (cur_size < out || cur_size > size - 1)
					cur_size = size - 1;
				in += 2;
			}
			// long?

			if (fmt[in + 1] == 'u')
			{
				isunsigned = true;
				if (cifre < 0)
					cifre = 16;
				in++;
			}

			if (fmt[in + 1] == 'l')
			{
				islong = true;
				if (cifre < 0)
					cifre = 16;
				in++;
			}
			switch (fmt[++in])
			{
			case '\0':
				goto end;
			case 'p':
				if (islong)
					goto end;
				addr = va_arg(ap, void *);
				cifre = sizeof(void *) * 2;
				if (out > size - (cifre + 1))
					goto end;
				string_tools::htostr(&str[out], (unsigned long long)addr, cifre);
				out += cifre;
				break;
			case 'd':
				tmp = (islong ? (isunsigned ? va_arg(ap, unsigned long) : va_arg(ap, long)) : (isunsigned ? va_arg(ap, unsigned int) : va_arg(ap, int)));
				if (isunsigned)
					string_tools::utoa(tmp, buf, 10);
				else
					string_tools::itoa(tmp, buf, 10);
				if (string_tools::strlen(buf) > size - out - 1)
					goto end;
				for (aux = buf; *aux; ++aux)
					str[out++] = *aux;
				break;
			case 'x':
				tmp = (islong ? (isunsigned ? va_arg(ap, unsigned long) : va_arg(ap, long)) : (isunsigned ? va_arg(ap, unsigned int) : va_arg(ap, int)));
				if (isunsigned)
					string_tools::utoa(tmp, buf, 16);
				else
					string_tools::itoa(tmp, buf, 16);
				if (string_tools::strlen(buf) > size - out - 1)
					goto end;
				for (aux = buf; *aux; ++aux)
					str[out++] = *aux;
				break;
			case 'b':
				tmp = (islong ? (isunsigned ? va_arg(ap, unsigned long) : va_arg(ap, long)) : (isunsigned ? va_arg(ap, unsigned int) : va_arg(ap, int)));
				if (islong)
					string_tools::utoa(tmp, buf, 2, 64);
				else
					string_tools::utoa(tmp, buf, 2, 32);
				if (string_tools::strlen(buf) > size - out - 1)
					goto end;
				for (aux = buf; *aux; ++aux)
					str[out++] = *aux;
				break;
			case 's':
				if (islong)
					goto end;
				aux = va_arg(ap, char *);
				while (out < cur_size && *aux)
					str[out++] = *aux++;
				break;
			case 'c':
				if (islong)
					goto end;
				tmp = va_arg(ap, int);
				if (out < size - 1)
					str[out++] = tmp;
			}
			++in;
			break;
		default:
			str[out++] = fmt[in++];
		}
	}
end:
	str[out++] = 0;

	return out;
}
int snprintf(char *buf, uint32_t n, const char *fmt, ...)
{
	va_list ap;
	int l;

	va_start(ap, fmt);
	l = vsnprintf(buf, n, fmt, ap);
	va_end(ap);

	return l;
}