#ifndef INFILE_H
#define INFILE_H

#include <fstream>
#include <string>
#include <cstdint>
#include <vector>

class InFile
{
public:
  enum OpenMode : uint64_t
  {
    BINARY = 1 << 0
  };

  InFile(const std::string& path);
  InFile(const std::string& path, const uint64_t& open_mode);
  virtual ~InFile();

  size_t length();
  bool isOpen() const;
  void position(const size_t& pos);
  void positionRelative(const size_t& offset);
  size_t getPosition() const;

  unsigned int getUnsignedInt();
  std::vector<unsigned int> getUnsignedIntVector(const size_t& int_cnt);
  unsigned short getUnsignedShort();
  std::vector<unsigned short> getUnsignedShortVector(const size_t& short_cnt);
  float getFloat();
  std::vector<float> getFloatVector(const size_t& float_cnt);
  unsigned char getUnsignedChar();
  std::vector<unsigned char> getUnsignedCharVector(const size_t& char_cnt);
  std::string getString();

  std::string toString();
  std::vector<unsigned char> toUnsignedCharVector();
  std::vector<char> toCharVector();

  void cache();

private:
  std::ifstream file_;
  bool cached_;
  std::vector<unsigned char> cache_;
  uint64_t mode_;
  size_t position_;
};

#endif // INFILE_H
