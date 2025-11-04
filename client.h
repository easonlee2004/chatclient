#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <cstdio>
#include <cstring>

// 发送消息
unsigned sendMsg(void* arg);
// 接收消息
unsigned recvMsg(void* arg);

#endif
