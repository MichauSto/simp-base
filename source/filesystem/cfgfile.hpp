#pragma once

#include <string>

namespace simp {

  struct CfgFile {
    CfgFile(std::string _data) : Data(std::move(_data)), CursorPos(0), LineIndex(1) {}
    bool LineEmpty() const;
    bool AtEnd() const;
    bool TestTag(const std::string_view& tag, bool advance = true) const;
    std::string_view GetLine(bool advance = true) const;
    std::string_view GetLineTrimmed(bool advance = true) const;
    std::string::const_iterator GetLineBegin() const;
    std::string::const_iterator GetLineEnd() const;
    float ReadFloat() const;
    bool ReadFloat(float& value) const;
    bool ReadFloats(float* buf, int count) const;
    int ReadInt() const;
    bool ReadInt(int& value) const;
    bool ReadInts(int* buf, int count) const;
    void SkipEmptyLines() const;
    void NextLine() const;
    void Reset() const;
    std::string Data;
    size_t GetLineIndex() const;
    mutable size_t CursorPos;
    mutable size_t LineIndex;
  };

}