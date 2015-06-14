#include "InFile.h"
#include <fstream>
#include <iostream>

using std::string;
using std::ifstream;

InFile::InFile(const std::string& path) :
    cached_(false), mode_(0), position_(0)
{
  file_.open(path);
}

InFile::~InFile()
{
  file_.close();
}

InFile::InFile(const std::string& path, const uint64_t& open_mode) :
    cached_(false), mode_(open_mode), position_(0)
{
  std::ios::openmode mode = std::ios::in;
  if (open_mode & OpenMode::BINARY)
    mode |= std::ios::binary;

  file_.open(path, mode);
}

std::string InFile::toString()
{
  string data;
  if (isOpen())
  {
    size_t size = length();
    data = string(size, ' ');
    file_.read(&data[0], size);
  }
  return data;
}

size_t InFile::length()
{
  file_.seekg(0, std::ios::end);
  size_t size = file_.tellg();
  file_.seekg(0, std::ios::beg);
  return size;
}

bool InFile::isOpen() const
{
  return file_.is_open();
}

std::vector<unsigned char> InFile::toUnsignedCharVector()
{
  std::vector<unsigned char> data;
  if (isOpen())
  {
    size_t len = length();
    data.resize(len);
    file_.read(reinterpret_cast<char*>(&data[0]), len);
  }
  return data;
}

void InFile::position(const size_t& pos)
{
  if (!cached_)
    file_.seekg(pos, std::ios::beg);
  position_ = pos;
}

unsigned int InFile::getUnsignedInt()
{
  unsigned int ret = 0;

  if (mode_ & BINARY)
    if (cached_)
      for (int i = 3; i >= 0; i--)
        ret |= cache_[position_ + i] << i * 8;

  position_ += 4;

  return ret;
}

std::vector<char> InFile::toCharVector()
{
  std::vector<char> data;
  if (isOpen())
  {
    size_t len = length();
    data.resize(len);
    file_.read(&data[0], len);
  }
  return data;
}

std::vector<unsigned int> InFile::getUnsignedIntVector(const size_t& int_cnt)
{
  std::vector<unsigned int> ret;
  for (size_t i = 0; i < int_cnt; i++)
    ret.push_back(getUnsignedInt());
  return ret;
}

void InFile::cache()  // TODO: Implement for not-binary mode
{
  if (mode_ & BINARY)
    cache_ = toUnsignedCharVector();
  cached_ = true;
}

std::string InFile::getString()
{
  std::string ret = "";
  if (mode_ & BINARY)
  {
    if (cached_)
    {
      while (cache_[position_] != '\0')
      {
        ret += cache_[position_];
        position_++;
      }
      position_++;
    }
  }
  return ret;
}

void InFile::positionRelative(const size_t& offset)
{
  if (!cached_)
    file_.seekg(offset, std::ios::cur);
  position_ += offset;
}

float InFile::getFloat()
{
  union
  {
    float result;
    unsigned char b[4];
  } u;

  for (int i = 0; i < 4; i++)
    u.b[i] = cache_[position_++];

  return u.result;
}

std::vector<float> InFile::getFloatVector(const size_t& float_cnt)
{
  std::vector<float> ret;
  for (size_t i = 0; i < float_cnt; i++)
    ret.push_back(getFloat());
  return ret;
}

size_t InFile::getPosition() const
{
  return position_;
}

unsigned char InFile::getUnsignedChar()
{
  unsigned char ret = 0;

  if (mode_ & BINARY)
    if (cached_)
      ret = cache_[position_];

  position_++;

  return ret;

}

std::vector<unsigned char> InFile::getUnsignedCharVector(const size_t& char_cnt)
{
  std::vector<unsigned char> ret;
  if (mode_ & BINARY)
    if (cached_)
      for (size_t i = 0; i < char_cnt; i++)
        ret.push_back(getUnsignedChar());

  return ret;
}

unsigned short InFile::getUnsignedShort()
{
  unsigned short ret = 0;
  if (mode_ & BINARY)
    if (cached_)
      for (int i = 1; i >= 0; i--)
        ret |= cache_[position_ + i] << i * 8;
  position_ += 2;
  return ret;
}

std::vector<unsigned short> InFile::getUnsignedShortVector(
    const size_t& short_cnt)
{
  std::vector<unsigned short> ret;
  for (size_t i = 0; i < short_cnt; i++)
    ret.push_back(getUnsignedShort());
  return ret;
}
