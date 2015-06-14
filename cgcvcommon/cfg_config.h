/* ... */

#ifndef CFG_CONFIG_H
#define CFG_CONFIG_H

#pragma once

#include "cfg_testcase.h"
#include "cfg_container.h"

#include <limits>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class TiXmlDocument;
class TiXmlElement;

namespace CgcvConfig
{
  
  class Testcase;  

  class FileException : public std::exception
  {
    std::string exceptionstr;
  public:
    FileException(const std::string &file) : exceptionstr("The file " + file + " does not exist or is not well formated") {}
    ~FileException() throw() {}
    virtual const char* what() const throw()
    {
      return exceptionstr.c_str();
    }
  };

  

  class Config : public Container
  {
  private:

    typedef std::map<std::string,Testcase*> TestcaseNames;
    typedef std::map<size_t,Testcase*> TestcaseIds;
    

    Config( const Config& /*obj*/);
    void operator = ( const Config& /*obj*/ );

    //vars
    TiXmlDocument *doc_;
    TestcaseNames testcasesNames_;
    TestcaseIds testcasesIds_;
   
    Config(TiXmlDocument *doc, TiXmlElement *element);
    Testcase* searchForTestcase(size_t id, const std::string *name);
  public:
    
    static Config* open(const std::string & filename);

    Testcase* getTestcase(const std::string & name);
    Testcase* getTestcase(size_t id);

    virtual ~Config();
  };

}
#endif //CFG_CONFIG_H
