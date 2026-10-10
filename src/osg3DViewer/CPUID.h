//#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <intrin.h>	// 所有Intrinsics函数

using namespace std;
char szBuf[64];

// 取得CPU厂商（Vendor）
//
// result: 成功时返回字符串的长度（一般为12）。失败时返回0。
// pvendor: 接收厂商信息的字符串缓冲区。至少为13字节。
void __cpuid(
	int CPUInfo[4],
	int InfoType
	);

void __cpuidex(
	int CPUInfo[4],
	int InfoType,
	int ECXValue
	);

int cpu_getvendor(char* pvendor)
{
	INT32 dwBuf[4];
	if (NULL == pvendor)	return 0;
	// Function 0: Vendor-ID and Largest Standard Function
	__cpuid(dwBuf, 0);
	// save. 保存到pvendor
	*(INT32*)&pvendor[0] = dwBuf[1];	// ebx: 前四个字符
	*(INT32*)&pvendor[4] = dwBuf[3];	// edx: 中间四个字符
	*(INT32*)&pvendor[8] = dwBuf[2];	// ecx: 最后四个字符
	pvendor[12] = '\0';
	return 12;
}

// 取得CPU商标（Brand）
//
// result: 成功时返回字符串的长度（一般为48）。失败时返回0。
// pbrand: 接收商标信息的字符串缓冲区。至少为49字节。
int cpu_getbrand(char* pbrand)
{
	INT32 dwBuf[4];
	if (NULL == pbrand)	return 0;
	// Function 0x80000000: Largest Extended Function Number
	__cpuid(dwBuf, 0x80000000);
	if (dwBuf[0] < 0x80000004)	return 0;
	// Function 80000002h,80000003h,80000004h: Processor Brand String
	__cpuid((INT32*)&pbrand[0], 0x80000002);	// 前16个字符
	__cpuid((INT32*)&pbrand[16], 0x80000003);	// 中间16个字符
	__cpuid((INT32*)&pbrand[32], 0x80000004);	// 最后16个字符
	pbrand[48] = '\0';
	return 48;
}

string GetCPUID() {
	INT32 dwBuf[4];
	std::string strCPUId;
	char buf[32] = { 0 };
	__cpuidex(dwBuf, 1, 1);
	//printf("%08X%08X\n", dwBuf[3], dwBuf[0]);
	memset(buf, 0, 32);
	sprintf_s(buf, 32, "%08X", dwBuf[3]);
	strCPUId += buf;
	memset(buf, 0, 32);
	sprintf_s(buf, 32, "%08X", dwBuf[0]);
	strCPUId += buf;
	return strCPUId;
}

//int main(int argc, _TCHAR* argv[])
//{
//	cpu_getvendor(szBuf);
//	printf("CPU Vendor:\t%s\n", szBuf);
//
//	cpu_getbrand(szBuf);
//	printf("CPU Name:\t%s\n", szBuf);
//
//	cout << "CPU ID:		" << GetCPUID() <<endl;
//	return 0;
//}
