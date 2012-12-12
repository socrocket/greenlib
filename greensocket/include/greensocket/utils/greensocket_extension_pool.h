// GreenSocket
//
// LICENSETEXT
// 
//   Copyright (C) 2009 : GreenSocs Ltd
//       http://www.greensocs.com/ , email: info@greensocs.com
// 
//   Developed by :
// 
//   Christian Schroeder, Robert Guenzel
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#ifndef __GREENSOCKET_EXTENSION_POOL_H__
#define __GREENSOCKET_EXTENSION_POOL_H__

#include <vector>

namespace gs{
namespace socket{

template <class T>
class extension_pool{
  struct extension_pool_entry{
    public:
    extension_pool_entry(T* content){
      value=content;
      next=NULL;
    }
    T* value;
    extension_pool_entry* next;
  };
  
public:
  extension_pool(int size): out_of_pool(NULL){
    in_pool=new extension_pool_entry(new T());  //create first one
    mine.push_back(in_pool->value);
    for (int i=0; i<size-1; i++){
      extension_pool_entry* e=new extension_pool_entry(new T());
      e->next=in_pool;
      in_pool=e;
      mine.push_back(in_pool->value);
    }
  }
  
  ~extension_pool(){
    for (unsigned int i=0; i<mine.size(); i++){
      delete mine[i];
    }
    
    while (in_pool){
      extension_pool_entry* e=in_pool;
      in_pool=in_pool->next;
      delete e;
    }

    while (out_of_pool){
      extension_pool_entry* e=out_of_pool;
      out_of_pool=out_of_pool->next;
      delete e;
    }
  }
  
  bool is_from(T* cont){
    for (unsigned i=0; i<mine.size(); i++){
      if (mine[i]==cont) return true;
    }
    return false;
  }
  
  T* create(){
    extension_pool_entry* e;
    if (in_pool==NULL){
      e=new extension_pool_entry(new T());
      mine.push_back(e->value);
    }
    else{
      e=in_pool;
      in_pool=in_pool->next;
    }
    e->next=out_of_pool;
    out_of_pool=e;
    return out_of_pool->value; 
  }

  void recycle(T* cont){
    assert(out_of_pool);
    extension_pool_entry* e=out_of_pool;
    out_of_pool=e->next;
    e->value=cont;
    e->next=in_pool;
    in_pool=e;
  }
  
private:
  extension_pool_entry* in_pool;
  extension_pool_entry* out_of_pool;
  std::vector<T*> mine; //just for clean up and is_from
};

}
}

#endif

