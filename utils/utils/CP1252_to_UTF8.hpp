/*
  MIT License

  Copyright (c) 2017 Arthur Sonzogni

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <iostream>
#include <locale>
#include <string>

static const wchar_t CP1252_UNICODE_TABLE[] =
    L"\u0000\u0001\u0002\u0003\u0004\u0005\u0006\u0007"
    L"\u0008\u0009\u000A\u000B\u000C\u000D\u000E\u000F"
    L"\u0010\u0011\u0012\u0013\u0014\u0015\u0016\u0017"
    L"\u0018\u0019\u001A\u001B\u001C\u001D\u001E\u001F"
    L"\u0020\u0021\u0022\u0023\u0024\u0025\u0026\u0027"
    L"\u0028\u0029\u002A\u002B\u002C\u002D\u002E\u002F"
    L"\u0030\u0031\u0032\u0033\u0034\u0035\u0036\u0037"
    L"\u0038\u0039\u003A\u003B\u003C\u003D\u003E\u003F"
    L"\u0040\u0041\u0042\u0043\u0044\u0045\u0046\u0047"
    L"\u0048\u0049\u004A\u004B\u004C\u004D\u004E\u004F"
    L"\u0050\u0051\u0052\u0053\u0054\u0055\u0056\u0057"
    L"\u0058\u0059\u005A\u005B\u005C\u005D\u005E\u005F"
    L"\u0060\u0061\u0062\u0063\u0064\u0065\u0066\u0067"
    L"\u0068\u0069\u006A\u006B\u006C\u006D\u006E\u006F"
    L"\u0070\u0071\u0072\u0073\u0074\u0075\u0076\u0077"
    L"\u0078\u0079\u007A\u007B\u007C\u007D\u007E\u007F"
    L"\u20AC\u0020\u201A\u0192\u201E\u2026\u2020\u2021"
    L"\u02C6\u2030\u0160\u2039\u0152\u0020\u017D\u0020"
    L"\u0020\u2018\u2019\u201C\u201D\u2022\u2013\u2014"
    L"\u02DC\u2122\u0161\u203A\u0153\u0020\u017E\u0178"
    L"\u00A0\u00A1\u00A2\u00A3\u00A4\u00A5\u00A6\u00A7"
    L"\u00A8\u00A9\u00AA\u00AB\u00AC\u00AD\u00AE\u00AF"
    L"\u00B0\u00B1\u00B2\u00B3\u00B4\u00B5\u00B6\u00B7"
    L"\u00B8\u00B9\u00BA\u00BB\u00BC\u00BD\u00BE\u00BF"
    L"\u00C0\u00C1\u00C2\u00C3\u00C4\u00C5\u00C6\u00C7"
    L"\u00C8\u00C9\u00CA\u00CB\u00CC\u00CD\u00CE\u00CF"
    L"\u00D0\u00D1\u00D2\u00D3\u00D4\u00D5\u00D6\u00D7"
    L"\u00D8\u00D9\u00DA\u00DB\u00DC\u00DD\u00DE\u00DF"
    L"\u00E0\u00E1\u00E2\u00E3\u00E4\u00E5\u00E6\u00E7"
    L"\u00E8\u00E9\u00EA\u00EB\u00EC\u00ED\u00EE\u00EF"
    L"\u00F0\u00F1\u00F2\u00F3\u00F4\u00F5\u00F6\u00F7"
    L"\u00F8\u00F9\u00FA\u00FB\u00FC\u00FD\u00FE\u00FF";

// Convert a
// * Windows-1252 (aka CP1252) stored as an std::string
// into an
// * UTF8 std::string.
std::string CP1252_to_UTF8(const char* data, size_t size) {

  // Byte array => Unicode.
  std::wstring unicode(size, ' ');
  for (size_t i = 0; i < unicode.size(); ++i)
    unicode[i] = CP1252_UNICODE_TABLE[(uint8_t)data[i]];

  // Unicode => UTF8.
  size_t utfSize = 0;
  wcstombs_s(&utfSize, nullptr, 0, unicode.c_str(), 0);
  std::string utf(utfSize - 1, ' ');
  wcstombs_s(nullptr, utf.data(), utfSize, unicode.c_str(), _TRUNCATE);
  return utf;
}

// Convert a
// * Windows-1252 (aka CP1252) stored as an std::string
// into an
// * UTF8 std::string.
std::string CP1252_to_UTF8(const std::string& byte_array) {
  return CP1252_to_UTF8(byte_array.data(), byte_array.size());
}