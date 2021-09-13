// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "windows.h" 
#include <helper.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    Helper& helper = get_helper();
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        helper.attach();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        helper.detach();
        break;
    }
    return TRUE;
}

