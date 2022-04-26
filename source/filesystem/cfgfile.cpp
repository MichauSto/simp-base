#include "cfgfile.hpp"

#include <charconv>

bool simp::CfgFile::LineEmpty() const
{
  if (AtEnd()) return false;
  auto end = GetLineEnd();
  return std::find_if(GetLineBegin(), end, std::isprint) == end;
}

bool simp::CfgFile::AtEnd() const
{
  return GetLineBegin() == Data.end();
}

bool simp::CfgFile::TestTag(const std::string_view& tag, bool advance) const
{
  auto line = GetLine(false);
  if (line.length() != tag.length()) 
    return false;
  if (_strnicmp(line.data(), tag.data(), tag.length())) 
    return false;
  if(advance) NextLine();
  return true;
}

std::string_view simp::CfgFile::GetLine(bool advance) const
{
  std::string_view line{ GetLineBegin(), GetLineEnd() };
  if (advance) NextLine();
  return { line.begin(), std::find_if(line.rbegin(), line.rend(), std::isprint).base()};
}

std::string_view simp::CfgFile::GetLineTrimmed(bool advance) const
{
  auto line = GetLine(advance);
  return { std::find_if(line.begin(), line.end(), std::isprint), line.end()};
}

std::string::const_iterator simp::CfgFile::GetLineBegin() const
{
  return Data.begin() + CursorPos;
}

std::string::const_iterator simp::CfgFile::GetLineEnd() const
{
  return std::find(GetLineBegin(), Data.end(), '\n');
}

float simp::CfgFile::ReadFloat() const
{
  float result = 0.f;
  ReadFloat(result);
  return result;
}

bool simp::CfgFile::ReadFloat(float& value) const
{
  auto line = GetLine(true);
  auto result = std::from_chars(line.data(), line.data() + line.size(), value);
  if ((bool)result.ec || result.ptr != line.data() + line.size()) return false;
  return true;
}

bool simp::CfgFile::ReadFloats(float* buf, int count) const
{
  for (int i = 0; i < count; ++i) {
    if (!ReadFloat(buf[i]))
      return false;
  }
  return true;
}

int simp::CfgFile::ReadInt() const
{
  int result = 0;
  ReadInt(result);
  return result;
}

bool simp::CfgFile::ReadInt(int& value) const
{
  auto line = GetLine(true);
  auto result = std::from_chars(line.data(), line.data() + line.size(), value);
  if ((bool)result.ec || result.ptr != line.data() + line.size()) return false;
  return true;
}

bool simp::CfgFile::ReadInts(int* buf, int count) const
{
  for (int i = 0; i < count; ++i) {
    if (!ReadInt(buf[i]))
      return false;
  }
  return true;
}

void simp::CfgFile::SkipEmptyLines() const
{
  while (LineEmpty()) NextLine();
}

void simp::CfgFile::NextLine() const
{
  auto end = GetLineEnd();
  if (end != Data.end()) CursorPos = (end - Data.begin()) + 1;
  else CursorPos = Data.end() - Data.begin();
  ++LineIndex;
}

void simp::CfgFile::Reset() const
{
  CursorPos = 0;
  LineIndex = 1;
}

size_t simp::CfgFile::GetLineIndex() const
{
  return LineIndex;
}
