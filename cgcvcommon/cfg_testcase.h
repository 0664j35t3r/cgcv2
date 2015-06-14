/* ... */

#ifndef CFG_TESTCASE_H
#define CFG_TESTCASE_H

#pragma once

#include "cfg_container.h"

class TiXmlElement ;


namespace CgcvConfig
{
 
  class Config;

  class Testcase : public Container
  {
    friend class Config;
  private:
    Testcase( const Testcase& /*obj*/);
    void operator = ( const Testcase& /*obj*/ );
    Testcase(TiXmlElement *myelement, Container* parent);

  protected:
    Config* config; 
    

  public:

    template<typename Type>
    Type getAttribute(const std::string& name, size_t id = 0) const;
    template<typename Type>
    const std::vector<Type> getMultiAttribute(const std::string& name) const;

    template<typename Type>
    void getAttribute(Type &attr, const std::string& name, size_t id = 0) const;
    template<typename Type>
    void getMultiAttribute(std::vector<Type>& multiattr, const std::string& name) const;

    template<class Object, class ObjParser>
    Object getObject(const std::string& name, size_t id = 0);

    #ifndef CFG_NO_GEOMETRY
    //vector getters
    template<class vectorT>
    void getVector(vectorT& v, const std::string& name, size_t id = 0) const;
    template<class vectorT>
    void getMultiVector(std::vector< vectorT >& v, const std::string& name) const;
    template<class vectorT>
    vectorT getVector(const std::string& name, size_t id = 0) const;
    template<class vectorT>
    std::vector<vectorT> getMultiVector(const std::string& name) const;

    //matrix getters
    template<class matrix_t>
    void getMatrix(matrix_t& m, const std::string& name, size_t id = 0) const;
    template<class matrix_t>
    void getMultiMatrix(std::vector<matrix_t >& mv, const std::string& name) const;
    template<class matrix_t>
    matrix_t getMatrix(const std::string& name, size_t id = 0) const;
    template<class matrix_t>
    std::vector<matrix_t> getMultiMatrix( const std::string& name) const;
    #endif

    virtual ~Testcase() {}
  };

  //IMPLEMENTATION
  /**
     \brief ctor
  */
  inline Testcase::Testcase(TiXmlElement *myelement, Container* parent) : Container(myelement,parent) {}

  /**
     \brief mirror for Container method which also looks in parent
  */
  template<typename Type>
  inline Type Testcase::getAttribute(const std::string& name, size_t id) const
  {
    //if the attribute is not in the testcase, try to find it in parent
    try
    {
      return Container::getAttribute<Type>(name,id);
    }
    catch(ConfigError &ex)
    {
      //not in container
      try
      {
        return parent_->getAttribute<Type>(name,id);
      }
      catch(std::exception)
      {
        //not in parent -> throw original exception
        throw ex;
      }
    }
  }

  /**
     \brief mirror for Container method which also looks in parent
  */
  template<typename Type>
  inline const std::vector<Type> Testcase::getMultiAttribute(const std::string& name) const
  {
    //if the attribute is not in the testcase, try to find it in parent
    try
    {
      return Container::getMultiAttribute<Type>(name);
    }
    catch(ConfigError &ex)
    {
      //not in container
      try
      {
        return parent_->getMultiAttribute<Type>(name);
      }
      catch(std::exception)
      {
        //not in parent -> throw original exception
        throw ex;
      }
    }
  }
  
  /**
     \brief mirror for Container method which also looks in parent
  */
  template<typename Type>
  inline void Testcase::getAttribute(Type &attr, const std::string& name, size_t id) const
  {
    attr = Testcase::getAttribute<Type>(name,id);
  }
  /**
     \brief mirror for Container method which also looks in parent
  */
  template<typename Type>
  inline void Testcase::getMultiAttribute(std::vector<Type>& multiattr, const std::string& name) const
  {
    multiattr = Testcase::getMultiAttribute<Type>(name);
  }
  /**
     \brief mirror for Container method which also looks in parent
  */
  template<class Object, class ObjParser>
  inline Object Testcase::getObject(const std::string& name, size_t id)
  {
    //if the object is not in the testcase, try to find it in parent
    try
    {
      return Container::getObject<Object,ObjParser>(name,id);
    }
    catch(ConfigError &ex)
    {
      //not in container
      try
      {
        return parent_->getObject<Object,ObjParser>(name,id);
      }
      catch(std::exception)
      {
        //not in parent -> throw original exception
        throw ex;
      }
    }
  }

  #ifndef CFG_NO_GEOMETRY
    /**
     \brief mirror for Container method which also looks in parent
    */
    template<class vectorT>
    inline void Testcase::getVector(vectorT& v, const std::string& name, size_t id) const
    {
      //if the attribute is not in the testcase, try to find it in parent
      try
      {
        return Container::getVector(v,name,id);
      }
      catch(ConfigError &ex)
      {
        //not in container
        try
        {
          return parent_->getVector(v,name,id);
        }
        catch(std::exception)
        {
          //not in parent -> throw original exception
          throw ex;
        }
      }
    }
    /**
     \brief mirror for Container method which also looks in parent
    */
    template<class vectorT>
    inline void Testcase::getMultiVector(std::vector< vectorT >& v, const std::string& name) const
    {
      //if the attribute is not in the testcase, try to find it in parent
      try
      {
        return Container::getMultiVector(v,name);
      }
      catch(ConfigError &ex)
      {
        //not in container
        try
        {
          return parent_->getMultiVector(v,name);
        }
        catch(std::exception)
        {
          //not in parent -> throw original exception
          throw ex;
        }
      }
    }
    template<class vectorT>
    inline vectorT Testcase::getVector(const std::string& name, size_t id) const
    {
      vectorT vec;
      getVector(vec,name,id);
      return vec;
    }
    template<class vectorT>
    inline std::vector<vectorT> Testcase::getMultiVector(const std::string& name) const
    {
       std::vector<vectorT> vecVec;
       getMultiVector(vecVec,name);
       return vecVec;
    }

    /**
      \brief mirror for Container method which also looks in parent
    */
    template<class matrix_t>
    inline void Testcase::getMatrix(matrix_t& m, const std::string& name, size_t id) const
    {
      //if the attribute is not in the testcase, try to find it in parent
      try
      {
        return Container::getMatrix(m,name,id);
      }
      catch(ConfigError &ex)
      {
        //not in container
        try
        {
          return parent_->getMatrix(m,name,id);
        }
        catch(std::exception)
        {
          //not in parent -> throw original exception
          throw ex;
        }
      }
    }
    /**
      \brief mirror for Container method which also looks in parent
    */
    template<class matrix_t>
    inline void Testcase::getMultiMatrix(std::vector<matrix_t >& mv, const std::string& name) const
    {
      //if the attribute is not in the testcase, try to find it in parent
      try
      {
        return Container::getMultiMatrix(mv,name);
      }
      catch(ConfigError &ex)
      {
        //not in container
        try
        {
          return parent_->getMultiVector(mv,name);
        }
        catch(std::exception)
        {
          //not in parent -> throw original exception
          throw ex;
        }
      }
    }
    template<class matrix_t>
    inline matrix_t Testcase::getMatrix(const std::string& name, size_t id) const
    {
      matrix_t matrix;
      getMatrix(matrix,name,id);
      return matrix;
    }
    template<class matrix_t>
    inline std::vector<matrix_t> Testcase::getMultiMatrix( const std::string& name) const
    {
      std::vector<matrix_t> matrixVec;
      getMultiMatrix(matrixVec,name);
      return matrixVec;
    }

    #endif

}

#endif //CFG_TESTCASE_H
