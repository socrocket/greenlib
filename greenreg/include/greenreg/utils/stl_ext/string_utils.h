/*
Copyright (c) 2008, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// ChangeLog GreenSocs
// 2009-01-21 Christian Schroeder: changed namespace
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!


#include <sstream>

#ifndef _STRINGGREENREG_UTILS_H_
#define _STRINGGREENREG_UTILS_H_

namespace gs {
namespace reg_utils {

class string {
	
public:

template< typename T>
static std::string to_string( const T _value)
{
	std::ostringstream oss;
	if( oss << _value ) // output integer to string stream
		return oss.str(); // retrieve it as a string
	else
		throw("Could not convert to string");
}

static std::string unsigned_long_to_hex_string( unsigned long _value, unsigned int _width = 8)
{
	std::stringstream ss;
	ss << "0x";
	ss.width( _width);
	ss.fill('0');
	ss << std::hex << _value;
	return( ss.str());
}

static unsigned long hex_string_to_unsigned_long( std::string _str)
{
	std::stringstream ss;
	unsigned long val = 0;
	
	// first remove the hex reference
	std::string::size_type index = _str.find( "0x", 0);
	if( index == std::string::npos)
	{
		index = _str.find( "0X", 0);
	}
	
	if( index != std::string::npos)
	{
		_str.erase( 0, 2);
	}
	
	// now initialize the stream for conversion of the value from hex to int...
	ss << std::hex << _str;
	ss >> val;
	
	// all done
	return( val);
}

static std::string tabs( unsigned int _num_tabs)
{
	std::stringstream ss;
	for( unsigned int i = 0; i < _num_tabs; i++)
	{
		ss << "\t";
	}
	return( ss.str());
}

static bool string_match( std::string _char_pattern, std::string _str, const char _ignore_char = '-')
{
	bool match = true;
	char pattern_char;
	char str_char;
	::size_t counter = 0;

	// strings of unmatched size cannot match
	if( _char_pattern.size() != _str.size())
	{
		// we have already mis-matched
		match = false;
	} else {

		// while we have charcters to process
		while( counter < _str.size())
		{
			// get pattern character
			pattern_char = _char_pattern.at( counter);

			// if it is an ignore char, skip
			if( pattern_char != _ignore_char)
			{
				// get string character
				str_char = _str.at( counter);

				// if they are not equal then match = false, counter = string size
				if( pattern_char != str_char)
				{
					match = false;
					counter = _str.size();

				// else increment counter
				} else {
					counter++;
				}

			// else increment counter
			} else {
				counter++;
			}
		}

	}

	// done
	return( match);
}

}; // namespace string

} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#endif // _STRINGGREENREG_UTILS_H_


#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#ifndef _STRINGGREENREG_UTILS_H_
#define _STRINGGREENREG_UTILS_H_

#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>

namespace gs {
namespace reg_utils {

class string {

public:
	typedef std::list<std::string> STRLIST;		// A list of strings.
	typedef std::set<std::string> STRSET;		// A set of strings.
	typedef std::vector<std::string> STRVEC;	// A vector of strings.
	typedef std::pair<std::string, std::string> STRPAIR;		// A pair of strings.

	// Case Insensitive string compare
	struct CaseInsensitiveStrComp : 
		public std::binary_function<const std::string&, const std::string&, bool>
	{
		bool operator() (const std::string& _lhs, const std::string& _rhs) const
		{
			#ifdef _WINDOWS
			return (stricmp( _lhs.c_str(), _rhs.c_str() ) == -1 );
			#else
			return (strcasecmp( _lhs.c_str(), _rhs.c_str() ) == -1 );
			#endif

		}
	};

	typedef std::set< std::string, CaseInsensitiveStrComp > CISTRSET;
	typedef CISTRSET::iterator CISTRSETIT;
	typedef CISTRSET::const_iterator CISTRSETCIT;

	// Standard whitespace characters
	// \t = tab character
	// \r = carriable return
	// \f = formfeed
	// \v = vertical tab
	// \n = newline
	static const std::string WHITESPACES;

public:

/*--------------------------------------------------------------------

 CREATION DATE	: 
 APPROVAL		: APPROVED   
 NAME			: String
 DESCRIPTION    : Constructor
 ASSUMPTIONS	: 
 PARAMETERS     : 
 RETURNS        : 
 EXCEPTIONS		:

---------------------------------------------------------------------*/
	string(){}

/*--------------------------------------------------------------------

 CREATION DATE	: 
 APPROVAL		: APPROVED   
 NAME			: ~String
 DESCRIPTION    : Destructor
 ASSUMPTIONS	: 
 PARAMETERS     : 
 RETURNS        : 
 EXCEPTIONS		:

---------------------------------------------------------------------*/
	~string(){}

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: does_pattern_match
 DESCRIPTION    : Tries to match a pattern with wildcards against
					a string.
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to be matched against
				  _pattern - the pattern to match with
				  _wildcards - string of wildcard characters; wilcards
				    must be single characters
 RETURNS        : bool - true: if the pattern matches the string
					   - false: the pattern does not match the string
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static bool does_pattern_match(const std::string& _str,
							 const std::string& _pattern,
							 const std::string& _wildcards,
							 bool _bCaseSensitive = true);

static bool does_filter_expression_match ( const std::string& _str,
									    const std::string& _filter );

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: to_upper
 DESCRIPTION    : returns a new string with all alpha characters converted to upper case.
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to be converted
 RETURNS        : std::string - converted string
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string to_upper(const std::string& _str);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: to_lower
 DESCRIPTION    : returns a new string with all alpha characters converted to lower case.
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to be converted
 RETURNS        : std::string - converted string
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string to_lower(const std::string& _str);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: parse
 DESCRIPTION    : parses a string based on input delimiters
 ASSUMPTIONS	: 
 PARAMETERS     : _str: string to parse, 
				  _ovector: STL list of strings. Out parameter
				  _delims: delimiters, defaulted to whitespace characters
 RETURNS        : void. Note: _strList acts as an Out parameter
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static void parse(const std::string& _str, 
								 std::list<std::string>& _ovector, 
								 const std::string& _delims = gs::reg_utils::string::WHITESPACES,
								 bool _returnEmptyTokens = false);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: parse
 DESCRIPTION    : parses a string based on input delimiters
 ASSUMPTIONS	: 
 PARAMETERS     : _str: string to parse, 
				  _olist: STL vector of strings. Out parameter
				  _delims: delimiters, defaulted to whitespace characters
 RETURNS        : void. Note: _strVector acts as an Out parameter
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static void parse(const std::string& _str, 
								 std::vector<std::string>& _olist, 
								 const std::string& _delims = gs::reg_utils::string::WHITESPACES,
								 bool _returnEmptyTokens = false);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: parse
 DESCRIPTION    : parses a string based on input delimiters
 ASSUMPTIONS	: 
 PARAMETERS     : _str: string to parse, 
				  _oset: STL set of strings. Out parameter
				  _delims: delimiters, defaulted to whitespace characters
 RETURNS        : void. Note: _strVector acts as an Out parameter
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static void parse(const std::string& _str, 
								 std::set<std::string>& _oset, 
								 const std::string& _delims = gs::reg_utils::string::WHITESPACES,
								 bool _returnEmptyTokens = false);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: trim_leading
 DESCRIPTION    : Trims leading characters that are part of _chars
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to trim removable characters from
				  _chars - removable characters, defaulted to white 
				  space characters
 RETURNS        : string - string with leading removable characters 
				  trimmed
 EXCEPTIONS		: 

---------------------------------------------------------------------*/
static std::string trim_leading(const std::string& _str, const std::string& _chars = gs::reg_utils::string::WHITESPACES);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: trim_trailing
 DESCRIPTION    : trims trailing characters that are part of _chars
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to trim removable characters from
				  _chars - removable characters, defaulted to white space characters
 RETURNS        : string - string with trailing removable characters 
				  trimmed
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string trim_trailing(const std::string& _str, const std::string& _chars = gs::reg_utils::string::WHITESPACES);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: trim_leading_and_trailing
 DESCRIPTION    : Same as trim_leading and trim_trailing,
				  except that it is slightly more efficient than
				  calling the two methods separately.
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to trim removable characters from
				  _chars - removable characters
 RETURNS        : string - string with leading and trailing removable
				  characters trimmed
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string trim_leading_and_trailing(const std::string& _str, const std::string& _chars = gs::reg_utils::string::WHITESPACES);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: trim_all
 DESCRIPTION    : trims all removable characters from string
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to trim removable characters from
				  _chars - removable characters
 RETURNS        : string - string with all removable characters 
				  trimmed
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string trim_all(const std::string& _str, const std::string& _chars = gs::reg_utils::string::WHITESPACES);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: reverse
 DESCRIPTION    : reverses the order of the input string
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to be reversed
 RETURNS        : string - reversed string
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string reverse(const std::string& _str);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: build_string
 DESCRIPTION    : Builds a single string from a vector of strings
 ASSUMPTIONS	: 
 PARAMETERS     : _strList - vector of strings to concatenate
				  _delim - delimiter string that is inserted
				           between strings from the vector
 RETURNS        : string - concatenated string
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string build_string(const std::vector<std::string>& _vstr, const std::string& _delim = "");


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: build_string
 DESCRIPTION    : Builds a single string from a list of strings
 ASSUMPTIONS	: 
 PARAMETERS     : _strList - list of strings to concatenate
				  _delim - delimiter string that is inserted
						   between strings from the list
 RETURNS        : string - concatenated string
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string build_string(const std::list<std::string>& _lstr, const std::string& _delim = "");

template< class BIT >
inline static std::string build_string(BIT _first, BIT _last, const std::string& _delim = "" )
{
	std::string strReturn ( *(_first++) );

	for(; _first != _last; ++_first)
	{
		strReturn += _delim + *_first;
	}

	return strReturn;
}

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: get_longest_string_index
 DESCRIPTION    : finds and returns the index of the longest string
 ASSUMPTIONS	: 
 PARAMETERS     : _vstr - vector of strings
 RETURNS        : int - index in vector of longest string
				  in case of error, it returns -1
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int get_longest_string_index(const std::vector<std::string>& _vstr);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: get_longest_string_index
 DESCRIPTION    : finds and returns an iterator to the longest string
 ASSUMPTIONS	: 
 PARAMETERS     : _lstr - list of strings
 RETURNS        : const iterator to std::list<std::string>
				  in case of error, returns const iterator to end of list
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::list<std::string>::const_iterator get_longest_string_index(const std::list<std::string>& _lstr);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: is_int
 DESCRIPTION    : Determines if a string is all integers
 ASSUMPTIONS	: Does not check for overflow
 PARAMETERS     : _str - string to check
 RETURNS        : bool - true if all integers, false if not
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static bool is_int(const std::string& _str);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED
 NAME			: to_int
 DESCRIPTION    : converts a std::string to an integer
 ASSUMPTIONS	: If the string represents an integer greater than the
                  maximum value supported for an integer, then the 
				  result will mostly be incorrect but an exception will 
				  be thrown
 PARAMETERS     : _str - string to convert
 RETURNS        : int - the int form of the string
 EXCEPTIONS		: std::exception: an exception is thrown if the string cannot be
				: converted to a double

---------------------------------------------------------------------*/
static int to_int(const std::string& _str);
static unsigned int to_uint(const std::string& _str);
static long to_long(const std::string& _str);
static unsigned long to_ulong(const std::string& _str);

/*--------------------------------------------------------------------

 APPROVAL		: PENDING
 NAME			: to_int
 DESCRIPTION    : converts a std::string to int
 ASSUMPTIONS	: The caller will be responsible for checking the return value
				  before accessing the int value.
 PARAMETERS     : _str - string to convert
				  _value - the result of the conversion
 RETURNS        : true if the conversion was successful and _value will contain
				  the integer value. false if the conversion was not successful
				  and _value will be undefined
 EXCEPTIONS		: NONE

---------------------------------------------------------------------*/
static bool to_int(const std::string& _str, int& _value);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED
 NAME			: to_string
 DESCRIPTION    : converts a int to a std::string
 ASSUMPTIONS	: 
 PARAMETERS     : _value - int to convert
 RETURNS        : std::string - the string form of the int
 EXCEPTIONS		: std::exception if not successful

---------------------------------------------------------------------*/
template< typename T>
static std::string _to_string( const T _value)
{
	std::ostringstream oss;
	if( oss << _value ) // output integer to string stream
		return oss.str(); // retrieve it as a string
	else
		throw("Could not convert to string");
}

static std::string to_string( const bool _value)
{ return( std::string( _value ? "true" : "false")); }

static std::string to_string( const int _value) 
{ return( _to_string< int>( _value)); }

static std::string to_string( const long _value)
{ return( _to_string< long>( _value)); }

static std::string to_string( const float _value)
{ return( _to_string< float>( _value)); }

static std::string to_string( const double _value)
{ return( _to_string< double>( _value)); }

static std::string to_string( const unsigned int _value)
{ return( _to_string< unsigned int>( _value)); }

static std::string to_string( const unsigned long _value)
{ return( _to_string< unsigned long>( _value)); }

// special case for reusability in data structures with unknown types
static std::string to_string(const std::string _value) 
{ return _value; }

template< typename T>
static void _from_string( const std::string & _value, T & _new_value)
{
	try {
		std::istringstream( _value) >> _new_value;
	} catch( ...) {
		throw( "could not convert from string");
	}
}

static void from_string( const std::string _value, bool & _new_value)
{ _new_value = ( compare_no_case("true", _value) == 0 ? true : false); }

static void from_string( const std::string _value, int & _new_value)
{ _from_string< int>( _value, _new_value); }

static void from_string( const std::string _value, long & _new_value)
{ _from_string< long>( _value, _new_value); }

static void from_string( const std::string _value, float & _new_value)
{ _from_string< float>( _value, _new_value); }

static void from_string( const std::string _value, double & _new_value)
{ _from_string< double>( _value, _new_value); }

static void from_string( const std::string _value, unsigned int & _new_value)
{ _from_string< unsigned int>( _value, _new_value); }

static void from_string( const std::string _value, unsigned long & _new_value)
{ _from_string< unsigned long>( _value, _new_value); }

static void from_string( const std::string _value, std::string & _new_value)
{ _new_value = _value; }

static std::string to_hex_string( unsigned long _value, unsigned int _width = 8)
{
	std::stringstream ss;
	ss << "0x";
	ss.width( _width);
	ss.fill('0');
	ss << std::hex << _value;
	return( ss.str());
}

static unsigned long to_hex_value( std::string _str);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: is_double
 DESCRIPTION    : determines if a string can be converted to a double
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to check
 RETURNS        : bool - true if can be converted, false if not
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static bool is_double(const std::string& _str);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED
 NAME			: to_double
 DESCRIPTION    : converts a std::string to double
 ASSUMPTIONS	: If the string represents a number greater than the
                  maximum value supported for a double, then the 
				  result will mostly be incorrect but an exception will 
				  be thrown
 PARAMETERS     : _str - string to convert
 RETURNS        : double - the double form of the string
 EXCEPTIONS		: std::exception: an exception is thrown if the string cannot be
				: converted to a double

---------------------------------------------------------------------*/
static double to_double(const std::string& _str);

/*--------------------------------------------------------------------

 APPROVAL		: PENDING
 NAME			: force_to_double
 DESCRIPTION    : converts a std::string to double.  This method removes
				: all ending alpha characters to do the conversion.  Handles
				: cases such as 132.47pf, 10K, etc... that the to_double
				: implementation will throw an exception on.
 ASSUMPTIONS	: If the string represents a number greater than the
                  maximum value supported for a double, then the 
				  result will mostly be incorrect but an exception will 
				  be thrown
 PARAMETERS     : _str - string to convert
 RETURNS        : double - the double form of the string
 EXCEPTIONS		: std::exception: an exception is thrown if the string cannot be
				: converted to a double

---------------------------------------------------------------------*/
static double force_to_double( const std::string & _str);

/*--------------------------------------------------------------------

 APPROVAL		: PENDING
 NAME			: to_double
 DESCRIPTION    : converts a std::string to double
 ASSUMPTIONS	: The caller will be responsible for checking the return value
				  before accessing the double value.
 PARAMETERS     : _str - string to convert
				  _value - the result of the conversion
 RETURNS        : true if the conversion was successful and _value will contain
				  the double value. false if the conversion was not successful
				  and _value will be undefined
 EXCEPTIONS		: NONE

---------------------------------------------------------------------*/
static bool to_double(const std::string& _str, double& _value);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED
 NAME			: to_string
 DESCRIPTION    : converts a double to a std::string
 ASSUMPTIONS	: 
 PARAMETERS     : _value - double to convert
				: _fixedDecimalPlaces - decimalPlaces to round off at,
				:	-1 means "Do not apply" and is the default valud;
 RETURNS        : std::string - the string form of the double
 EXCEPTIONS		: std::exception if not successful
 

---------------------------------------------------------------------*/
static std::string to_string(const double _value, const int _fixedDecimalPlaces);

 
/*--------------------------------------------------------------------

 APPROVAL		: PENDING
 NAME			: to_string
 DESCRIPTION    : Finds if _substr is a word sub string of _str. 
				  For example, "at" is a word sub string of 
				  "at least" but not of "atleast"
 ASSUMPTIONS	: A word is assumed to be delimited by punctuations 
				  or white spaces
 PARAMETERS     : _str - the string to be searched in
				  _substr - the string to be searched
 RETURNS        : bool. True if _substr is a word sub string, false otherwise
 EXCEPTIONS		: None
 

---------------------------------------------------------------------*/
static bool is_word_substr(const std::string& _str, const std::string& _substr);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: alpha_numeric_comp
 DESCRIPTION    : determines if _str1 comes before _str2 alphaNumerically
				  For example "B10" would be greater than "B4".
 ASSUMPTIONS	: Uses absolute value implicitly when comparing numbers in a string
 PARAMETERS     : _str1, _str2 - strings to check
 RETURNS        : int:  -1 if _str1 < _ str2
						0  if _str1 == _str2
						1  if _str1 > _str2
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int alpha_numeric_comp(const std::string& _str1, const std::string& _str2);

// For use with std::sort(...) for vector.
struct alpha_numeric_comp_struct :
	public std::binary_function< std::string, std::string, bool >
{
	bool operator()(const std::string& _str1, const std::string& _str2) const
	{
		return (gs::reg_utils::string::alpha_numeric_comp(_str1, _str2) == -1);
	}
};

struct greater_alpha_numeric_comp :
		public std::binary_function< std::string, std::string, bool >
{
	bool operator()(const std::string& _str1, const std::string& _str2) const
	{
		return (gs::reg_utils::string::alpha_numeric_comp(_str1, _str2) == 1);
	}
};

typedef std::set< std::string, alpha_numeric_comp_struct > ANSTRSET;
typedef ANSTRSET::iterator ANSTRSETIT;
typedef ANSTRSET::const_iterator ANSTRSETCIT;
/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: compare_no_case
 DESCRIPTION    : performs case-insensitive comparison on two strings
				  for ex:- "Hello" will be equal to "hELLO"
 ASSUMPTIONS	: 
 PARAMETERS     : _str1, _str2 - strings to check
 RETURNS        : int:  -1 if _str1 < _ str2
						0  if _str1 == _str2
						1  if _str1 > _str2
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int compare_no_case(const std::string& _str1, const std::string& _str2);


/*--------------------------------------------------------------------
APPROVAL		: PENDING   
NAME			: is_string_in_cont
DESCRIPTION    : Returns true if the _str string is in the range of _vStart and _vend
                 performs case-sensitive or case-insensitive comparison based on the optional flag.
ASSUMPTIONS	: 
PARAMETERS     : _str1 strings to check, 
                 _vStart and _vend begin and end range of the container to check,
				 _caseSensitive optional flag for a case sensitive check.
RETURNS        : true if the string is in the container, false if not.
EXCEPTIONS		:
---------------------------------------------------------------------*/
template< typename It>
inline static bool is_string_in_cont( const std::string& _str, It _vStart, It _vend, bool _caseSensitive = false )
{
	bool retval = false;
	for( ; !retval && _vStart != _vend; ++_vStart)
	{
		if(    (!_caseSensitive && gs::reg_utils::string::compare_no_case( _str, *_vStart) == 0)
			|| _str == *_vStart)
			retval = true;
	}
	return retval;
}


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: get_next_non_empty_token
 DESCRIPTION    : get the next non-empty token in a delimited by token_delimiters.
				: The delimiters are treated by individual chars.
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to check
				: _token_delimiters - token delimiters
 RETURNS        : string - next non-empty token
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static std::string get_next_non_empty_token(std::string& _str, 
										const std::string& _token_delimiters="(),:");

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: get_occurances_of_char
 DESCRIPTION    : get the number of times a char appears in a string
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to check
				: char - char to check
 RETURNS        : int - number of occurances
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int get_occurances_of_char(const std::string& _str, char _ch);

/*--------------------------------------------------------------------

 APPROVAL		: PENDING   
 NAME			: get_occurances_of_string
 DESCRIPTION    : get the number of times a char appears in a string
 ASSUMPTIONS	: 
 PARAMETERS     : _str - string to check and find number of matches
				: _strToMatch - key string to match
 RETURNS        : int - number of occurances
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int get_occurances_of_string( const std::string& _str, const std::string& _strToMatch);

/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: find_first_instance
 DESCRIPTION    : Returns the index of the first instance of the search 
				  string in the input string vector
 ASSUMPTIONS	: 
 PARAMETERS     : _strs - vector of strings
				: _str - search string
 RETURNS        : if string is found it returns the index, otherwise -1
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int find_first_instance(const std::vector<std::string>& _strs, const std::string& _str);


/*--------------------------------------------------------------------

 CREATION DATE	: XX/XX/XX
 APPROVAL		: APPROVED   
 NAME			: find_last_instance
 DESCRIPTION    : Returns the index of the first instance of the search 
				  string in the input string vector
 ASSUMPTIONS	: 
 PARAMETERS     : _strs - vector of strings
				: _str - search string
 RETURNS        : if string is found it returns the index, otherwise -1
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static int find_last_instance(const std::vector<std::string>& _strs, const std::string& _str);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: 
 DESCRIPTION    : Removes the first instance of the search 
				  string in the input string vector
 ASSUMPTIONS	: 
 PARAMETERS     : _strs - vector of strings
				: _str - search string
 RETURNS        : void
 EXCEPTIONS		:
 
---------------------------------------------------------------------*/
static void remove_first_instance(std::vector<std::string>& _strs, const std::string& _str);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: 
 DESCRIPTION    : Replaces the first instance of the search string
 ASSUMPTIONS	: 
				  with a new string in the input string vector
 PARAMETERS     : _strs - vector of strings
				: _str - search string
				: _newStr - the string to replace
 RETURNS        : void
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static void replace_first_instance(std::vector<std::string>& _strs, const std::string& _str, const std::string& _newStr);


/*--------------------------------------------------------------------

 APPROVAL		: APPROVED   
 NAME			: 
 DESCRIPTION    : replaces every substring in _stringToModify that matches 
				  _sequenceToReplace with _replaceSequence
 ASSUMPTIONS	: 
 PARAMETERS     : _stringToModify - string to be searched and replaced
				: _sequenceToReplace - search string
				: _replaceSequence - new string in place of the search string
 RETURNS        : true - if a substitution was made
				  false - if no change was made
 EXCEPTIONS		:

---------------------------------------------------------------------*/
static bool find_and_replace(std::string& _stringToModify, const std::string& _sequenceToReplace, const std::string& _replaceSequence);


//--------------------------------------------------------------------
// APPROVAL:	PENDING
// DESCRIPTION: This find the _tobeReplaceCharacters in the 
//				_stringToModify and replace them with the 
//				_replacingCharacter.
// RETURN:  true if at least one cahracter was found and replaced.
//			fasle if no changes in the _stringToModify
//--------------------------------------------------------------------
static bool replace_chars(std::string& _stringToModify, const std::string& _tobeReplaceCharacters, const char _replacingCharacter);

static bool does_string_exist(const std::string& _str, const std::set<std::string>& _strsToFind);

//--------------------------------------------------------------------
// APPROVAL:	PENDING
// DESCRIPTION: String difference report
// PARAMETER: _stringA - first key string
//			  _stringB - second key string
//			  _differenceChar - character to return for a match char 
//			between strings
// RETURN: Returns a string that represents the differences
//			between two strings using Key 1 letters as the 
//			returned difference.	If either string is longer than
//			the other, the remaining characters are appended to the
//			return string.
// EXAMPLE: _stringA) ABCDEFG987
//			_stringB) ABCBEFG789
//			_diffChar) -
//			return)   ---D---9-7
//--------------------------------------------------------------------
static std::string string_difference( const std::string & _str1, const std::string & _str2, const char _matchChar);
static bool string_match( std::string _char_pattern, std::string _str, const char _ignore_char = '-');

//--------------------------------------------------------------------
// APPROVAL:	PENDING
// DESCRIPTION: Copies the input string to the clipboard
// RETURN:  
//--------------------------------------------------------------------
#ifdef IGNORE_WINDOWS
//#ifdef _WINDOWS
	static void copy_to_clipboard( const CString & _cstring);
	static void copy_to_clipboard( const std::string & _string);
#endif

//--------------------------------------------------------------------
// APPROVAL:	PENDING
// DESCRIPTION: Gets portions of a string that are qualified by a char.
// PARAMETER: _orig - string to parse
//			      _ch - delimeter character 
// RETURN: Returns a vector of strings. A string entry for each set of
//         charaters between two instances of the delimeter character. 
// EXAMPLE: _orig) "Almost -all of this- and -this- is extracted."
//			    _ch) '-'
//          return) {"-all of this-","-this-"}
//--------------------------------------------------------------------
static std::vector<std::string> get_qualified_text(const std::string & _orig, const char _ch);

//--------------------------------------------------------------------
// APPROVAL:  APPROVED
// DESCRIPTION: Determines if a string contains any of the strings in
//              a string vector
// PARAMETER: _testStr - string to parse
//			      _strVect - vector of strings to test against
// RETURN: Returns the first string in the vector that was found in
//         _testStr, or if not found, and empty string.
// EXAMPLE: _testStr) "This is a sentence that contains something to find."
//			    _strVect) {"something to find","that")
//          return) "something to find"
//--------------------------------------------------------------------
static bool does_string_contain(const std::string & _testStr, const std::vector<std::string> & _strVect, size_t & _index);

static std::string tabs( unsigned int _num_tabs);

}; // class string
} // end namespace gs::reg_utils
} // end namespace gs::reg_utils

#ifdef IGNORE_WINDOWS
//#ifdef _WINDOWS

//--------------------------------------------------------------------
// APPROVAL:	PENDING
// DESCRIPTION: CArchive operators for std::string.
//--------------------------------------------------------------------
CArchive & operator <<( CArchive& ar, std::string & _string);
CArchive & operator >>( CArchive& ar, std::string & _string);
CArchive & operator <<( CArchive& ar, const std::string & _string);
CArchive & operator <<( CArchive& ar, std::vector< std::string> & _vec);
CArchive & operator >>( CArchive& ar, std::vector< std::string> & _vec);
#endif


#endif // _STRINGGREENREG_UTILS_H_

#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
