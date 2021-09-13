#include <base/hook/inline.h>

#if !defined(DISABLE_DETOURS)

#include <base/hook/detail/replace_pointer.h>
#include <Windows.h>
#include <base/detours/detours.h>

namespace base { 
namespace hook {
	
	bool inline_install(uintptr_t* pointer_ptr, uintptr_t detour)
	{
		LONG status;
		if ((status = DetourTransactionBegin()) == NO_ERROR)
		{
			if ((status = DetourUpdateThread(::GetCurrentThread())) == NO_ERROR)
			{
				if ((status = DetourAttach((PVOID*)pointer_ptr, (PVOID)detour)) == NO_ERROR)
				{
					if ((status = DetourTransactionCommit()) == NO_ERROR)
					{
						return true;
					}
				}
			}
			DetourTransactionAbort();
		}
		::SetLastError(status);
		return false;
	}

	bool inline_uninstall(uintptr_t* pointer_ptr, uintptr_t detour)
	{
		LONG status;
		if ((status = DetourTransactionBegin()) == NO_ERROR)
		{
			if ((status = DetourUpdateThread(::GetCurrentThread())) == NO_ERROR)
			{
				if ((status = DetourDetach((PVOID*)pointer_ptr, (PVOID)detour)) == NO_ERROR)
				{
					if ((status = DetourTransactionCommit()) == NO_ERROR)
					{
						return true;
					}
				}
			}
			DetourTransactionAbort();
		}
		::SetLastError(status);
		return false;
	}
}
}

#endif
