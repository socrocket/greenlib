#ifndef WRAPPER_BASE_H_
#define WRAPPER_BASE_H_

using namespace std;

namespace gs_generic_signal
{
/*!
* @brief: The wrapper base class basically contains a global map of all
* the initiator-socket names and their corresponding IDs
*/

class wrapper_base{
	
protected:
/*!
*@brief: This function will return the map.
*
*/
static std::map<std::string, unsigned int>* get_decode_map()
{
  static std::map<std::string, unsigned int>* socketmap= new std::map<std::string, unsigned int>();
  return socketmap;
}

/*!
*@brief: This function will return the ID of the name passed as
* an argument
*
*/
public:
 static unsigned int decode_name(const std::string& name){
  if (get_decode_map()->find(name)==get_decode_map()->end()) 
   {
   	std::cout<<"Error: The name does not exists in the Map." << endl;
        exit(1);
       return 0;
   }
   else
	  return (*get_decode_map())[name];
 }

};

}

#endif /*WRAPPER_BASE_H_*/
