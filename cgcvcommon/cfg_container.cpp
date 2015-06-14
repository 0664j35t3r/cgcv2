


#include "cfg_container.h"


namespace CgcvConfig
{

  /**
   \brief ctor
  */
  Container::Container(TiXmlElement *myelement, Container *parent) : myelement_(myelement), parent_(parent)
  { 
    if(!myelement_)
      throw ConfigError("Internal Error in Config - Container has been built with no content");
  }
  /**
   \brief dtor
  */
  Container::~Container() 
  {
    //free the tree
    ChildrenContainer::iterator iter = children_.begin();
    for(; iter != children_.end(); ++iter)
      delete iter->second;
    children_.clear();
  }

  /**
    \brief get the name of this container
    \return the name of this container
  */ 
  const std::string& Container::getName() const
  {
    return myelement_->ValueStr();
  }
  /**
   \brief  Determines the value string of an attribute
  */
  const std::string& Container::getAttributeStr(TiXmlElement* element) const
  {

    //is the attribute referencing to an option field?
    const std::string * selname;
    if((selname = element->Attribute(std::string("select"))))
    {
        TiXmlElement* result = getOption(element,*selname);
        //if we got a result and it has been not cyclic
        if(result && result != element)
          return getAttributeStr(result);
    }

    //is the attribute coded as value="xxx"
    const std::string* str = element->Attribute(std::string("value"));
    if(str)
      return *str;
    
    //is the attribute coded as <attr>xxx</attr>
    if(element->FirstChild() && element->FirstChild()->ToText())
      return element->FirstChild()->ToText()->ValueStr();

    //can not interpret this statement
    std::stringstream err;
    err << "Attribute " << element->Value() << " does not have any readable values";
    throw ConfigError(err.str());
  }

  /**
    \brief Get Container for a certain name - using this method a tree structure is support
    \param name the name of the Container to find
    \id the id-th Container having name as name is found - default: 0
  */
  Container* Container::getContainer(const std::string& name, size_t id)
  {
    //try to get from children map
    ChildrenContainer::iterator findResult = children_.find(std::make_pair(name,id));
    if(findResult != children_.end())
    {
      return (findResult->second);
    }

    //else try to find TiXml Elment and create a new container

    TiXmlElement* child = getIthChild(myelement_, id, name);
    if(child)
    {
      //create new container
      Container *container = new Container(child,this);
      children_.insert(std::make_pair(std::make_pair(name,id),container));
      return container;
    }
    else
    {
      std::stringstream err;
      err << "Container " << name << "[" << id << "] does not exist in Container " << myelement_->Value();
      throw ConfigError(err.str());
    }
  }

  /**
    \brief counts the number of entries having the same name
    \param name of the entries to be counted
  */
  size_t Container::getNumElements(const std::string& name) const
  {
    TiXmlElement* child;
    size_t i = 0;
    for(child = myelement_->FirstChildElement(name); child; ++i, child = child->NextSiblingElement(name));
    for (TiXmlAttribute* a = myelement_->FirstAttribute(); a; a = a->Next())
      if (name == a->Name())
        ++i;
    return i;
  }

  

  /**
    \brief search for option field "name" in current container. asks parent if no option field "name" found in current container
  */
  TiXmlElement* Container::searchOption(const std::string & name) const
  {
    TiXmlElement* child;
    const char* optionname;
    for(child = myelement_->FirstChildElement(); child ; child = child->NextSiblingElement())
    {
        if((optionname = child->Value()) && optionname == name)
        break;
    }
    //we found an option field
    if(child)
      return child;
    //else
   
    //ask parent
    if(!parent_)
      return 0;
    return parent_->searchOption(name);
  }
  /**
    \brief check if an option field of this name is accessable from this container
  */
  bool Container::hasOption(const std::string& name) const
  {
    return (searchOption(name) != 0);
  }
  /**
    \brief identifies in which way the selection is made in element and finds the corresponding TiXmlElement
  */
  TiXmlElement* Container::getOption(TiXmlElement *element, const std::string& name) const
  {
    const std::string * selection;
    //get selection from field "selected"
    selection = element->Attribute(std::string("selected"));
    
    if(!selection)
      //get it from < >...< >
      if(element->FirstChild() && element->FirstChild()->ToText())
      {
        selection = &(element->FirstChild()->ToText()->ValueStr());
      }
    
    //have we found anything which corresponds to a selection?
    if(selection)
    {
      size_t id;
      std::stringstream data(*selection);
      data >> id;
      if(!data.fail())
      {
        //that has been a number :) -> extract the ith entry
          return getOption(name, id);
      }
      else
        return getOption(name, *selection);
    }
    //get default value
    return getOption(name);
  }

  /**
    \brief tries to find an option field named "name" and gets its default attribute (first one if nothing is specified)
  */
  TiXmlElement* Container::getOption(const std::string & name) const
  {
    TiXmlElement* found = searchOption(name);
    const std::string* defoption;
    if(!found)
      return 0;
    //else
    //are there options?
    if(!found->FirstChildElement("option"))
    {
      std::stringstream err;
      err << "options \"" << name << "\" does not have an option tag!";
      throw ConfigError(err.str()); 
    }
    TiXmlElement* child;
    //is there a standard value?
    if((defoption = found->Attribute(std::string("default"))))
    {
      const std::string * optiontag;
      //maybe we find this tag as name
      for(child = found->FirstChildElement("option"); child ; child = child->NextSiblingElement("option"))
        if((optiontag = child->Attribute(std::string("name"))) && *optiontag == *defoption)
          return child;
      
      //no tag -> try number
      size_t defi;
      std::stringstream data(*defoption);
      data >> defi;
      if(!data.fail())
      {
        //that has been a number :) -> extract the ith entry
        child = getIthChild(found, defi, "option");
        if(child)
          return child;
      }
      //everything failed
      std::stringstream err;
      err << "options \"" << name << "\" does not support the default option \""<< defoption << "\"!";
      throw ConfigError(err.str()); 
    }
    else
    {
      //no default value -> take first
      return found->FirstChildElement("option");
    }
  }

  /**
    \brief tries to find an option field named "name" and gets its id-th value
  */
  TiXmlElement* Container::getOption(const std::string & name, size_t id) const
  {
    TiXmlElement* found = searchOption(name);
    if(!found)
      return 0;
    TiXmlElement* child = getIthChild(found, id, "option");
    if(!child)
    {
      std::stringstream err;
      err << "options \"" << name << "\" does not have an " << id << "th option";
      throw ConfigError(err.str()); 
    }
    return child;
  }
  /**
    \brief tries to find an option field named "name" and gets the value marked as id
  */
  TiXmlElement* Container::getOption(const std::string & name, std::string id) const
  {
    TiXmlElement* found = searchOption(name);
    TiXmlElement* child;
    if(!found)
      return 0;
    const std::string * optiontag;
    //maybe we find this tag as name
    for(child = found->FirstChildElement("option"); child ; child = child->NextSiblingElement("option"))
      if((optiontag = child->Attribute(std::string("name"))) && *optiontag == id)
        return child;
    //no luck
    std::stringstream err;
    err << "options \"" << name << "\" does not have an option \"" << id << "\"!";
    throw ConfigError(err.str()); 
  }

}
