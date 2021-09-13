#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { namespace hook { 

	bool inline_install(uintptr_t* pointer_ptr, uintptr_t detour);
	bool inline_uninstall(uintptr_t* pointer_ptr, uintptr_t detour);

}}
