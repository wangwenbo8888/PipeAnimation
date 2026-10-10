#include "stdafx.h"
#include "Reg_GetHDSerial.h"

#if 0

bool CheckSerial(const std::string serial)
{
    // µ√µΩ”≤≈Ã–Ú¡–∫≈
    CGetHDSerial HDSerial;  
    char* temp=HDSerial.GetHDSerial();    
	std::string ser(temp);

	MD5 algo;
	return serial == algo.encode(ser);
}
#endif