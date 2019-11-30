#include <stdio.h>
#include <stdlib.h>

const static char* b64 =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// maps A=>0,B=>1..
const static unsigned char unb64[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //10
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //20
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //30
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //40
		0, 0, 0, 62, 0, 0, 0, 63, 52, 53, //50
		54, 55, 56, 57, 58, 59, 60, 61, 0, 0, //60
		0, 0, 0, 0, 0, 0, 1, 2, 3, 4, //70
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, //80
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, //90
		25, 0, 0, 0, 0, 0, 0, 26, 27, 28, //100
		29, 30, 31, 32, 33, 34, 35, 36, 37, 38, //110
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, //120
		49, 50, 51, 0, 0, 0, 0, 0, 0, 0, //130
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //140
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //150
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //170
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //180
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //190
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //200
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //210
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //220
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //230
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //250
		0, 0, 0, 0, 0, 0, }; // This array has 255 elements

//review api
char* base64(const void* binaryData, int len, int *flen) {
	const unsigned char* bin = (const unsigned char*) binaryData;
	char* res;

	int rc = 0; // result counter
	int byteNo; // I need this after the loop

	int modulusLen = len % 3;
	int pad = ((modulusLen & 1) << 1) + ((modulusLen & 2) >> 1); // 2 gives 1 and 1 gives 2, but 0 gives 0.

	*flen = 4 * (len + pad) / 3;
	res = (char*) malloc(*flen + 1); // and one for the null
	if (!res) {
		puts("ERROR: base64 could not allocate enough memory.");
		puts("I must stop because I could not get enough");
		return 0;
	}

	for (byteNo = 0; byteNo <= len - 3; byteNo += 3) {
		unsigned char BYTE0 = bin[byteNo];
		unsigned char BYTE1 = bin[byteNo + 1];
		unsigned char BYTE2 = bin[byteNo + 2];
		res[rc++] = b64[BYTE0 >> 2];
		res[rc++] = b64[((0x3 & BYTE0) << 4) + (BYTE1 >> 4)];
		res[rc++] = b64[((0x0f & BYTE1) << 2) + (BYTE2 >> 6)];
		res[rc++] = b64[0x3f & BYTE2];
	}

	if (pad == 2) {
		res[rc++] = b64[bin[byteNo] >> 2];
		res[rc++] = b64[(0x3 & bin[byteNo]) << 4];
		res[rc++] = '=';
		res[rc++] = '=';
	} else if (pad == 1) {
		res[rc++] = b64[bin[byteNo] >> 2];
		res[rc++] = b64[((0x3 & bin[byteNo]) << 4) + (bin[byteNo + 1] >> 4)];
		res[rc++] = b64[(0x0f & bin[byteNo + 1]) << 2];
		res[rc++] = '=';
	}

	res[rc] = 0; // NULL TERMINATOR! ;)
	return res;
}

//FIXME! 
unsigned char* unbase64(const char* ascii, int len, int *flen) {
	const unsigned char *safeAsciiPtr = (const unsigned char*) ascii;
	unsigned char *bin;
	int cb = 0;
	int charNo;
	int pad = 0;

	if (len < 2) { // 2 accesses below would be OOB.
		// catch empty string, return NULL as result.
		puts(
				"ERROR: You passed an invalid base64 string (too short). You get NULL back.");
		*flen = 0;
		return 0;
	}
	if (safeAsciiPtr[len - 1] == '=')
		++pad;
	if (safeAsciiPtr[len - 2] == '=')
		++pad;

	*flen = 3 * len / 4 - pad;
	bin = (unsigned char*) malloc(*flen);
	if (!bin) {
		puts("ERROR: unbase64 could not allocate enough memory.");
		puts("I must stop because I could not get enough");
		return 0;
	}

	for (charNo = 0; charNo <= len - 4 - pad; charNo += 4) {
		int A = unb64[safeAsciiPtr[charNo]];
		int B = unb64[safeAsciiPtr[charNo + 1]];
		int C = unb64[safeAsciiPtr[charNo + 2]];
		int D = unb64[safeAsciiPtr[charNo + 3]];

		bin[cb++] = (A << 2) | (B >> 4);
		bin[cb++] = (B << 4) | (C >> 2);
		bin[cb++] = (C << 6) | (D);
	}

	if (pad == 1) {
		int A = unb64[safeAsciiPtr[charNo]];
		int B = unb64[safeAsciiPtr[charNo + 1]];
		int C = unb64[safeAsciiPtr[charNo + 2]];

		bin[cb++] = (A << 2) | (B >> 4);
		bin[cb++] = (B << 4) | (C >> 2);
	} else if (pad == 2) {
		int A = unb64[safeAsciiPtr[charNo]];
		int B = unb64[safeAsciiPtr[charNo + 1]];

		bin[cb++] = (A << 2) | (B >> 4);
	}

	return bin;
}
