

#ifndef SYSTEM_H
#define SYSTEM_H

#include "Platforms.h"

// common standard library headers
#include <cassert>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>

// template classes
#include "TArray.h"
#include "THashSet.h"
#include "THashTable.h"
#include "TList.h"
#include "TSet.h"
#include "TStack.h"



class JDCENGINE System
{
public:
    // swap byte order (size = 2, 4, or 8)
    static void SwapBytes (int iSize, void* pvValue);
    static void SwapBytes (int iSize, int iQuantity, void* pvValue);

    // The output is stored in little endian format.
    static void EndianCopy (int iSize, const void* pvSrc, void* pvDst);
    static void EndianCopy (int iSize, int iQuantity, const void* pvSrc,
        void* pvDst);

    // time utility (return value is seconds)
    static double GetTime ();

    // File support for buffer load and save.
    static bool Load (const char* acFilename, char*& racBuffer, int& riSize);
    static bool Save (const char* acFilename, const char* acBuffer,
        int iSize);
    static bool Append (const char* acFilename, char* acBuffer, int iSize);

    // Buffer read and write; hides endianness.  The return value is the
    // number of bytes read or written.  The function name suffix indicates
    // the number of bytes transferred per item.
    static int Read1 (const char* acBuffer, int iQuantity, void* pvData);
    static int Read2 (const char* acBuffer, int iQuantity, void* pvData);
    static int Read4 (const char* acBuffer, int iQuantity, void* pvData);
    static int Read8 (const char* acBuffer, int iQuantity, void* pvData);
    static int Write1 (char* acBuffer, int iQuantity, const void* pvData);
    static int Write2 (char* acBuffer, int iQuantity, const void* pvData);
    static int Write4 (char* acBuffer, int iQuantity, const void* pvData);
    static int Write8 (char* acBuffer, int iQuantity, const void* pvData);

    // File read and write; hides endianness.  The return value is the
    // number of bytes read or written.  The function name suffix indicates
    // the number of bytes transferred per item.
    static int Read1 (FILE* pkFile, int iQuantity, void* pvData);
    static int Read2 (FILE* pkFile, int iQuantity, void* pvData);
    static int Read4 (FILE* pkFile, int iQuantity, void* pvData);
    static int Read8 (FILE* pkFile, int iQuantity, void* pvData);
    static int Write1 (FILE* pkFile, int iQuantity, const void* pvData);
    static int Write2 (FILE* pkFile, int iQuantity, const void* pvData);
    static int Write4 (FILE* pkFile, int iQuantity, const void* pvData);
    static int Write8 (FILE* pkFile, int iQuantity, const void* pvData);

    // This is needed on the Macintosh because of its complicated application
    // structure.  The returned value is just the input, but has the side
    // effect of changing directory to the working directory of the
    // application.  The relative paths used for the data sets are then
    // resolved correctly.
    static const char* GetPath (const char* acFilename);

    // Creation of colors, hides endianness.
    static unsigned int MakeRGB (unsigned char ucR, unsigned char ucG,
        unsigned char ucB);
    static unsigned int MakeRGBA (unsigned char ucR, unsigned char ucG,
        unsigned char ucB, unsigned char ucA);

    // Allocation and deallocation of 2D arrays.  On deallocation, the array
    // pointer is set to null.
    template <class T> static void Allocate (int iCols, int iRows,
        T**& raatArray)
	{
		raatArray = new T*[iRows];
		raatArray[0] = new T[iRows*iCols];
		for (int iRow = 1; iRow < iRows; iRow++)
		{
			raatArray[iRow] = &raatArray[0][iCols*iRow];
		}
	}
	
    template <class T> static void Deallocate (T**& raatArray)
	{
		delete[] raatArray[0];
		delete[] raatArray;
		raatArray = 0;
	}

    // Allocation and deallocation of 3D arrays.  On deallocation, the array
    // pointer is set to null.
    template <class T> static void Allocate (int iCols, int iRows,
        int iSlices, T***& raaatArray)
	{
		raaatArray = new T**[iSlices];
		raaatArray[0] = new T*[iSlices*iRows];
		raaatArray[0][0] = new T[iSlices*iRows*iCols];
		for (int iSlice = 0; iSlice < iSlices; iSlice++)
		{
			raaatArray[iSlice] = &raaatArray[0][iRows*iSlice];
			for (int iRow = 0; iRow < iRows; iRow++)
			{
				raaatArray[iSlice][iRow] =
					&raaatArray[0][0][iCols*(iRow+iRows*iSlice)];
			}
		}
	}
    template <class T> static void Deallocate (T***& raaatArray)
	{
		delete[] raaatArray[0][0];
		delete[] raaatArray[0];
		delete[] raaatArray;
		raaatArray = 0;
	}
};

#include "JdcSystem.inl"



#endif
