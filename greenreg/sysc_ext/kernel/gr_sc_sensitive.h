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

//
// ChangeLog GreenSocs
//
// 2008-12-03 Christian Schroeder: format
// 2009-01-29 Christian Schroeder: renamings
// 2009-01-29 FURTHER CHANGES SEE SVN LOG!
//


#ifndef _GR_SC_SENSITIVE_H_
#define _GR_SC_SENSITIVE_H_

namespace gs {
namespace reg {

class gr_module;
class gr_event;

class gr_sc_sensitive
{
public:
  gr_sc_sensitive( gr_module * _gr_module);
  virtual ~gr_sc_sensitive();
	
  gr_sc_sensitive& operator () ( gr_event& ); // m_gr_module->sensitive << bla bla bla
  gr_sc_sensitive& operator << ( gr_event& ); // gr_event->add_static( m_gr_module->current_callback);
    
protected:
  gr_module * m_gr_module;
};

} // end namespace gs::reg
} // end namespace gs::reg

#endif /*_GR_SC_SENSITIVE_H_*/
