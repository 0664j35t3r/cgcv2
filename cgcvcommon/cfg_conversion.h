/* ... */

#ifndef CFG_CONTAINER_H
//include container and so ourselves if someone directly included conversion...
#include "cfg_container.h"
#else

#include <vector>

namespace CgcvConfig
{

  /**
    \brief default type conversions for the Container class
  */
  template <typename Type>
  inline Type Container::convert(const std::string& str) const
  {
    Type val;
    std::stringstream sstr(str);
    sstr >> val;
    if (sstr.fail())
    {
      std::stringstream err;
      err << "could not convert \"" << str << "\" as wished";
      throw ConfigError(err.str());
    }

    return val;
  }
  /**
    \brief special conversion for strings
  */
  template <>
  inline std::string Container::convert<std::string>(const std::string& str) const
  {
    return str;
  }

#ifndef CFG_NO_GEOMETRIC
  /**
    \brief special conversion for vector types
  */
  template <class vec_t>
  inline void Container::vconvert(const std::string& str, vec_t& vec)
  {

    typedef typename std::size_t size_type;
    std::stringstream is(str);
    char ch;
    vec_t newvec;
    std::stringstream err;
    err << "could not convert \"" << str << "\" to a vector";

    is >> ch;
    bool open = false;
    if (ch == '(')
    {
      //ignore '('
      open = true;
    }
    else
      is.putback(ch);
    for (size_type i = 0; i < vec.size(); ++i)
    {
      is.setf(std::ios::skipws);
      is >> newvec[i];
      if (is.fail())
        throw ConfigError(err.str());
      is.unsetf(std::ios::skipws);
      //get all seperation stuff
      if (i != vec.size() - 1 && is >> ch && ch != ',' && ch != ';' && ch != ' ')
        throw ConfigError(err.str());
    }
    if (open && is >> ch && ch != ')')
      throw ConfigError(err.str());
    vec = newvec;
  }

  /**
    \brief special conversion for matrix types
  */
  template <class matrix_t>
  inline matrix_t Container::mconvert(const std::string& str)
  {
    typedef typename std::size_t size_type;
    std::stringstream is(str);
    char ch;

    std::stringstream err;
    err << "could not convert \"" << str << "\" to a matrix";

    typedef std::vector<std::vector<typename matrix_t::element_type>> newmat_t;
    newmat_t newmat;

    int i = 0;
    while (ch = is.get(), is)
    {
      switch (ch)
      {
        case '\n':
        case ';':
          ++i;
        case ',':
        case ' ':
        case '\t':
          break;
        default:
        {
          is.putback(ch);
          typename matrix_t::element_type e;
          is >> std::noskipws >> e;

          if (is.fail())
            throw ConfigError(err.str() + ": invalid character");

          if (newmat.size() <= i)
            newmat.push_back(typename newmat_t::value_type());
          newmat.back().push_back(e);
          break;
        }
      }
    }

    size_t rows = newmat.size();
    size_t cols = newmat.back().size();
    matrix_t mat(rows, cols);

    for (size_t i = 0; i < rows; ++i)
    {
      if (newmat[i].size() != cols)
        throw ConfigError(err.str() + ": invalid row dimension");

      for (size_t j = 0; j < cols; ++j)
        mat(i, j) = newmat[i][j];
    }

    return mat;
  }

  template <class matrix_t>
  inline void Container::mconvert(const std::string& str, matrix_t& mat)
  {
    typedef typename std::size_t size_type;
    std::stringstream is(str);
    char ch;

    std::stringstream err;
    err << "could not convert \"" << str << "\" to a matrix";

    matrix_t newmat;

    is >> ch;
    bool open1 = false;
    if (ch == '(')
    {
      //ignore '('
      open1 = true;
    }
    else
      is.putback(ch);
    for (size_type i = 0; i < mat.size(); ++i)
    {
      is >> ch;
      bool open2 = false;
      if (ch == '(')
      {
        //ignore '('
        open2 = true;
      }
      else
        is.putback(ch);
      for (size_type j = 0; j < mat[i].size(); ++j)
      {
        is.setf(std::ios::skipws);
        is >> newmat[i][j];
        if (is.fail())
          throw ConfigError(err.str());
        is.unsetf(std::ios::skipws);
        //get all seperation stuff
        if (j != mat[i].size() - 1 && is >> ch && ch != ',' && ch != ' ')
          throw ConfigError(err.str());
      }

      is.setf(std::ios::skipws);
      is >> ch;
      if (open2 == true)
      {
        if (ch != ')')
          throw ConfigError(err.str());
        else
          is >> ch;
      }
      while (ch == '\n')
        is >> ch;
      if (ch == ';' || ch == ',')
        is >> ch;
      while (ch == '\n')
        is >> ch;
      is.putback(ch);
      is.unsetf(std::ios::skipws);
    }

    if (open1 && is >> ch && ch != ')')
      throw ConfigError(err.str());
    mat = newmat;
  }
#endif
}

#endif //CFG_CONTAINER_H
