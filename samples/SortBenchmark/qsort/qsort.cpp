#pragma unmanaged

// Defines the entry point for the console application.

#include "stdafx.h"
#include <stdlib.h>     
#include <string.h>
#include <algorithm>
//#include <cstdlib>
#include "sort.h"

#include <vector>
#include <math.h>

static int d = 100;
static int hashBytes = 2;

int compare(void *context, const void * a, const void * b)
{
	return memcmp(a, b, d);
}
bool compareb(void *context, const void * a, const void * b)
{
	return memcmp(a, b, d) < 0;
}
bool comparec(const void * a, const void * b)
{
    return memcmp(a, b, d) < 0;
}

#define swapcode(TYPE, parmi, parmj, n) { 		\
	long i = (n) / sizeof (TYPE); 			\
	TYPE *pi = (TYPE *) (parmi); 			\
	TYPE *pj = (TYPE *) (parmj); 			\
	do { 						\
		TYPE	t = *pi;			\
		*pi++ = *pj;				\
		*pj++ = t;				\
		        } while (--i > 0);				\
}

extern "C" __declspec(dllexport)
void stdquicksort(void* buf, int len, int dim)
{
	dim = d;
	qsort_s(buf, len, dim, compare, 0);
}


int comparefirst2(const void * a, const void * b)
{
	return memcmp(a, b, 2);
}

#define  getHash(data)               \
	(((int ((unsigned char)*(data))) << 8) + ((unsigned char)*((data)+1))  )


int hash(char * data)
{
	int ret = ((unsigned char)*data);
	for (int i = 1; i < hashBytes; i++)
	{
		ret = (ret << 8) + (unsigned char)*(data + i);
	}
	return ret;
}

static int * boundaryArr;

int GetIndex(int hashVal)
{
	return boundaryArr[hashVal];
	//return hashVal % 1024;
}



#define HASH(data) (((int ((unsigned char)*(data))) << 8) + ((unsigned char)*((data) + 1)))


extern "C" __declspec(dllexport)
void bin(char * buf, int len, int dim, int binNum, int * boundary, int * sPos, char * outBuf, size_t outBufSize)
{
	//int hashBitSize = fmax(8, (int)(log2((float)(binNum - 1)) + 1));
	//hashBytes = (hashBitSize - 1) / 8 + 1;
	hashBytes = 2;
	boundaryArr = boundary;


	for (int i = 0; i < binNum; i++)
	{
		sPos[i] = 0;
	}
	for (int i = 0; i < len; i++)
	{
		int hashval = hash(buf + i*dim);
		int index = GetIndex(hashval);
		sPos[index] += dim;

	}

	int sum = 0;
	for (int i = 0; i < binNum; i++)
	{
		int t = sum;
		sum += sPos[i];
		sPos[i] = t;
	}


	//0.84 seconds
	//char * ttbuf = new char[len*dim];
	for (int i = 0; i < len; i++)
	{
		int index = GetIndex(hash(buf + i * dim));
		memcpy_s(outBuf + sPos[index], outBufSize - sPos[index], buf + i * dim, dim);
		sPos[index] += dim;
	}
	

	//1.0 second
	/*
	int counter = 0;
	int i = 0;
	while (i < len)
	{
		int index = GetIndex(hash(buf + i * dim));
		if (sPos[index] == i*dim)
		{
			i++;
			sPos[index] += dim;
		}
		else if (sPos[index] >= sPos[index+binNum])
		{
			i++;
		}
		else if (sPos[index] < len * dim)
		{
			while (sPos[index] < len*dim && GetIndex(hash(buf + sPos[index])) == index)
			{
				sPos[index] += dim;
			}
			memcpy_s(tBuf, len*dim, buf + sPos[index], dim);
			memcpy_s(buf + sPos[index], len*dim - sPos[index], buf + i * dim, dim);
			memcpy_s(buf + i * dim, len*dim - i*dim, tBuf, dim);

			sPos[index] += dim;
			while (sPos[index] < len*dim && GetIndex(hash(buf + sPos[index])) == index)
			{
				sPos[index] += dim;
			}
		}
	}

	*/
	//for (int i = 0; i < len; i++)
	//{
	//	int index = boundary[hash(buf + i*dim, hashByteSize)];
	//	memcpy_s(tbuf + sPos[index], outBufSize - sPos[index], buf + i*dim, dim);
	//	sPos[index] += dim;
	//}
	//memcpy_s(buf, len*dim, tbuf, len*dim);

//	delete tbuf;



}






extern "C" __declspec(dllexport)
void getBinSize(char * buf, int len, int dim, int binNum, int * boundary, int * binSize)
{
	for (int i = 0; i < binNum; i++)
	{
		binSize[i] = 0;
	}
	for (int i = 0; i < len; i++)
	{
		int index = HASH(buf + i*dim);// % binNum;
		binSize[boundary[index]] += dim;
	}
}




extern "C" __declspec(dllexport)
void stdsort(void* buf, int len, int dim)
{
	//char ** pointer = new char *[len];

	std::vector<char*> pointer(len);

	char * p = (char *)buf;
	for (int i = 0; i < len; i++)
	{
		pointer[i] = p;
		p = p + dim;
	}

	std::sort(pointer.begin(), pointer.end(), comparec);

	char* tbuf = new char[len*dim];

	for (int i = 0; i < len; i++)
	{
		//swapcode(char, pointer + i, tbuf + i*dim,dim);
		memcpy_s(tbuf + i*dim, len*dim-i*dim, pointer[i], dim);
	}

	for (int i = 0; i < len; i++)
	{
		//swapcode(char, (char*)buf + (i*dim), tbuf + i*dim, dim);
		memcpy_s((char*)buf + (i*dim), len*dim-i*dim, tbuf + i*dim, dim);
	}

	//delete pointer;
	delete tbuf;
}



extern "C" __declspec(dllexport)
void MyMemoryCopy(char* srcBuf, int srcOff, char* dest, int destOff, int dstSize, int size)
{
	memcpy_s(dest + destOff, dstSize, srcBuf + srcOff, size);
}

#pragma unmanaged
int __cdecl compare64(void *context, const void *a, const void *b)
{
    int i;
    int dim = *(int*)context;
    unsigned __int64 *a64 = (unsigned __int64*)a;
    unsigned __int64 *b64 = (unsigned __int64*)b;

#if 0
    if (a64[0] > b64[0]) return 1;
    if (a64[0] < b64[0]) return -1;
    for (i=1; i < dim; i++)
    {
        if (a64[i] > b64[i]) return 1;
        if (a64[i] < b64[i]) return -1;
    }
#else
    if (_byteswap_uint64(a64[0]) > _byteswap_uint64(b64[0])) return 1;
    if (_byteswap_uint64(a64[0]) < _byteswap_uint64(b64[0])) return -1;
    for (i = 1; i < dim; i++)
    {
        if (_byteswap_uint64(a64[i]) > _byteswap_uint64(b64[i])) return 1;
        if (_byteswap_uint64(a64[i]) < _byteswap_uint64(b64[i])) return -1;
    }
    
#endif
    return 0;
}

// "buf" has "num" alignment units of size "align"
#pragma unmanaged
extern "C" __declspec(dllexport)
void __stdcall alignsort64(unsigned __int64 *buf, int align, int num)
{
    qsort_s(buf, num, align*8, compare64, &align);
}
