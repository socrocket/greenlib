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


#define EXCLUDE_FROM_BUILD
#ifndef EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD

#include "file_stream_tokenizer.h"
#include "greenreg/utils/stl_ext/string_utils.h"
#include <sstream>
#include <string>
#include <iostream>

//#include "MessageEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace gs::reg_utils;

file_stream_tokenizer::file_stream_tokenizer( std::ifstream* infile_stream) :
			// default delimeters
    begin ('\0')
    , token ("")
    , end ('\0')
	, m_comment ('\0')	
    , m_comment_string ("")
	, m_stop_on_comments_only (false)
	, m_current_line_number (1)
	, m_delimiters ("")
	, m_was_comment (false)
	, m_clear_all (false)
	, m_clear_leading (true)
	, m_clear_trailing (true)
	, m_whitespace (" \t\r\n")
	{
	if( infile_stream->is_open() ) {		

		if( !infile_stream->eof())
		{
#ifdef STD_STL
			infile_stream->get( *(this->m_sstream.rdbuf()), 
				std::iostream::traits_type::to_char_type(std::iostream::traits_type::eof()));
#else // STLPort
			infile_stream->get( *(this->m_sstream.rdbuf()), 
				std::char_traits<char>::to_char_type( std::char_traits<char>::eof()));
#endif
		}

	} else
        throw("Invlaid ifstream passed to file_stream_tokenizer::file_stream_tokenizer");
}


file_stream_tokenizer::~file_stream_tokenizer()
{
	// Nothing to clean up.
}

std::string file_stream_tokenizer::add_delimiters( const std::string& _delimiters)
{   
    return m_delimiters += _delimiters;
}

std::string file_stream_tokenizer::del_delimiters( const std::string& _delimiters)
{   
    std::string::size_type target;
    for( std::string::const_iterator index = _delimiters.begin(); index != _delimiters.end(); ++index)
	{
		target = m_delimiters.find(*index);
		while(target != std::basic_string<char>::npos )
		{
			m_delimiters.erase(target,1);
			target = m_delimiters.find(*index);
		}
    }

    return m_delimiters;
}

std::string file_stream_tokenizer::set_delimiters( const std::string& _delimiters)
{   
    return m_delimiters = _delimiters;
}

std::string file_stream_tokenizer::add_token_delimiters( const std::string& _delimiters)
{   
    return token_delimiters += _delimiters;
}

std::string file_stream_tokenizer::del_token_delimiters( const std::string& _delimiters)
{   
	std::string::size_type target;
    for( std::string::const_iterator index = _delimiters.begin(); index != _delimiters.end(); ++index)
	{        
		target = token_delimiters.find(*index);
		while( target != std::basic_string<char>::npos)
		{
			token_delimiters.erase( target, 1);
			target = token_delimiters.find(*index);
		}
    }

    return token_delimiters;
}

std::string file_stream_tokenizer::set_token_delimiters( const std::string& _delimiters)
{   
    return token_delimiters = _delimiters;
}

std::string file_stream_tokenizer::get_next_token()
{
    std::basic_string<char>::size_type pos;

    char c;

	// flags 
    bool done = false;
    m_was_comment = false;
    m_comment_string = "";
	// delete the last token
    token = "";

	// only move begin if end was not
	// reset to NULL by a tokenDelimeter
	if (end != '\0')
		begin = end;
  
	// check if the last delimeter found is a tokenDelimeter...if it is,
	// make it the current token and return.
	if ((pos = token_delimiters.find(end)) != std::basic_string<char>::npos)
	{
		token = end;
		end = '\0';
		return token;
	}

	// read the next token from the stream
	while( !done && !m_sstream.eof())
	{
        m_sstream.get(c);
		
		// track the line number (newline characters)
        if( c == '\n')
		{
            m_current_line_number++;
        }
        
		// skip rest of line if m_comment was found and then continue
		// reading for token
        if( c == m_comment )
		{
            m_was_comment = true;
			m_sstream.get(c);
            while( c != '\n' && !m_sstream.eof())
			{
				//if ( isprint(c ) )
					m_comment_string += c; 

				m_sstream.get(c);
			}
            m_current_line_number++;

            if( !token.empty() || m_stop_on_comments_only)
			{
                done = true;
            }

        }
		else if (m_sstream.eof())
		{
			// do nothing
			done = true;
		}
		else
			// check if delimeter found
			if( (pos = m_delimiters.find(c)) != std::basic_string<char>::npos)
			{             
				done = true;    
				end  = m_delimiters.at(pos);
			} 
			else
				// check if a token delimeter is found
				if ((pos = token_delimiters.find(c)) != std::basic_string<char>::npos)
				{
					done = true;
					end	= token_delimiters.at(pos);
				} 
				else 
					// no delimeter found, just keeping adding characters
					// to the token
				{
					token += c;
				}
	}

	// trim m_whitespace
	if( m_clear_all) 
		token = gs::reg_utils::string::trim_all( token, m_whitespace);
	else if( m_clear_leading && m_clear_trailing) 
		token = gs::reg_utils::string::trim_leading_and_trailing( token, m_whitespace);
	else if( m_clear_leading) 
		token = gs::reg_utils::string::trim_leading( token, m_whitespace);
	else if( m_clear_trailing) 
		token = gs::reg_utils::string::trim_trailing( token, m_whitespace);

	//*** Change this to a function if not wanting to use CString
/*	CString st;
	st = token.c_str();
	st.TrimLeft();
	st.TrimRight();
	token = st;
*/
    return token;
}



std::string file_stream_tokenizer::get_next_non_empty_token()
{
	get_next_token();
	while(!m_sstream.eof() && token=="" && !( this->m_was_comment && this->m_stop_on_comments_only ))
	{
		get_next_token();
	}
//	S4( token);
	return token;
}

int file_stream_tokenizer::get_next_line_of_tokens( std::vector<std::string> &tokens)
{
	tokens.clear();

	get_next_non_empty_token();
	while ((token != "") && (token != "\n"))
	{
		tokens.push_back(token);
		get_next_non_empty_token();
	}

	return (int)tokens.size();
}


int file_stream_tokenizer::get_next_line_of_all_tokens( std::vector<std::string> &tokens)
{
	tokens.clear();

	get_next_token();
	while (!end_of_file() && (token != "\n"))
	{
		tokens.push_back(token);
		get_next_token();
	}


	return (int)tokens.size();
}

void file_stream_tokenizer::set_comment( char newComment, bool _StopOnCommentsOnly)
{
    m_comment = newComment;
	this->m_stop_on_comments_only = _StopOnCommentsOnly;
}

int file_stream_tokenizer::get_line_number()
{   
    return m_current_line_number;
}



void file_stream_tokenizer::put_back( const std::string& _str)
{
    std::string::const_reverse_iterator reverseIt;
	for( reverseIt = _str.rbegin(); reverseIt != _str.rend(); ++reverseIt)
	{
		if( *reverseIt == '\n')
			m_current_line_number--;
		m_sstream.putback(*reverseIt);
	}
}// file_stream_tokenizer::put_back

bool file_stream_tokenizer::end_of_file()
{
    return m_sstream.eof();
}

bool file_stream_tokenizer::was_comment()
{
    return m_was_comment;
}

int file_stream_tokenizer::get_next_token_integer()
{
	// flag used when checking for 0 special case.
	bool isValid = true;

	// get the next non-empty token
	get_next_non_empty_token();

	// check eof
	if (end_of_file())
		throw("UnExpected EOF in file_stream_tokenizer::get_next_token_integer");
	
	// change the token to an integer
	int tmp = atoi(token.data());

	// if atoi has an error it returns 0.  If tmp == 0,
	// there may be an error or the token/integer was
	// 0.  Test for special case of 0.
	if (tmp == 0)
	{
		unsigned int i = 0;
		// check first character for + or -
		// this allows for +0 or -0
		if (token[i] == '+' || token[i] == '-')
			i++;

		// The rest of token must be zeros
		while (i < token.size() && isValid)
		{
			isValid = (token[i] == '0');
			++i;
		}
	}

	// check if problem was found
	if (isValid)
		return tmp;
	else
		throw("Invliad token in file_stream_tokenizer::get_next_token_integer");
}


bool file_stream_tokenizer::get_next_token_bool()
{
	bool retval = false;

	// get the next non-empty token
	get_next_non_empty_token();

	// check eof
	if (end_of_file())
		throw("UnExpected EOF in file_stream_tokenizer::get_next_token_bool");
	
	// change the integer to a boolean
	if( token == "1")
		retval = true;
	else if( token == "0")
		retval = false;
	else
		throw("Invalid token in file_stream_tokenizer::get_next_token_bool");

	return retval;
}


double file_stream_tokenizer::get_next_token_double()

{
	// flag used when checking for 0.0 special case.
	bool isValid = true;
	// signals that a '.' has already been found in the token
	// used for 0.0 special case
	bool dotFound = false;
	// flag used for special case check...signals that a '.' cannot
	// occur in location 1;
	bool foundSign = false;

	get_next_non_empty_token();
	// check eof
	if (end_of_file())
		throw("UnExpected EOF in file_stream_tokenizer::get_next_token_double");
	
	double tmp = atof(token.data());

	// if atof has an error it returns 0.  If tmp == 0,
	// there may be an error or the token/double was
	// 0.0  Test for special case.

	if (tmp == 0.0)
	{
		unsigned int i = 0;

		// check first character for + or -
		if (token[i] == '+' || token[i] == '-')
		{
			++i;
			foundSign = true;
		}

		// gives the location of where the first digit
		// should be....this finds cases where the '.'
		// comes before any digits
		//int firstDig = i;
		// check rest of token for zeros and '.'
		while (i < token.size() && isValid)
		{
			// check for 0 or '.'
			isValid = (token[i] == '0' || token[i] == '.');

			// check the position of '.' if it exists
			if (token[i] == '.')
			{
				// can't start with a '.'
				if (i == 0)
					isValid = false;
				// can't end with a '.'
				if (i == (token.size() - 1))
					isValid = false;
				// can't be after a sign
				if (foundSign && (i == 1))
					isValid = false;
				// can't have more than two '.'
				if (dotFound)
					isValid = false;
				else
					dotFound = true;
			}

			++i;
		}
	}

	// check for any problems
	if (isValid)
		return tmp;
	else
		throw("Invalid token in file_stream_tokenizer::get_next_token_double");

}// file_stream_tokenizer::get_next_token_double


// CV - cap func name, describe...  move to string class... // verify with BN
bool file_stream_tokenizer::token_equal_char( const std::string& _token, const std::string& _chars) 
{
	bool retval = false;

	if( (_token.size() == 1) && (_chars.find( token) != std::string::npos)) 
		retval = true;
	return retval;
}


#endif // EXCLUDE_FROM_BUILD 
#undef EXCLUDE_FROM_BUILD
