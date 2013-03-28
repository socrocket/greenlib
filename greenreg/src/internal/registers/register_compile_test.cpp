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


#include "greenreg/internal/registers/register_compile_test.h"

#include "greenreg/utils/storage/addressable_container_ref.h"

#include "greenreg/internal/registers/bit/bit.h"
#include "greenreg/internal/registers/bit/bit_data.h"
#include "greenreg/internal/registers/bit/bit_accessor.h"

#include "greenreg/internal/registers/register/I_register.h"
#include "greenreg/framework/registers/gr_register_container.h"
#include "greenreg/framework/registers/gr_register_container_reference_container.h"

#include "greenreg/framework/devices/gr_device.h"

#include "greenreg/internal/registers/register_data/I_register_data.h"
#include "greenreg/internal/registers/register_data/standard_register_data.h"
#include "greenreg/internal/registers/register_data/primary_register_data.h"

namespace gs {
namespace reg {

register_compile_test::register_compile_test()
{
	gr_device d( "register_compile_test_dummy", ALIGNED_ADDRESS, 0, NULL);
	gr_register_container r( "registers", ALIGNED_ADDRESS, 1, d);
	r.create_register( "test", "test description",
		0x300, 0x12345678, 0xFFFFFFFF,
		STANDARD_REG | SPLIT_IO | SINGLE_BUFFER | FULL_WIDTH, 32, 0x0);

	gs::reg_utils::addressable_container_ref<  uint_gr_t, I_register> r2;
	r2 = r;
}

register_compile_test::~register_compile_test()
{
}

} // end namespace gs:reg
} // end namespace gs:reg
