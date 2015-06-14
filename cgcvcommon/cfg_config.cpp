


#include "cfg_config.h"


namespace CgcvConfig
{

  Config::Config(TiXmlDocument *doc, TiXmlElement *element) :  Container(element), doc_(doc)
  {
  }
  /**
   \brief dtor
  */
  Config::~Config()
  {
    delete doc_;
  }

  /**
     \brief Search for a Testcase either by id or name
  */
  Testcase* Config::searchForTestcase(size_t id, const std::string* name)
  {
    TiXmlElement* child;
    size_t i = 0;
    
    for(child = myelement_->FirstChildElement("testcase"); child; ++i, child = child->NextSiblingElement("testcase"))
    {
      const std::string *testcase_name = child->Attribute(std::string("name"));
      if ( ( (name && testcase_name && *testcase_name == *name)) ||
           (id == i) )
           
      {
        //we found the right testcase
        //create new container
        Testcase *testace = new Testcase(child,this);
        if(testcase_name)
          testcasesNames_.insert(std::make_pair(*testcase_name,testace));
        testcasesIds_.insert(std::make_pair(i,testace));
        return testace;
      }
    }
    //no success
    std::stringstream err;
    err << "Testcase " << name << " does not exist in Config";
    throw ConfigError(err.str());
  }

  /** 
    \brief Opens the file and creates a Config object using the data of the file
    \param filename file to be opened
    \return created config
  */
  Config* Config::open(const std::string & filename)
  {
    TiXmlDocument *doc = new TiXmlDocument(filename);
    if(!doc->LoadFile())
    {
      std::stringstream errstr;
      errstr << doc->ErrorDesc() << "\n at " << doc->ErrorRow() << ":" << doc->ErrorCol();
      throw BadConfigException(errstr.str());
    }

    TiXmlNode* config = doc->FirstChild("config");
    if(config == NULL || config->ToElement() == NULL)
      throw BadConfigException("No \"config\" node found in document.");

    return new Config(doc,config->ToElement());

  }
 

  /** 
   \brief gets the first Testcase named name
   \param name name of the Testcase to be found
   \return pointer to the found Testcase
  */
  Testcase* Config::getTestcase(const std::string & name)
  {
    //try to get from testcase map
    TestcaseNames::iterator findResult = testcasesNames_.find(name);
    if(findResult != testcasesNames_.end())
      return (findResult->second);


    //else try to find TiXml Element and create a new Testcase
    return searchForTestcase(std::numeric_limits<size_t>::max(), &name);   
  }
  /** 
   \brief gets the id-th Testcase within the configuration
   \param id the id of the Testcase
   \return pointer to the found Testcase
  */
  Testcase* Config::getTestcase(size_t id)
  {
     //try to get from testcase map
    TestcaseIds::iterator findResult = testcasesIds_.find(id);
    if(findResult != testcasesIds_.end())
      return findResult->second;


    //else try to find TiXml Element and create a new Testcase
    return searchForTestcase(id, 0);  
  }
}
