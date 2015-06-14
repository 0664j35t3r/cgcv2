#include "Image.h"
#include <lodepng.h>
#include <sstream>

Image::Image()
    : width_(0),
      height_(0),
      format_(Image::Format::RGB),
      component_count_(3),
      data_()
{
}

Image::Image(int width, int height, Image::Format format)
    : width_(width),
      height_(height),
      format_(format),
      component_count_(0),
      data_(),
      error_("")
{
  switch (format_)
  {
    case Image::Format::RGB:
      component_count_ = 3;
      break;
    case Image::Format::RGBA:
      component_count_ = 4;
      break;
  }
  data_.resize(width_ * height_ * component_count_);
}

Image::Image(const Image& instance)
    : width_(instance.width_),
      height_(instance.height_),
      format_(instance.format_),
      data_(instance.data_),
      error_("")
{
}

int Image::getWidth()
{
  return width_;
}

int Image::getHeight()
{
  return height_;
}

Image::Format Image::getFormat()
{
  return format_;
}

std::string Image::getError()
{
  return error_;
}

unsigned char* Image::getDataPointer()
{
  return &data_[0];
}

bool Image::save(std::string file_name)
{
  LodePNGColorType ct = LCT_RGB;
  if (format_ == Image::Format::RGBA)
    ct = LCT_RGBA;

  unsigned error = lodepng::encode(file_name, data_, width_, height_, ct);

  if (error)
  {
    std::stringstream ss;
    ss << "Encoder error (" << error << "): " << lodepng_error_text(error);
    error_ = ss.str();
    return false;
  }

  return true;
}

void Image::flip(bool x, bool y)
{
  std::vector<unsigned char> data_copy(data_);

  for (int row = 0; row < height_; row++)
  {
    for (int col = 0; col < width_; col++)
    {
      unsigned flipped_row = x ? height_ - row - 1 : row;
      unsigned flipped_col = y ? width_ - col - 1 : col;
      unsigned first_index_new = row * width_ * component_count_ +
                                 col * component_count_;
      unsigned first_index_old = flipped_row * width_ * component_count_ +
                                 flipped_col * component_count_;
      for (int component_index = 0; component_index < component_count_;
           component_index++)
      {
        data_[first_index_new + component_index] =
            data_copy[first_index_old + component_index];
      }
    }
  }
}
