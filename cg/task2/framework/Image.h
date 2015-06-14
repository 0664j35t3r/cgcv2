#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>

/**
* Class that's used for the storing, loading and saving of images.
*/
class Image
{
public:
  /**
   * The byte format that images can have.
   */
  enum class Format
  {
    /**
     * Bytes are stored in the RGB format.
     */
    RGB,

    /**
     * Bytes are stored in the RGBA format.
     */
    RGBA
  };

  /**
   * Default constructor of the Image class. 
   */
  Image();

  /**
   * Constructor that takes and sets the width, height and the format.
   * @param width The images width.
   * @param height The images height. 
   * @param format The images byte format. 
   */
  Image(int width, int height, Format format);

  /**
   * Copy constructor. 
   * @param instance The instance that we want to copy.
   */
  Image(const Image& instance);

  /**
   * Get the width of the image.
   * @return int The width of the image. 
   */
  int getWidth();

  /**
   * Get the height of the image.
   * @return int The height of the image. 
   */
  int getHeight();

  /**
   * Get the format of the image.
   * @return Format The format of the image. 
   */
  Format getFormat();

  /**
   * Get the error that may have happened in one of the methods.
   * @return std::string The error that happened.
   */
  std::string getError();

  /**
   * Get the pointer that points to the beginning of the data to read/write 
   * from/to there.
   * @return unsigned char* The pointer to the beginning of the data. 
   */
  unsigned char* getDataPointer();

  /**
   * Saves the image to disk.
   * @param file_name The file where the image should be saved.
   * @return bool Whether the save was successful or not. 
   */
  bool save(std::string file_name);

  /**
   * Flips the image data.
   * @param x Flip the image along the x-axis.
   * @param y Flip the image along the y-axis.
   * @return void  
   */
  void flip(bool x, bool y);

private:
  /**
   * Width of the image.
   */
  int width_;

  /**
   * Height of the image.
   */
  int height_;

  /**
   * Byte format of the image.
   */
  Format format_;

  /**
   * The number of components that are needed for the used 
   * format e.g. RGB -> 3.
   */
  int component_count_;

  /**
   * The data of the image. Contains the actual bytes.
   */
  std::vector<unsigned char> data_;

  /**
   * Error that may have happened within a method.
   */
  std::string error_;
};

#endif
