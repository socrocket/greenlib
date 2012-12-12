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

#if !defined _FILE_STREAM_TOKENIZER_H_
#define _FILE_STREAM_TOKENIZER_H_

//------------------
// Include files.
//------------------
#include <fstream>
#include <string>
#include <exception>
#include <vector>
#include <sstream>

namespace gs {
namespace reg_utils {

//--------------------------------------------------------------------
// CLASS NAME   : file_stream_tokenizer
// DESCRIPTION  : file_stream_tokenizer mimics java's StreamTokenizer for
//              : file io.  It parses the file using specified m_delimiters.
//--------------------------------------------------------------------
class file_stream_tokenizer
{
public:

    // Members.
    char   begin;
    std::string token;
    char   end;

//--------------------------------------------------------------------
// Implementation of class file_stream_tokenizer.
//--------------------------------------------------------------------
// METHOD       :  file_stream_tokenizer()
// DESCRIPTION  :  Constructor. Throws exception if an invalid ifstream is passed in.
// PARAMETER    :  infile_stream - pointer to an opened file stream.
// RETURN VALUE :  None.
//--------------------------------------------------------------------
	file_stream_tokenizer( std::ifstream* infile_stream);

//--------------------------------------------------------------------
// METHOD       :  ~file_stream_tokenizer()
// DESCRIPTION  :  Destructor.
// PARAMETER    :  None.
// RETURN VALUE :  None.
//--------------------------------------------------------------------
	~file_stream_tokenizer();

//--------------------------------------------------------------------
// METHOD       :  get_next_token()
// DESCRIPTION  :  This method reads characters from the stream until
//					a delimeter is encountered.  _all characters read
//					are placed into a token.
//					If a tokenDelimeter is in the end
//					position when this function is called, it is made
//					the current token and returned; nothing new is read from
//					the stream. The end position will then be changed to NULL.
//					If the end position is a not a tokenDelimeter when this
//					function is called, the next token is read from the stream
//					and returned.  _all m_whitespace is trimmed from the beginning
//					and end of the token, unless m_whitespace is not a delimiter.
// PARAMETER    :  None.
// RETURN VALUE :  string token = the string tokenized.
//--------------------------------------------------------------------
	std::string get_next_token();

//--------------------------------------------------------------------
// METHOD       :  get_next_non_empty_token()
// DESCRIPTION  :  Continually calls get_next_token() until eof or
//					a non-empty token is returned.
// PARAMETER    :  None.
// RETURN VALUE :  string token = the string tokenized.
//--------------------------------------------------------------------
	std::string get_next_non_empty_token();

//--------------------------------------------------------------------
// METHOD       :  get_next_line_of_tokens()
// DESCRIPTION  :  Reads in the next line of NON-EMPTY tokens.
//					Places the tokens
//					in the tokens vector and returns
//					the number of read-in tokens (excluding token m_delimiters)
//					as the return value.
// PARAMETER    :  vector tokens = a vector containing string types.  This
//					procedure will fill the  tokens vectors with the tokens from
//					the next line.
// RETURN VALUE :  int count = the number of tokens on the next line not
//					not counting token m_delimiters.
//--------------------------------------------------------------------
	int get_next_line_of_tokens( std::vector<std::string> &tokens);

//--------------------------------------------------------------------
// METHOD       :  get_next_line_of_all_tokens()
// DESCRIPTION  :  Reads in the next line of ALL tokens.
//					Places the tokens
//					in the tokens vector and returns
//					the number of read-in tokens (excluding token m_delimiters)
//					as the return value.
// PARAMETER    :  vector tokens = a vector containing string types.  This
//					procedure will fill the  tokens vectors with the tokens from
//					the next line.
// RETURN VALUE :  int count = the number of tokens on the next line not
//					not counting token m_delimiters.
//--------------------------------------------------------------------
	int get_next_line_of_all_tokens( std::vector<std::string> &tokens);

//--------------------------------------------------------------------
// METHOD       :  get_next_token_double()
// DESCRIPTION  :  Reads the next (non-empty)token and converts it to a double.
//					If the next token is not a valid double, an exception
//					is thrown.
// PARAMETER    :  None.
// RETURN VALUE	: (double) - The next token converted to an double.
//--------------------------------------------------------------------
	double get_next_token_double();

//--------------------------------------------------------------------
// METHOD       :  get_next_token_integer()
// DESCRIPTION  :  Reads the next (non-empty)token and converts it to an integer.
//					If the next token is not a valid integer, an exception
//					is thrown.
// PARAMETER    :  None.
// RETURN VALUE	: (integer) - The next token converted to an integer.
//--------------------------------------------------------------------
	int get_next_token_integer();

//--------------------------------------------------------------------
// METHOD       :  get_next_token_bool()
// DESCRIPTION  :  Reads the next (non-empty)token and converts it to a boolean.
//					If the next token is not a valid integer 0 or 1, an exception
//					is thrown.
//					Note: This method can be easily modified to look for "t"/"nil" or
//					"true"/"false" or whatever.
// PARAMETER    :  None.
// RETURN VALUE	: (bool) - The next token converted to an boolean.
//--------------------------------------------------------------------
	bool get_next_token_bool();

//--------------------------------------------------------------------
// METHOD       :  get_line_number()
// DESCRIPTION  :  Returns the current line number for the file pointer.
//              :  This is done by counting how many EOL characters it
//              :  has encountered since the beginning of the file.
// PARAMETER    :  None.
// RETURN VALUE :  int lineNumber = the number of EOL encountered so far.
//--------------------------------------------------------------------
	int    get_line_number();

//--------------------------------------------------------------------
// METHOD       :  put_back()
// DESCRIPTION  :  Put back the string.
// PARAMETER    :  string str = the string.
// RETURN VALUE :  None.
//--------------------------------------------------------------------
	void   put_back( const std::string& _str);

// CR-add check for multiple m_delimiters of same char
//--------------------------------------------------------------------
// METHOD       :  add_delimiters()
// DESCRIPTION  :  Add more characters to be used as m_delimiters.
// PARAMETER    :  string delimiter = a string of characters where
//              :               each character is a delimiter.
// RETURN VALUE :  string m_delimiters = current string of m_delimiters.
//--------------------------------------------------------------------
	std::string add_delimiters( const std::string& _delimiters);

//--------------------------------------------------------------------
// METHOD       :  del_delimiters()
// DESCRIPTION  :  Remove characters to be used as m_delimiters.
// PARAMETER    :  string delimiter = a string of characters where
//              :               each character is a delimiter.
// RETURN VALUE :  string m_delimiters = current string of m_delimiters.
//--------------------------------------------------------------------
	std::string del_delimiters( const std::string& _delimiters);

// CV - check if newDelimiters is empty
//--------------------------------------------------------------------
// METHOD       :  set_delimiters()
// DESCRIPTION  :  Set characters to be use as m_delimiters.
// PARAMETER    :  string delimiter = a string of characters where
//              :               each character is a delimiter.
// RETURN VALUE :  string m_delimiters = current string of m_delimiters.
//--------------------------------------------------------------------
	std::string set_delimiters( const std::string& _delimiters);

//CV  - check for multiple of same char
//--------------------------------------------------------------------
// METHOD       :  add_token_delimiters()
// DESCRIPTION  :  Add more characters to be use as m_delimiters and tokens.
// PARAMETER    :  string delimiter = a string of characters where
//              :               each character is a delimiter.
// RETURN VALUE :  string m_delimiters = current string of m_delimiters.
//--------------------------------------------------------------------
	std::string add_token_delimiters( const std::string& _delimiters);

//--------------------------------------------------------------------
// METHOD       :  del_token_delimiters()
// DESCRIPTION  :  Remove characters to be use as m_delimiters and tokens,
//					 if any.
// PARAMETER    :  string delimiter = a string of characters where
//              :               each character is a delimiter.
// RETURN VALUE :  string m_delimiters = current string of m_delimiters.
//--------------------------------------------------------------------
	std::string del_token_delimiters( const std::string& _delimiters);

//CV - check if newDelimiters is empty, do not make change
//--------------------------------------------------------------------
// METHOD       :  set_token_delimiters()
// DESCRIPTION  :  Set characters to be use as m_delimiters and also
//					returned as tokens.
// PARAMETER    :  string delimiter = a string of characters where
//              :               each character is a delimiter.
// RETURN VALUE :  string m_delimiters = current string of m_delimiters.
//--------------------------------------------------------------------
	std::string set_token_delimiters( const std::string& _delimiters);

//--------------------------------------------------------------------
// METHOD       :  set_comment()
// DESCRIPTION  :  Set the character to denote the rest fo the line is
//              :  just comments.
// PARAMETER    :  char m_comment = the m_comment character flag.
// RETURN VALUE :  None.
//--------------------------------------------------------------------
	void set_comment(char m_comment, bool _StopOnCommentsOnly = false);

	bool token_equal_char( const std::string& _token, const std::string& _chars);

	// m_whitespace handling
	void add_whitespace( const std::string& _ws) { if( !_ws.empty()) m_whitespace += _ws; }

	// methods apply to tokens
	inline void clear_all_whitespace( bool clear = true) { m_clear_all = clear;}	// default false
	inline void clear_leading_whitespace( bool clear = true) { m_clear_leading = clear;}// default true
	inline void clear_trailing_whitespace( bool clear = true) { m_clear_trailing = clear;}// default true
	inline bool is_clear_all_whitespace() { return m_clear_all; }
	inline bool is_clear_leading_whitespace() { return m_clear_leading; }
	inline bool is_clear_trailing_whitespace() { return m_clear_trailing; }

//--------------------------------------------------------------------
// METHOD       :  end_of_file()
// DESCRIPTION  :  Tests for end of file.
// PARAMETER    :  None.
// RETURN VALUE :  bool eof ? true:false.
//--------------------------------------------------------------------
	bool end_of_file();

//--------------------------------------------------------------------
// METHOD       :  was_comment()
// DESCRIPTION  :  Check to see if the last get_next_token() read a m_comment.
//              :  If so, and the return string is empty, then is not a valid
//              :  read.
// PARAMETER    :  None.
// RETURN VALUE :  bool eof ? true:false.
//--------------------------------------------------------------------
	bool was_comment();

	inline const std::string& get_comment_string() const { return m_comment_string; }

	inline void redefine_parser( char _comment, std::string _whitespace, std::string _delimiters, std::string _token_delimiters)
	{
		m_comment = _comment;
		m_whitespace = _whitespace;
		m_delimiters = _delimiters;
		token_delimiters = _token_delimiters;
	}

protected:
    // Members.
	char m_comment;
	std::string m_comment_string;
	bool m_stop_on_comments_only;
//    int whiteSpaceMin;
//    int whiteSpaceMax;
    int m_current_line_number;

	// delimeters that separate tokens
	std::string m_delimiters;

	// delimeters that separate tokens and are also returned as
	// individual tokens
	std::string token_delimiters;

	std::stringstream m_sstream;

	bool m_was_comment;

	bool m_clear_all;
	bool m_clear_leading;
	bool m_clear_trailing;
	std::string m_whitespace;
};

} // end namespace gs:reg_utils
} // end namespace gs:reg_utils

  
#endif // !defined(_FILESTREAMTOKENIZER_H_

#endif // EXCLUDE_FROM_BUILD
#undef EXCLUDE_FROM_BUILD
