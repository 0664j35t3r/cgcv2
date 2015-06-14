/* ... */

#ifndef CFG_CONTAINER_H
#define CFG_CONTAINER_H

#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "tinyxml/tinyxml.h"

namespace CgcvConfig
{
  namespace ColorFormat
  {
    struct RGB8
    {
      template <class ColorType>
      static ColorType fromStream(std::istream& in)
      {
        if (in.peek() == '#')
        {
          in.get();
          unsigned int i;
          in >> std::hex >> i;
          return ColorType(i);
        }

        unsigned char r, g, b;
        in >> r >> g >> b;

        return ColorType(r, g, b);
      }
    };
  }


  class Container;
  class BadConfigException : public std::exception
  {
    std::string exceptionstr;
  public:
    BadConfigException(const std::string &text) : exceptionstr("Bad config: " + text) {}
    ~BadConfigException() throw() {}
    virtual const char* what() const throw()
    {
      return exceptionstr.c_str();
    }
  };
  class ConfigError : public std::exception
  {
    std::string exceptionstr;
  public:
    ConfigError(const std::string &text) : exceptionstr("Config runtime error: "+text) {}
    ~ConfigError() throw() {}
    virtual const char* what() const throw()
    {
      return exceptionstr.c_str();
    }
  };
  template<class O>
  class EmptyObjectBuilder
  {
  protected:
    O obj_;
    Container * container_;
    std::string id_;
  public:
    EmptyObjectBuilder(const std::string & id, Container* cont) : id_(id), container_(cont) {}
    virtual void addAttribute(const std::string& /*name*/ ,const std::string& /*value*/) = 0;
    virtual void addValue(const std::string & /*value*/) = 0;
    virtual void addContainer(const Container* /*cont*/) = 0;
    virtual O getObject() { return obj_; }
  };

  class Container
  {
  private:
    typedef std::map<std::pair<std::string,size_t>, Container*> ChildrenContainer ;

    Container( const Container& /*obj*/);
    void operator = ( const Container& /*obj*/ );

    const std::string& getAttributeStr(TiXmlElement* element) const;
    TiXmlElement* getIthChild(TiXmlElement* element, size_t id, const std::string & name) const
    {
      TiXmlElement* child;
      size_t i = 0;
      for(child = element->FirstChildElement(name); child && i < id; child = child->NextSiblingElement(name), ++i);
      return child;
    }
    TiXmlElement* getOption(TiXmlElement *element, const std::string& name) const;

  protected:
    TiXmlElement *myelement_;
    Container* parent_;
    ChildrenContainer children_;

    Container(TiXmlElement *myelement, Container* parent = 0);

    TiXmlElement* getOption(const std::string & name) const;
    TiXmlElement* getOption(const std::string & name, size_t id) const;
    TiXmlElement* getOption(const std::string & name, std::string id) const;
    TiXmlElement* searchOption(const std::string & name) const;
    
    template<typename Type>
    Type convert(const std::string & str) const;
    
    #ifndef CFG_NO_GEOMETRIC
    template<class vec_t>
    static void vconvert(const std::string & str, vec_t & v);
    template<class matrix_t>
    static void mconvert(const std::string & str, matrix_t & m);
    template<class matrix_t>
    static matrix_t mconvert(const std::string& str);
    #endif


  public:
    //normal getters
    template<typename Type>
    Type getAttribute(const std::string& name, size_t id = 0) const;
    template<typename Type>
    std::vector<Type> getMultiAttribute(const std::string& name) const;
    template<typename Type>
    void getAttribute(Type &attr, const std::string& name, size_t id = 0) const;
    template<typename Type>
    void getMultiAttribute(std::vector<Type>& multiattr, const std::string& name) const;

    #ifndef CFG_NO_GEOMETRIC
    //vector getters
    template<class vectorT>
    void getVector(vectorT& v, const std::string& name, size_t id = 0) const;
    template<class vectorT>
    void getMultiVector(std::vector<vectorT >& v, const std::string& name) const;
    template<class vectorT>
    vectorT getVector(const std::string& name, size_t id = 0) const;
    template<class vectorT>
    std::vector<vectorT> getMultiVector(const std::string& name) const;

    //matrix getters
    template<class matrix_t>
    matrix_t getUnknownMatrix(const std::string& name, size_t id = 0) const;
    template<class matrix_t>
    void getMatrix(matrix_t& m, const std::string& name, size_t id = 0) const;
    template<class matrix_t>
    void getMultiMatrix(std::vector<matrix_t>& mv, const std::string& name) const;
    template<class matrix_t>
    matrix_t getMatrix(const std::string& name, size_t id = 0) const;
    template<class matrix_t>
    std::vector<matrix_t> getMultiMatrix( const std::string& name) const;
    #endif

    // color getters
    template<class ColorType, class ColorFormat>
    ColorType getColor(const std::string& name) const;

    template<class Object, class ObjParser>
    Object getObject(const std::string& name, size_t id = 0);

    Container* getContainer(const std::string& name, size_t id = 0);

    size_t getNumElements(const std::string& name) const;
    const std::string& getName() const;

    bool hasOption(const std::string& name) const;
    template<typename Type>
    Type getSelect(const std::string& name) const;
    template<typename Type>
    Type getSelect(const std::string& name, size_t id) const;
    template<typename Type>
    Type getSelect(const std::string& name, const std::string& selection) const;
   

    virtual ~Container();
  };

  //IMPLEMENTATION

  /**
    \brief Tryes to find all entries having the given name, gets their values, and converts them to Type 
    \return a vector containing all converted values
  */
  template<typename Type>
  inline std::vector<Type> Container::getMultiAttribute(const std::string& name) const
  {
    TiXmlElement* child;
    std::vector<Type> multiAttribute;
    //run through all having this name
    for(child = myelement_->FirstChildElement(name); child; child = child->NextSiblingElement(name))
    {
      multiAttribute.push_back(convert<Type>(getAttributeStr(child)));
    }
    return multiAttribute;
  }

  /**
    \brief Tryes to find the id-th entry with name in the current container, gets its values, converts it to Type
    \return converted value
  */
  template<typename Type>
  inline Type Container::getAttribute(const std::string& name, size_t id) const
  {
    //can be built in attribute
    const std::string *result;
    if(id == 0 && (result = myelement_->Attribute(name)) )
      return convert<Type>(*result);

    //check for normal attribute
    TiXmlElement* child;
    if((child = getIthChild(myelement_,id,name)))
    {
      return convert<Type>(getAttributeStr(child));
    }
    
    {
      std::stringstream err;
      err << "Element " << name << "[" << id << "] does not exist in Container " << myelement_->Value();
      ConfigError error(err.str());
      throw error;
    }
  }

  /**
    \brief Tryes to find the id-th entry with name in the current container, gets its values, converts it to Type 
    \param attr the attribute which will hold the value
  */
  template<typename Type>
  inline void Container::getAttribute(Type &attr, const std::string& name, size_t id) const
  {
    attr = getAttribute<Type>(name,id);
  }
  /**
    \brief Tryes to find all entries having the given name, gets their values, converts them to Type
    \param multiattr vector which will hold the values
  */
  template<typename Type>
  inline void Container::getMultiAttribute(std::vector<Type>& multiattr, const std::string& name) const
  {
    multiattr = getMultiAttribute<Type>(name);
  }

  /**
    \brief get the id-th entry named "name" convert it to an Object using the ObjParser
  */
  template<class Object, class ObjParser>
  inline Object Container::getObject(const std::string& name, size_t id)
  {

    //check for normal attribute
    TiXmlElement* child = getIthChild(myelement_,id,name);
    if(!child)
    {
      std::stringstream err;
      err << "Object " << name << "[" << id << "] does not exist in Container " << myelement_->Value();
      throw ConfigError(err.str());
    }
    //else
    Container* thiscontainer = getContainer(name, id);
    ObjParser parser(name, thiscontainer);
    TiXmlAttribute* attr; 
    for(attr = child->FirstAttribute(); attr; attr = attr->Next())
      parser.addAttribute(attr->NameTStr(),attr->ValueStr());
    TiXmlNode* firstNode;
    if((firstNode = child->FirstChild()) && firstNode->ToText())
      parser.addValue(firstNode->ToText()->ValueTStr());
    TiXmlElement* subChild;
    std::map<std::string, std::size_t> container_occurence;
    std::map<std::string, std::size_t>::iterator currentresult;
    for(subChild = child->FirstChildElement(); subChild; subChild = subChild->NextSiblingElement())
    {
      currentresult = container_occurence.find(subChild->ValueStr());
      if(currentresult != container_occurence.end())
      {
        currentresult->second++;
      }
      else
      {
        std::pair<std::map<std::string, std::size_t>::iterator, bool> insertionresult = container_occurence.insert(std::pair<std::string,std::size_t>(subChild->ValueStr(),0));
        currentresult = insertionresult.first;
      }
      Container* cont =  thiscontainer->getContainer(currentresult->first,currentresult->second);
      parser.addContainer(cont);
    }
    return parser.getObject();
  }
  #ifndef CFG_NO_GEOMETRY
    //vector getters
    template<class vectorT>
    inline void Container::getVector(vectorT& v, const std::string& name, size_t id) const
    {
      std::string data = getAttribute<std::string>(name,id);
      vconvert<vectorT>(data, v);
    }
    template<class vectorT>
    inline void Container::getMultiVector(std::vector<vectorT >& v, const std::string& name) const
    {
      std::vector<std::string> data = getMultiAttribute<std::string>(name);
      v.clear();
      vectorT vec;
      for(size_t i = 0; i < data.size(); ++i)
      {
        vconvert<vectorT>(data[i],vec);
        v.push_back(vec);
      }
    }
    template<class vectorT>
    inline vectorT Container::getVector(const std::string& name, size_t id) const
    {
      vectorT vec;
      getVector(vec,name,id);
      return vec;
    }
    template<class vectorT>
    inline std::vector<vectorT> Container::getMultiVector(const std::string& name) const
    {
       std::vector<vectorT> vecVec;
       getMultiVector(vecVec,name);
       return vecVec;
    }

    // matrix getters
    template<class matrix_t>
    inline matrix_t Container::getUnknownMatrix(const std::string& name, size_t id ) const
    {
      std::string data = getAttribute<std::string>(name,id);
      return mconvert<matrix_t>(data);
    }

    template<class matrix_t>
    inline void Container::getMatrix(matrix_t& m, const std::string& name, size_t id ) const
    {
      std::string data = getAttribute<std::string>(name,id);
      mconvert<matrix_t>(data, m);
    }
    template<class matrix_t>
    inline void Container::getMultiMatrix(std::vector<matrix_t >& mv, const std::string& name) const
    {
      std::vector<std::string> data = getMultiAttribute<std::string>(name);
      mv.clear();
      matrix_t mat;
      for(size_t i = 0; i < data.size(); ++i)
      {
        mconvert<matrix_t>(data[i],mat);
        mv.push_back(mat);
      }
    }
    template<class matrix_t>
    inline matrix_t Container::getMatrix(const std::string& name, size_t id) const
    {
      matrix_t matrix;
      getMatrix(matrix,name,id);
      return matrix;
    }
    template<class matrix_t>
    inline std::vector<matrix_t> Container::getMultiMatrix( const std::string& name) const
    {
      std::vector<matrix_t> matrixVec;
      getMultiMatrix(matrixVec,name);
      return matrixVec;
    }
    #endif

    template<typename Type>
    inline Type Container::getSelect(const std::string& name) const
    {
      TiXmlElement* result = getOption(name);
      return convert<Type>(getAttributeStr(result));
    }
    template<typename Type>
    inline Type Container::getSelect(const std::string& name, size_t id) const
    {
      TiXmlElement* result = getOption(name,id);
      return convert<Type>(getAttributeStr(result));
    }
    template<typename Type>
    inline Type Container::getSelect(const std::string& name, const std::string& selection) const
    {
      TiXmlElement* result = getOption(name,selection);
      if(!result)
      {
        std::stringstream err;
        err << "Select " << name << "does not exist";
        throw ConfigError(err.str());
      }
      return convert<Type>(getAttributeStr(result));
    }

    template<class ColorType, class ColorFormat>
    inline ColorType Container::getColor(const std::string& name) const
    {
      std::string color;
      getAttribute(color, name);
      std::istringstream stream(color);
      ColorType c = ColorFormat::template fromStream<ColorType>(stream);
      if (stream.fail())
        throw ConfigError(std::string("Invalid Color Format: ") + color);
      return c;
    }
}

#include "cfg_conversion.h"

#endif //CFG_CONTAINER_H
