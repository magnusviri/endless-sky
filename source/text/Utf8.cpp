/* Utf8.cpp
Copyright (c) 2017, 2018 by Flavio J. Saraiva

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Utf8.h"

#if defined(_WIN32)
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;

namespace {
	constexpr char32_t BOM = 0x0000FEFF;
}



namespace Utf8 {
#if defined(_WIN32)
	wstring ToUTF16(const string &input, bool isPath)
	{
		const auto page = CP_UTF8;
		wstring result;
		if(input.empty())
			return result;

		bool endsInSlash = isPath && (input.back() == '/' || input.back() == '\\');
		int size = MultiByteToWideChar(page, 0, &input[0], input.length() - endsInSlash, nullptr, 0);
		result.resize(size);
		MultiByteToWideChar(page, 0, &input[0], input.length() - endsInSlash, &result[0], size);

		return result;
	}



	string ToUTF8(const wchar_t *str)
	{
		string result;
		if(!str || !*str)
			return result;

		const auto page = CP_UTF8;
		// The returned size will include the null character at the end.
		int size = WideCharToMultiByte(page, 0, str, -1, nullptr, 0, nullptr, nullptr) - 1;
		result.resize(size);
		WideCharToMultiByte(page, 0, str, -1, &result[0], size, nullptr, nullptr);

		return result;
	}
#endif



	// Check if this character is the byte order mark (BOM) sequence.
	bool IsBOM(char32_t c)
	{
		return c == BOM;
	}



	size_t NextCodePoint(const string &str, size_t pos)
	{
		if(pos >= str.length())
			return string::npos;

		for(++pos; pos < str.length(); ++pos)
			if((str[pos] & 0x80) == 0 || (str[pos] & 0xc0) == 0xc0)
				break;
		return pos;
	}



	// Returns the start of the unicode code point at pos in utf8.
	size_t CodePointStart(const string &str, size_t pos)
	{
		// 0xxxxxxx and 11?????? start a code point
		while(pos > 0 && (str[pos] & 0x80) != 0x00 && (str[pos] & 0xc0) != 0xc0)
			--pos;
		return pos;
	}



	// Determines the number of bytes used by the unicode code point in utf8.
	int CodePointBytes(const char *str)
	{
		// end - 00000000
		if(!str || !*str)
			return 0;

		// 1 byte - 0xxxxxxx
		if((*str & 0x80) == 0)
			return 1;

		// invalid - 10?????? or 11?????? invalid
		if((*str & 0x40) == 0 || (*(str + 1) & 0xc0) != 0x80)
			return -1;

		// 2 bytes - 110xxxxx 10xxxxxx
		if((*str & 0x20) == 0)
			return 2;

		// invalid - 111????? 10?????? invalid
		if((*(str + 2) & 0xc0) != 0x80)
			return -1;

		// 3 bytes - 1110xxxx 10xxxxxx 10xxxxxx
		if((*str & 0x10) == 0)
			return 3;

		// invalid - 1111???? 10?????? 10?????? invalid
		if((*(str + 3) & 0xc0) != 0x80)
			return -1;

		// 4 bytes - 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		if((*str & 0x8) == 0)
			return 4;

		// not unicode - 11111??? 10?????? 10?????? 10??????
		return -1;
	}



	// Decodes a unicode code point in utf8.
	// Invalid codepoints are converted to 0xFFFFFFFF.
	char32_t DecodeCodePoint(const string &str, size_t &pos)
	{
		if(pos >= str.length())
		{
			pos = string::npos;
			return 0;
		}

		// invalid (-1) or end (0)
		int bytes = CodePointBytes(str.c_str() + pos);
		if(bytes < 1)
		{
			++pos;
			return bytes;
		}

		// 1 byte
		if(bytes == 1)
			return (str[pos++] & 0x7f);

		// 2-4 bytes
		char32_t c = (str[pos++] & ((1 << (7 - bytes)) - 1));
		for(int i = 1; i < bytes; ++i)
			c = (c << 6) + (str[pos++] & 0x3f);
		return c;
	}
}
