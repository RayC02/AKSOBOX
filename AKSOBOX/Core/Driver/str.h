#ifndef _STR_H_
#define _STR_H_
int GbkToUtf_8(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);
int Utf_8ToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);
#endif

