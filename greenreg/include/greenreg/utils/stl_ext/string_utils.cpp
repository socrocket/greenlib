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


#include "greenreg/utils/stl_ext/string_utils.h"
#include <sstream>

#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

//#include <atlrx.h>
//#include <algorithm>
//#include <cstring>

#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "greenreg/utils/stl_ext/string_utils.h"

const std::string gs::reg_utils::string::WHITESPACES = " \t\r\f\v\n";

bool gs::reg_utils::string::does_pattern_match(const std::string& _str,
			 const std::string& _pattern,
			 const std::string& _wildcards,
			 bool _bCaseSensitive/* = true*/)
{
	std::string str = _str;
	std::string pattern = _pattern;

	if ( !_bCaseSensitive )
	{
		str = to_upper( str );
		pattern = to_upper( pattern );
	}

	// return value....assume a match
	bool match = true;
	// indicates whether or not the pattern std::string
	// starts with a wildcard...this is a special case
	bool startsWild;
	// indicates whether or not the pattern std::string
	// ends with a wildcard...this is a special case
	bool endsWild;
	// current index into std::string to match against
	std::string::size_type index = 0;

	// check for special case of starting pattern with wildcard
	std::string::size_type tmpPos = pattern.find_first_of(_wildcards);
	if (tmpPos == 0)
		startsWild = true;
	else
		startsWild = false;

	// check for special case of starting pattern with wildcard
	tmpPos = pattern.find_last_of(_wildcards);
	if (tmpPos != (pattern.size()-1))
		endsWild = false;
	else
		endsWild = true;

	// tokenize the _pattern into substrings based on wildcards
	std::list<std::string> substrings;
	gs::reg_utils::string::parse(pattern, substrings, _wildcards);

	std::list<std::string>::iterator theIt = substrings.begin();

	// if the pattern does not start wild, the first match must be
	// at pos 0...do this one separately if startsWild
	if (!startsWild)
	{
		// look for match at front
		index = str.find(*theIt,index);
		// failed to match beginning
		if (index != 0)
			return false;
		else
		{
			// move index past this substring
			index += (*theIt).size();
			// move to next substring
			++theIt;

		}
	}

	// loop until all substrings have been matched or end of str
	while (theIt != substrings.end()&& index != std::string::npos)
	{
		// find
		index = str.find(*theIt,index);
		if (index != std::string::npos)
		{
			// move index past this substring
			index += (*theIt).size();
			// move to next substring
			++theIt;
		}

	}

	// if could not find a substring or still have substrings to match
	if (index == std::string::npos || theIt != substrings.end())
		match = false;
	else
	{
		// if remaining chars in _str, must have ending wildcards
		if (index != str.size())
			if (!endsWild)
				match = false;
	}

	return match;
}

bool gs::reg_utils::string::does_filter_expression_match ( const std::string& _str,
									    const std::string& _filter )
{
	bool bMatch = true;

	// If the filter is empty the only true is returned if the string is also empty.
	if ( _filter.empty() ) return (_str.empty());
	// Stop checking if the filter is just a wildcard...
	if ( _filter == "?" ) return bMatch;

	// If the first letter of the filter is a "!" then we not with the rest of the comparison.
	if ( _filter.at(0) == '!' )
	{
		std::string filter = _filter.substr( 1 );

		if ( gs::reg_utils::string::does_pattern_match( _str, filter, "?", false ) )
			bMatch = false;
	}
	else if ( _filter.at(0) == '<' || _filter.at(0) == '>' )
	{
		bool bLessThan = false;
		bool bEqualTo = false;
		// Less than or greater than to comparison...
		if ( _filter.at(0) == '<' )
			bLessThan = true;

		std::string realFilter = _filter.substr( 1 );
		// Check for an equal sign...
		if ( !realFilter.empty() && realFilter.at(0) == '=' )
		{
			bEqualTo = true;
			realFilter = realFilter.substr(1);
		}

		int result = 0;
		if ( gs::reg_utils::string::is_double( _str ) && gs::reg_utils::string::is_double( realFilter ) )
		{
			// Numbers so convert... back..
			double dEntry = gs::reg_utils::string::to_double( _str );
			double dFilter = gs::reg_utils::string::to_double( realFilter );

			result = 0;
			if ( dEntry < dFilter )
				result = -1;
			else if ( dEntry > dFilter )
				result = 1;
		}
		else
		{
			// Not numbers but to a alphanumeric anyway...
			// Do an alpha_numeric_compare..
			result = gs::reg_utils::string::alpha_numeric_comp( _str, realFilter );
		}

		if ( (!bEqualTo && result == 0) )
			bMatch = false;
		else if ( bLessThan && result == 1)
			bMatch = false;
		else if (!bLessThan && result == -1 )
			bMatch = false;
	}
	else
	{
		// No operators just a string match..
		if (! gs::reg_utils::string::does_pattern_match( _str, _filter, "?", false ) )
			bMatch = false;
	}
	return bMatch;
}

// converts every character in the input string to upper-case if possible
std::string gs::reg_utils::string::to_upper(const std::string& _str)
{
	std::string retval = _str;
    for( std::string::iterator c = retval.begin(); c != retval.end(); ++c)
        *c = (char)toupper(*c);
    return retval;
}


// converts every character in the input string to upper-case if possible
std::string gs::reg_utils::string::to_lower(const std::string& _str)
{
	std::string retval = _str;
    for( std::string::iterator c = retval.begin(); c != retval.end(); ++c)
        *c = (char)tolower(*c);
    return retval;
}


/* tokenizes _str based on delimiters and stores them in a vector
loops thru every character in the string and whenever it finds
a delimiter it extracts the most recent token and stores it
in the vector
*/
void gs::reg_utils::string::parse(const std::string& _str,
						  std::vector<std::string>& _ovector,
						  const std::string& _delims /*= WHITESPACES*/,
						  bool _returnEmptyTokens /*= false*/)
{
	std::string::size_type start = 0; // start of a token
	std::string::size_type end = 0; // end of a token
	std::string::size_type size = _str.size();

	while( end < size ) {

		// is this character a delimiter ?
		if( std::string::npos != _delims.find(_str.at(end)) ) {
			if( start != end ) {
				// found a token, so store it in the vector
				_ovector.push_back(_str.substr(start, end-start));
//				start = end + 1; // move ahead of the delimiter
				start = end; // set start to end (so that delimiter can be parsed)

			} else { // token starts with a delimiter

				if( _returnEmptyTokens) // if returning an empty token push a blank
					_ovector.push_back( _str.substr( start, 1));
				++start; // increment start
				++end; // increment end
			}
		} else {
			++end; // increment end if no delimiters were found... (building token)
		}
	}

	if( start != end ) // lets not leave the last one out
		_ovector.push_back(_str.substr(start, end-start));
}


/* tokenizes _str based on delimiters and stores them in a vector
loops thru every character in the string and whenever it finds
a delimiter it extracts the most recent token and stores it
in the list
*/
void gs::reg_utils::string::parse(const std::string& _str,
						  std::list<std::string>& _olist,
						  const std::string& _delims /*= WHITESPACES*/,
						  bool _returnEmptyTokens /*= false*/)
{
	std::string::size_type start = 0; // start of a token
	std::string::size_type end = 0; // end of a token
	std::string::size_type size = _str.size();

	while( end != size ) {

		// is this character a delimiter ?
		if( std::string::npos != _delims.find(_str.at(end)) ) {
			if( start != end ) {
				// found a token, so store it in the list
				_olist.push_back(_str.substr(start, end-start));
				start = end + 1; // move ahead of the delimiter
			}
			else { // token starts with a delimiter, so ignore it
				++start;
				if( _returnEmptyTokens) // if returning an empty token push a blank
					_olist.push_back( "");
			}
		}

		++end;
	}

	if( start != end ) // lets not leave the last one out
		_olist.push_back(_str.substr(start, end-start));
}

void gs::reg_utils::string::parse(const std::string& _str,
						  std::set<std::string>& _oset,
						  const std::string& _delims /*= WHITESPACES*/,
						  bool _returnEmptyTokens /*= false*/)
{
	std::string::size_type start = 0; // start of a token
	std::string::size_type end = 0; // end of a token
	std::string::size_type size = _str.size();

	while( end < size ) {

		// is this character a delimiter ?
		if( std::string::npos != _delims.find(_str.at(end)) ) {
			if( start != end ) {
				// found a token, so store it in the vector
				_oset.insert(_str.substr(start, end-start));
				start = end + 1; // move ahead of the delimiter
			}
			else { // token starts with a delimiter, so ignore it
				++start;
				if( _returnEmptyTokens) // if returning an empty token push a blank
					_oset.insert( "");
			}
		}

		++end;
	}

	if( start != end ) // lets not leave the last one out
		_oset.insert(_str.substr(start, end-start));
}

// Removes leading removable characters from _str
std::string gs::reg_utils::string::trim_leading(const std::string& _str, const std::string& _chars /*= " \t\r\f\v\n"*/)
{
	// stores the index of the first non-removable character in the string
	std::string::size_type start = 0; 
	std::string::size_type size = _str.size();

	// loop from the beginning until you reach a non-removable character
	while( start < size && std::string::npos != _chars.find(_str.at(start)) ) ++start;

	// return the entire string from position start
	return _str.substr(start); // return trimmed string
}


// Removes trailing removable characters from _str
std::string gs::reg_utils::string::trim_trailing(const std::string& _str, const std::string& _chars /*= " \t\r\f\v\n"*/)
{
	int end = (int) _str.size() - 1; // stores the index of the first non-removable character in the string
							   // from the end

	// loop from the end until you reach a non-removable character
	while( end >= 0 && std::string::npos != _chars.find(_str.at(end)) ) --end;

	// return the entire string from position 0 to end
	return _str.substr(0, end+1); // return trimmed string
}


// Removes leading and trailing removable characters from _str
std::string gs::reg_utils::string::trim_leading_and_trailing(const std::string& _str, const std::string& _chars /*=" \r\t\n"*/ )
{
	int start = 0, size = (int) _str.size();
	// trim leading
	while( start < size && std::string::npos != _chars.find(_str.at(start)) ) ++start;

	int end = size - 1;
	// trim trailing
	while( end >= 0 && std::string::npos != _chars.find(_str.at(end)) ) --end;

	// return remaining characters
	if(start <= end)
		return _str.substr(start, end-start+1);
	else // probably the string is full of removable characters
		return std::string();
}


// Removes all removable characters from _str
std::string gs::reg_utils::string::trim_all(const std::string& _str, const std::string& _chars /*= " \t\r\f\v\n"*/)
{
	std::string strTrim;
	std::string::size_type nSize = _str.size();

	strTrim.reserve(nSize); // to avoid multiple reallocations possible when inserting characters
							// it allocates space for nSize characters

	std::string::size_type nSrc = 0;

	// loop thru the entire input string
	while( nSrc < nSize ) {
		// if it is a non-removable character append it to destination string
		if( std::string::npos == _chars.find(_str.at(nSrc)) ) {
			strTrim.append(1, _str.at(nSrc));
		}
		++nSrc;
	}

	return strTrim;
}


// returns a reversed string
std::string gs::reg_utils::string::reverse(const std::string& _str)
{
	std::string ostr;
	ostr.resize(_str.size()); // to avoid multiple reallocations possible when inserting characters

	for(std::string::size_type i = 0, size = _str.size(); i < size; ++i )
		ostr[size-i-1] = _str[i];

	return ostr;
}


// concatenates all the strings in the vector and adds delimiters to mark different strings
std::string gs::reg_utils::string::build_string(const std::vector<std::string>& _vstr,
									   const std::string& _delim /*= ""*/)
{
	std::string strReturn;
	std::vector<std::string>::const_iterator start, end;

	for( start = _vstr.begin(), end = _vstr.end(); start != end; ++start )
	{
		// add leading _delim to all entries except the first
		if (start != _vstr.begin())
			strReturn += _delim + *start;
		else
			strReturn += *start;
	}

	return strReturn;
}


// concatenates all the strings in the list and adds delimiters to mark different strings
std::string gs::reg_utils::string::build_string(const std::list<std::string>& _lstr,
									   const std::string& _delim /*= ""*/)
{
	std::string strReturn;
	std::list<std::string>::const_iterator start, end;

	for( start = _lstr.begin(), end = _lstr.end(); start != end; ++start )
	{
		// add leading _delim to all entries except the first
		if (start != _lstr.begin())
			strReturn += _delim + *start;
		else
			strReturn += *start;
	}

	return strReturn;
}


// returns the index of the longest string in the vector
// if the vector is empty it returns -1
int gs::reg_utils::string::get_longest_string_index(const std::vector<std::string>& _vstr)
{
	int nIndex = -1;
	std::string::size_type nLen, nMaxLen = 0;

	for(std::string::size_type i = 0, nSize = _vstr.size(); i < nSize; ++i ) {
		nLen = _vstr[i].size();
		if( nLen > nMaxLen ) { // new size greater than previously recorded max size
			nIndex = (int) i; // store the new index
			nMaxLen = nLen; // store the new max size
		}
	}

	return nIndex; // return index to max size
}


// returns the index of the longest string in the list
// if the list is empty it returns end iterator to std::list<std::string>
std::list<std::string>::const_iterator gs::reg_utils::string::get_longest_string_index(const std::list<std::string>& _lstr)
{
	int nMaxLen = -1, nLen;
	std::list<std::string>::const_iterator start, end;
	std::list<std::string>::const_iterator itReturn = _lstr.end();

	for( start = _lstr.begin(), end = _lstr.end(); start != end; ++start ) {
		nLen = (int) start->size();
		if( nLen > nMaxLen ) { // new size greater than previously recorded max size
			itReturn = start; // store the new iterator to max string
			nMaxLen = nLen; // store the new max size
		}
	}

	return itReturn; // return iterator to max size
}


// finds if the input string is an integer
// NOTE: it does not take overflows into account
bool gs::reg_utils::string::is_int(const std::string& _str)
{
	std::string strInt = trim_leading_and_trailing(_str);

	std::string::const_iterator start = strInt.begin();
	std::string::const_iterator end = strInt.end();

	if( strInt.size() ) {

		if( *start == '-' || *start == '+' ) ++start;
		if( start == end ) return false; // no digits present

		// loop until you find a non-digit character
		while( start != end && ::isdigit(*start) ) ++start;
	}
	else
		return false;  // empty string

	// if you are the end then all are digits otherwise not
	return start == end;
}


// converts the input string to an integer
int gs::reg_utils::string::to_int(const std::string& _str)
{
	// check if the string is a valid int
	if( is_int(_str))
	{
		int i = 0;
		#ifdef __SOLARIS_VERSION__
			i = atoi( _str.c_str());
			return i;
		#else
			// output the string to a string stream
			std::stringstream ss(_str);
			if( ss >> i)
				return i;
		#endif
	}

	// should not reach this point if the integer is valid
	throw("Could not convert string to integer");
}

unsigned int gs::reg_utils::string::to_uint(const std::string& _str)
{
	// check if the string is a valid int
	if( is_int(_str) )
	{
		unsigned int i = 0;
		#ifdef __SOLARIS_VERSION__
			i = (unsigned int)atoi( _str.c_str());
			return i;
		#else
			// output the string to a string stream
			std::stringstream ss(_str);
			if( ss >> i)
				return i;
		#endif
	}

	// should not reach this point if the integer is valid
	throw("Could not convert string to integer");
}

long gs::reg_utils::string::to_long(const std::string& _str)
{
	// check if the string is a valid int
	if( is_int(_str))
	{
		long i = 0;
		#ifdef __SOLARIS_VERSION__
			i = atol( _str.c_str());
			return i;
		#else
			// output the string to a string stream
			std::stringstream ss(_str);
			if( ss >> i)
				return i;
		#endif
	}

	// should not reach this point if the integer is valid
	throw("Could not convert string to integer");
}

unsigned long gs::reg_utils::string::to_ulong(const std::string& _str)
{
	// check if the string is a valid int
	if( is_int(_str))
	{
		unsigned long i = 0;
		#ifdef __SOLARIS_VERSION__
			i = (unsigned long)atol( _str.c_str());
			return i;
		#else
			// output the string to a string stream
			std::stringstream ss(_str);
			if( ss >> i)
				return i;
		#endif
	}

	// should not reach this point if the integer is valid
	throw("Could not convert string to integer");
}

// returns true if the conversion was successful, false otherwise
bool gs::reg_utils::string::to_int(const std::string& _str, int& _value)
{
	#ifdef __SOLARIS_VERSION__
		if( is_int(_str))
		{
			_value = atoi( _str.c_str());
			return true;
		}
		else
			return false;
	#else
		return ( (std::istringstream(_str) >> _value) != NULL );
	#endif
}

unsigned long gs::reg_utils::string::to_hex_value( std::string _str)
{
	std::stringstream ss;
	ss << std::hex;
	if( gs::reg_utils::string::does_pattern_match( _str, "0x?", "?"))
		gs::reg_utils::string::find_and_replace( _str, "0x", "");
	ss << _str;
	unsigned int retval;
	ss >> retval;
	return retval;
}

// finds if the input string is an double
// NOTE: it does not take overflows into account
bool gs::reg_utils::string::is_double(const std::string& _str)
{
	std::string strInt = trim_leading_and_trailing(_str);

	std::string::size_type nSize = strInt.size();

	std::string::const_iterator start = strInt.begin();
	std::string::const_iterator end = strInt.end();

	if( nSize ) {

		// begins with a '+' or '-'
		if( *start == '-' || *start == '+' )  {
			++start;
			--nSize;
		}
		if( start == end )
			return false; // no digits present

		int nCount = (int) gs::reg_utils::string::get_occurances_of_char( strInt, '.');
		if( nCount > 1 )
			return false; // more than one dot
		else if( nCount == 1 && nSize == 1 )
			return false; // no digits present just either a sign and dot or just a dot

		// loop until you find a non-digit or non-dot character
		while( start != end && ( *start == '.' || ::isdigit(*start) ) ) ++start;
	}
	else
		return false;   // empty string

	// if you are the end then all are digits otherwise not
	return start == end;
}


// converts a string to double if possible, otherwise throws an exception
double gs::reg_utils::string::to_double( const std::string& _str)
{
	if( is_double(_str))
	{
		double d;
		#ifdef __SOLARIS_VERSION__
			d = atof( _str.c_str());
			return d;
		#else
			std::stringstream ss(_str);
			if(ss >> d)
				return d;
		#endif
	}

	// should not reach this point if the double is valid
	throw("Could not convert string to double");
}

double gs::reg_utils::string::force_to_double( const std::string & _str)
{
	std::string str = "";
	str = _str;
	str = gs::reg_utils::string::trim_all( _str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+-=\\|[]{};':,<>/?`~ \n\r\t");
	if( is_double(str))
		return( atof( str.c_str()));

	// should not reach this point if the double is valid
	throw("Could not convert string to double");
}

// returns true if the conversion was successful, false otherwise
bool gs::reg_utils::string::to_double(const std::string& _str, double& _value)
{

	if( is_double(_str))
	{
		#ifdef __SOLARIS_VERSION__
			_value = atof( _str.c_str());
			return true;
		#else
			return ( (std::istringstream(_str) >> _value) != NULL );
		#endif
	}
	else
		return false;

}


// converts a double to string
std::string gs::reg_utils::string::to_string(const double _value, const int _fixedDecimalPlaces /*= -1*/)
{
	std::ostringstream oss;

	if( _fixedDecimalPlaces > -1) {
		oss << std::setprecision( _fixedDecimalPlaces ) << std::fixed;
	}

	if( oss << _value ) // output double to string stream
		return oss.str(); // retrieve as string
	else
	{
		throw("Could not convert double to string.");
	}
}


/* Finds if _substr is a word sub string of _str.
   For example, "at" is a word sub string of "at least" but not of "atleast".
   A word is assumed to be delimited by punctuations or white spaces*/

bool gs::reg_utils::string::is_word_substr(const std::string& _str, const std::string& _substr)
{
	std::string::size_type nPos = _str.find( _substr );

	if( nPos != std::string::npos ) { // found substring

		std::string::size_type nSubLen = _substr.length();
		// check the char before and after the substring
		if( ( 0 == nPos ||
				::ispunct( _str[nPos-1] ) || ::isspace( _str[nPos-1] ) ) &&
			( _str.length() == nPos + nSubLen ||
				::ispunct( _str[nPos+nSubLen] ) || ::isspace( _str[nPos+nSubLen] ) )
		  )
			return true;
	}

	return false;
}


// performs a special comparison in which numbers and strings are compared differently
// whenever we find a number then we compare their absolute value instead of usual string
// comparison. so comparing a10 and a9 will return a10 as greater
int gs::reg_utils::string::alpha_numeric_comp( const std::string& _str1, const std::string& _str2 )
{
	std::string::size_type i1, i2, starti1, starti2; // indices
	std::string::size_type size1, size2;
	int val1, val2; // for comparing numbers in strings

	i1 = i2 = starti1 = starti2 = 0;
	size1 = _str1.size(); size2 = _str2.size();

	int ret = 0;

	while( true ) {

		/*
		for both the strings, loop until you find a non-digit character
		*/

		starti1 = i1;
		while( ::isdigit(_str1[i1]) && i1 < size1 ) ++i1;

		starti2 = i2;
		while( ::isdigit(_str2[i2]) && i2 < size2 ) ++i2;

		// one of them starts with a non-digit char and the other one does not
		if( (starti1 != i1 && starti2 == i2) || (starti1 == i1 && starti2 != i2) ) {
			if( size1 != i1 && size2 == i2 ) // str2 has reached the end before str1 so str1 is greater
				return 1;
			else if( size1 == i1 && size2 != i2 )  // str1 has reached the end before str2 so str2 is greater
				return -1;
			else if( size1 != i1 && size2 != i2 ) // neither have reached the end, so we have a non-digit and
												  // a digit, so just compare and return
				return _str1[starti1] < _str2[starti2] ? -1 : 1;
		}

		// both of them have a valid number in them, so lets compare the value of the numbers
		val1 = ::atoi( _str1.substr(starti1, i1-starti1).c_str() );
		val2 = ::atoi( _str2.substr(starti2, i2-starti2).c_str() );

		// bail out if the values are different
		if( val1 != val2 )
			return val1 < val2 ? -1 : 1;
		else if( i1 == size1 && i2 == size2 ) // check if we are the end
			return 0;

		/*
		for both the strings, loop until you find a digit
		*/

		starti1 = i1;
		while( ! ::isdigit(_str1[i1])&& i1 < size1 ) ++i1;

		starti2 = i2;
		while( ! ::isdigit(_str2[i2])&& i2 < size2 ) ++i2;

		// compare the newly extracted strings (without digits)
		ret = _str1.substr(starti1, i1-starti1).compare(_str2.substr(starti2, i2-starti2));
		// bail out if they are not equal
		if( ret != 0 )
			return ret;
		else if( i1 == size1 && i2 == size2 ) // check if we are the end
			return 0;
	}
}


// Performs case-insensitive comparison on two strings
int gs::reg_utils::string::compare_no_case( const std::string& _str1, const std::string& _str2)
{
	std::string::size_type nLen1 = _str1.size(), nLen2 = _str2.size();
	int nRet;

	if( nLen1 < nLen2 )
	{
		// Different names method: strnicmp on VC++, strncasecmp for GCC
		#ifdef _WINDOWS
			nRet = ::strnicmp(_str1.c_str(), _str2.c_str(), nLen1);
		#else
			nRet = ::strncasecmp(_str1.c_str(), _str2.c_str(), nLen1);
		#endif

		if( nRet == 0 ) // the strings are equal upto nLen1 number of characters
			nRet = -1;	// and nLen1 < nLen2, so str1 < str2 is true
	}
	else if( nLen1 > nLen2 )
	{
		// Different names method: strnicmp on VC++, strncasecmp for GCC
		#ifdef _WINDOWS
			nRet = ::strnicmp(_str1.c_str(), _str2.c_str(), nLen2);
		#else
			nRet = ::strncasecmp(_str1.c_str(), _str2.c_str(), nLen2);
		#endif

		if( nRet == 0 ) // the strings are equal upto nLen2 number of characters
			nRet = 1;		// and nLen1 > nLen2, so str1 > str2 is true
	}
	else
	{
		// Different names method: strnicmp on VC++, strncasecmp for GCC
		#ifdef _WINDOWS
			nRet = ::strnicmp(_str1.c_str(), _str2.c_str(), nLen2);
		#else
			nRet = ::strncasecmp(_str1.c_str(), _str2.c_str(), nLen2);
		#endif
	}

	return nRet;
}


// get the next non empty token in a std::string. token delimiters are also returned.
std::string gs::reg_utils::string::get_next_non_empty_token(std::string & _str,
												const std::string& _token_delimiters /*="(),:"*/){
	try {
		std::string token;

		std::string::size_type endPos = _str.size();
		for(std::string::size_type i=0; i<_token_delimiters.size(); i++) {
			if(_str.find(_token_delimiters.at(i))<endPos)
				endPos = _str.find(_token_delimiters.at(i));
		}
		// If no delimiter found, return the string
		if(endPos==_str.size())
		{
			token = _str;
			_str.erase();
			return token;
		}
		if(endPos==0) ++endPos;
		// trims leading and trailing whitespace from token
		token = string::trim_leading_and_trailing(_str.substr(0,endPos));
		_str = string::trim_leading_and_trailing(_str.substr(endPos, _str.size()));
		return token;
	} catch(...) {
		throw;
	}
}


// get the number of times a character appears in a std::string
int gs::reg_utils::string::get_occurances_of_char(const std::string& _str, const char _ch)
{
#ifdef _WINDOWS
	std::string::size_type st = _str.find( _ch, 0);
	int count = 0;
	if( st != 0)
	{
		std::string::size_type first = st;
		count++;
		st = _str.find( _ch, 0);
		while( st != first && st != 0)
		{
			count++;
			st = _str.find( _ch, 0);
		}
	}

	return count;
#else
	return (int) std::count(_str.begin(), _str.end(), _ch);
#endif
}

// identifies and returns the number of times a substring is represented in a string
int gs::reg_utils::string::get_occurances_of_string( const std::string& _str, const std::string& _strToMatch)
{
	int retval = 0;

	// try to find an initial instance
	std::string::size_type index = _str.find( _strToMatch);

	// now loop through while continuing to find instances and count them
	while( index != std::string::npos)
	{
		retval++; // increment counter
		index += _strToMatch.size(); // adjust index by match string size
		index = _str.find(_strToMatch, index); // try to find another occurance
	}

	// in no case will this method return < 0, but used int instead of unsigned int to
	// be consistant with get_occurances_of_char method.
	return retval;
}


// returns the index of the first occurance of _str in vector _strs
// if the string is not found, then it returns -1
int gs::reg_utils::string::find_first_instance( const std::vector<std::string>& _strs, const std::string& _str)
{
	int icounter = 0;
	bool found = false;

	while( icounter < (int)_strs.size() && !found) {
		if( _str == _strs[icounter]) found = true;
		++icounter;
	}

	if( found) --icounter;
	else icounter = -1;

	return( icounter);
}


// returns the index of the last occurance of _str in vector _strs
// if the string is not found, then it returns -1
int gs::reg_utils::string::find_last_instance(const std::vector<std::string>& _strs, const std::string& _str)
{
	int icounter = (int)_strs.size()-1;
	bool found = false;

	while( icounter > -1 && !found) {
		if( _str == _strs[icounter]) found = true;
		--icounter;
	}

	if( found) ++icounter;
	else icounter = -1;

	return( icounter);
}


// removes the first occurance of _str in vector _strs, if found
void gs::reg_utils::string::remove_first_instance(std::vector< std::string>& _strs, const std::string& _str)
{
	std::vector< std::string>::iterator strsIterator;
	bool found = false;
	strsIterator = _strs.begin();

	while( strsIterator != _strs.end() && !found) {
		if( (*strsIterator) == _str) found = true;
		strsIterator++;
	}

	if( found) {
		strsIterator--;
		_strs.erase( strsIterator);
	}
}


// replaces the first occurance of _str in vector _strs with _newStr, if found
void gs::reg_utils::string::replace_first_instance(std::vector<std::string>& _strs, const std::string& _str, const std::string& _newStr)
{
	std::vector< std::string>::iterator strsIterator;
	bool found = false;
	strsIterator = _strs.begin();

	while( strsIterator != _strs.end() && !found) {
		if( (*strsIterator) == _str) found = true;
		strsIterator++;
	}

	if( found) {
		strsIterator--;
		*strsIterator = _newStr;
	}
}


// replaces all occurances of string _sequenceToReplace in string _stringToModify with string _replaceSequence
bool gs::reg_utils::string::find_and_replace( std::string& _stringToModify, const std::string& _sequenceToReplace, const std::string& _replaceSequence)
{
	bool retval = false;

	std::string::size_type index = _stringToModify.find( _sequenceToReplace);
	while( index != std::string::npos)
	{
		_stringToModify.erase( index, _sequenceToReplace.size());
		_stringToModify.insert( index, _replaceSequence);

		retval = true;

		index += _replaceSequence.size();
		index = _stringToModify.find(_sequenceToReplace, index);
	}
	return retval;
}

//--------------------------------------------------------------------
// APPROVAL:	PENDING
// DESCRIPTION: This find the _tobeReplaceCharacters in the
//				_stringToModify and replace them with the
//				_replacingCharacter.
// RETURN:  true if at least one cahracter was found and replaced.
//			fasle if no changes in the _stringToModify
//--------------------------------------------------------------------
bool gs::reg_utils::string::replace_chars(std::string& _stringToModify, const std::string& _tobeReplaceCharacters, const char _replacingCharacter){
	bool modified = false;

	for(std::basic_string<char>::iterator stringIter = _stringToModify.begin();
		stringIter != _stringToModify.end(); ++stringIter){
			if(_tobeReplaceCharacters.find(*stringIter) != std::basic_string<char>::npos){
				*stringIter = _replacingCharacter;
				modified = true;
			}
	}

	return modified;
}

bool gs::reg_utils::string::does_string_exist(const std::string & _str, const std::set<std::string> & _strsToFind)
{
	return ( _strsToFind.count( _str ) != 0 );
}

std::string gs::reg_utils::string::string_difference( const std::string & _str1, const std::string & _str2, const char _matchChar)
{
	std::string diff;
	::size_t size1 = _str1.size();
	::size_t size2 = _str2.size();
	char char1;
	char char2;
	::size_t counter = 0;

	// for each character in string 1
	while( counter < size1) {

		// get string 1 character at this index
		char1 = _str1.at(counter);

		// if string 2 has characters at this index
		if( counter < size2) {

			// get string 2 character at this index
			char2 = _str2.at( counter);

			// if the characters are not equal at this index
			if( char1 != char2)
				// add the character difference from the first string
				diff += char1;

			// otherwise if the charcters are equal a this index
			else
				// add the _matchChar supplement to the string for this index
				diff += _matchChar;

		// if string 1 is longer than string 2
		} else {

			// simply add char for this index from string 1
			diff += _str1.at( counter);
		}

		// increment the counter
		counter++;
	}

	// if string 2 is longer than string 1, use its extra characters for the difference
	if( size2 > size1) {

		// no reason to reset the counter, just loop the rest of the way for string 2
		while( counter < size2) {

			// add the next character of string 2 at this index (since non exist for string 1)
			diff += _str2.at(counter);

			// increment the counter
			counter++;
		}
	}

	// done
	return( diff);
}

bool gs::reg_utils::string::string_match( std::string _char_pattern, std::string _str, const char _ignore_char)
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


/****************************************************/
/****************************************************/
//
//		WINDOWS ONLY BELOW THIS COMMENT
//
/****************************************************/
/****************************************************/

//#ifdef 0
//#ifdef _WINDOWS
#ifdef ALWAYS_EXCLUDE

void gs::reg_utils::string::copy_to_clipboard( const CString & _cstring) {

	// verify that the clipboard can be opened
	int ok = OpenClipboard(NULL);

	// if no clipboard handler can be obtained, we are not going to put anything onto the clipboard
	if (!ok) return;

	// Get the string into a buffer for the clipboard and lock the buffer while writing
	HGLOBAL clipbuffer;
	char * buffer;
	EmptyClipboard(); // this will blast anyone elses data off the clipboard
	clipbuffer = Global_alloc(GMEM_DDESHARE, strlen( (LPCSTR)_cstring)+1);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy(buffer, (LPCSTR)_cstring);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT,clipbuffer);

	// It's closing time...
	CloseClipboard();
}

void gs::reg_utils::string::copy_to_clipboard( const std::string & _string) {
	CString cstring = _string.c_str();
	gs::reg_utils::string::copy_to_clipboard( cstring);
}

CArchive & operator <<( CArchive& ar, std::string & _string) {

	CString cstring = _string.c_str();
	ar << cstring;

	return ar;
}

CArchive & operator <<( CArchive& ar, const std::string & _string) {

	CString cstring = _string.c_str();
	ar << cstring;

	return ar;
}

CArchive & operator >>( CArchive& ar, std::string & _string) {

	CString cstring;
	ar >> cstring;
	_string = (LPCSTR) cstring;
	if( _string.size() == 0)
		_string = ""; // fix for initialization problem

	return ar;
}

CArchive & operator <<( CArchive& ar, std::vector< std::string> & _vec) {
	std::vector< std::string>::iterator it;

	ar << (unsigned int) _vec.size();

	it = _vec.begin();
	while( it != _vec.end()) {
		ar << *it;
		it++;
	}

	return ar;
}

CArchive & operator >>( CArchive& ar, std::vector< std::string> & _vec) {
	unsigned int size;
	std::string s;

	ar >> size;
	for( int icounter = 0; icounter < size; icounter++) {
		s = "";
		ar >> s;
		_vec.push_back( s);
	}

	return ar;
}

#endif // _WINDOWS

std::vector<std::string> gs::reg_utils::string::get_qualified_text(const std::string &_orig, const char _ch)
{
	std::string str;
	std::vector<std::string> results;
	bool keep=false;
	str="";
	for(size_t i=0;i<_orig.length();i++)
	{
		if(_orig.at(i)==_ch)
		{
			if(!str.empty())
			{
				str+=_orig.at(i);
				results.push_back(str);
				keep=false;
			}
			else
				keep=true;
			str="";
		}
		if(keep)
			str+=_orig.at(i);
	}
	return results;
}

bool gs::reg_utils::string::does_string_contain(const std::string &_testStr, const std::vector<std::string> & _strVect, size_t & _index)
{
	_index = 0;
	for(std::vector<std::string>::const_iterator iter=_strVect.begin(); iter!=_strVect.end(); ++iter, ++_index)
	{
		if(_testStr.find(*iter)!=std::string::npos)
			return true;
	}
	return false;
}

std::string gs::reg_utils::string::tabs( unsigned int _num_tabs)
{
	std::stringstream ss;
	for( unsigned int i = 0; i < _num_tabs; i++)
	{
		ss << "\t";
	}
	return( ss.str());
}


#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
