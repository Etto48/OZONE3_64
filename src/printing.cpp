#include "include/printing.h"

uint8_t curx = 0, cury = 0;
uint8_t saved_curx = 0, saved_cury = 0;
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

uint64_t get_number(const char *&str)
{
	uint64_t len = 0;
	while (*str >= '0' && *str <= '9')
	{
		len++;
		str++;
	}
	uint64_t ret = 0;
	uint64_t mult = 1;
	for (uint64_t i = 1; i <= len; i++)
	{
		ret += (str[-i] - '0') * mult;
		mult *= 10;
	}
	return ret;
}

bool set_color(uint64_t num, const char *&str)
{
	if (num >= 30 && num <= 39) //foreground
	{
		switch (num - 30)
		{
		case 0: //black
			default_color = default_color & 0xf0 | 0;
			break;
		case 1: //red
			default_color = default_color & 0xf0 | 0x04;
			break;
		case 2: //green
			default_color = default_color & 0xf0 | 0x02;
			break;
		case 3: //yellow
			default_color = default_color & 0xf0 | 0x06;
			break;
		case 4: //blue
			default_color = default_color & 0xf0 | 0x01;
			break;
		case 5: //magenta
			default_color = default_color & 0xf0 | 0x05;
			break;
		case 6: //cyan
			default_color = default_color & 0xf0 | 0x03;
			break;
		case 7: //white
			default_color = default_color & 0xf0 | 0x07;
			break;
		case 8: //all vga colors
		{
			if (*str != ';')
				return false;
			auto mode_num = get_number(++str);
			if (*str != ';')
				return false;

			if (mode_num == 5)
			{
				auto color_number = get_number(++str);
				switch (color_number)
				{
				case 0:
					default_color = default_color & 0xf0 | 0x0;
					break;
				case 1:
					default_color = default_color & 0xf0 | 0x4;
					break;
				case 2:
					default_color = default_color & 0xf0 | 0x2;
					break;
				case 3:
					default_color = default_color & 0xf0 | 0x6;
					break;
				case 4:
					default_color = default_color & 0xf0 | 0x1;
					break;
				case 5:
					default_color = default_color & 0xf0 | 0x5;
					break;
				case 6:
					default_color = default_color & 0xf0 | 0x3;
					break;
				case 7:
					default_color = default_color & 0xf0 | 0x7;
					break;
				case 8:
					default_color = default_color & 0xf0 | 0x8;
					break;
				case 9:
					default_color = default_color & 0xf0 | 0xc;
					break;
				case 10:
					default_color = default_color & 0xf0 | 0xa;
					break;
				case 11:
					default_color = default_color & 0xf0 | 0xe;
					break;
				case 12:
					default_color = default_color & 0xf0 | 0x9;
					break;
				case 13:
					default_color = default_color & 0xf0 | 0xd;
					break;
				case 14:
					default_color = default_color & 0xf0 | 0xb;
					break;
				case 15:
					default_color = default_color & 0xf0 | 0xf;
					break;
				default:
					return false;
					break;
				}
			}
			break;
		}
		case 9: //default
			default_color = default_color & 0xf0 | 0x07;
			break;
		}
	}
	else if (num >= 40 && num <= 49) //background
	{
		switch (num - 40)
		{
		case 0: //black
			default_color = default_color & 0x0f | 0;
			break;
		case 1: //red
			default_color = default_color & 0x0f | 0x40;
			break;
		case 2: //green
			default_color = default_color & 0x0f | 0x20;
			break;
		case 3: //yellow
			default_color = default_color & 0x0f | 0x60;
			break;
		case 4: //blue
			default_color = default_color & 0x0f | 0x10;
			break;
		case 5: //magenta
			default_color = default_color & 0x0f | 0x50;
			break;
		case 6: //cyan
			default_color = default_color & 0x0f | 0x30;
			break;
		case 7: //white
			default_color = default_color & 0x0f | 0x70;
			break;
		case 8: //all vga colors
		{
			if (*str != ';')
				return false;
			auto mode_num = get_number(++str);
			if (*str != ';')
				return false;
			if (mode_num == 5)
			{
				auto color_number = get_number(++str);
				switch (color_number)
				{
				case 0:
					default_color = default_color & 0x0f | 0x00;
					break;
				case 1:
					default_color = default_color & 0x0f | 0x40;
					break;
				case 2:
					default_color = default_color & 0x0f | 0x20;
					break;
				case 3:
					default_color = default_color & 0x0f | 0x60;
					break;
				case 4:
					default_color = default_color & 0x0f | 0x10;
					break;
				case 5:
					default_color = default_color & 0x0f | 0x50;
					break;
				case 6:
					default_color = default_color & 0x0f | 0x30;
					break;
				case 7:
					default_color = default_color & 0x0f | 0x70;
					break;
				case 8:
					default_color = default_color & 0x0f | 0x80;
					break;
				case 9:
					default_color = default_color & 0x0f | 0xc0;
					break;
				case 10:
					default_color = default_color & 0x0f | 0xa0;
					break;
				case 11:
					default_color = default_color & 0x0f | 0xe0;
					break;
				case 12:
					default_color = default_color & 0x0f | 0x90;
					break;
				case 13:
					default_color = default_color & 0x0f | 0xd0;
					break;
				case 14:
					default_color = default_color & 0x0f | 0xb0;
					break;
				case 15:
					default_color = default_color & 0x0f | 0xf0;
					break;
				default:
					return false;
					break;
				}
			}
			break;
		}
		case 9: //default
			default_color = default_color & 0x0f | 0;
			break;
		}
	}
	else if (num == 0 && *str == 'm')
	{
		default_color = 0x07;
	}
	else
	{
		return false;
	}
	if (*str == ';')
		return set_color(get_number(++str), str);
	else if (*str != 'm')
		return false;
	return true;
}

bool parse_ansi(const char *&str)
{									//wip
	if (*str >= '0' && *str <= '9') //it's a number
	{
		auto num = get_number(str);
		switch (*str)
		{
		case 'K': //erase line
			switch (num)
			{
			case 0: //clear from here to end line
				for (uint8_t i = curx; i < 80; i++)
					put_char(' ', default_color, i, cury);
				break;
			case 1: //clear from line beginning to here
				for (uint8_t i = 0; i <= curx; i++)
					put_char(' ', default_color, i, cury);
				break;
			case 2: //clear from line beginning to end line
				for (uint8_t i = 0; i < 80; i++)
					put_char(' ', default_color, i, cury);
				break;
			default: //error
				return false;
				break;
			}
			break;
		case 'S': //scroll up n lines
			for (uint8_t i = 0; i < num; i++)
				kscroll();
			break;
		case 'T'://scroll down n lines
			break;
		case 'J': //clear display
			switch (num)
			{
			case 0: //clear from here to end
				for (uint8_t y = cury; y < 25; y++)
					for (uint8_t x = y == cury ? curx : 0; x < 80; x++)
						put_char(' ', default_color, x, y);
				break;
			case 1: //clear from beginning to here
				for (uint8_t y = 0; y <= cury; y++)
					for (uint8_t x = 0; x < y == cury ? curx : 80; x++)
						put_char(' ', default_color, x, y);
				break;
			case 2: //clear from beginning to end
				clear(default_color);
				break;
			default: //error
				return false;
				break;
			}
			break;
		case 'A':
			cury -= min(num, cury);
			break;
		case 'B':
			cury += max(num, 25-cury);
			break;
		case 'C':
			curx += max(num, 80-curx);
			break;
		case 'D':
			curx -= min(num, curx);
			break;
		case 'E':
			curx = 0;
			cury += max(num, 25-cury);
			break;
		case 'F':
			curx = 0;
			cury -= min(num, cury);
			break;
		case 'G':
			curx = num%80;
			break;
		case 'm': //change color
		case ';': //2 or more numbers
		{
			auto old_color = default_color;
			auto old_str = str;
			if (!set_color(num, str))
			{ //it is not a color
				str = old_str;
				default_color = old_color;
				uint64_t num2 = get_number(++str);
				switch (*str)
				{
				case 'H': //cursor position
					curx = num % 80;
					cury = num2 % 25;
					break;

				default:
					break;
				}
			}
			break;
		}
		default:
			return false;
			break;
		}
	}
	else //it's not a number
	{
		switch (*str)
		{
		case 's':
			saved_curx = curx;
			saved_cury = cury;
			break;
		case 'u':
			curx = saved_curx;
			cury = saved_cury;
			break;

		default:
			return false;
			break;
		}
	}
	return true;
}

void kprint(const char *string)
{
	while (*string != 0)
	{
		switch (*string)
		{
		case '\n':
			do
			{
				put_char(' ', default_color, curx++, cury);
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
				parse_ansi(++string);
				break;
			// case 'c':
			// 	color = *++string;
			// 	break;
			// case '0':
			// 	color = default_color;
			default:
				break;
			}
			++string;
			break;
		default:
			put_char(*string++, default_color, curx++, cury);
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