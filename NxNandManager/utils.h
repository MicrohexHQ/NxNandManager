#pragma once
#ifndef __utils_h__
#define __utils_h__

#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <windows.h>
#include <winioctl.h>
#include "types.h"
#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <wchar.h>
#include <algorithm>
#include <sstream>
#include <tchar.h>
#include "DiskSector.h"
#include "partitionmanager.h"


typedef std::chrono::duration< double > double_prec_seconds;
typedef std::chrono::time_point< std::chrono::system_clock, double_prec_seconds > timepoint_t;

// MinGW
#if defined(__MINGW32__) || defined(__MINGW64__) || defined(__MSYS__)
#define strcpy_s strcpy
#define sprintf_s snprintf
#endif

//CRYPTO
#define ENCRYPT 1
#define DECRYPT 2

// ERRORS
#define ERR_WRONG_USE			   -1001
#define ERR_INVALID_INPUT		   -1002
#define ERR_INVALID_OUTPUT		   -1003
#define ERR_INVALID_PART		   -1004
#define ERR_IO_MISMATCH			   -1005
#define ERR_INPUT_HANDLE		   -1006
#define ERR_OUTPUT_HANDLE		   -1007
#define ERR_CRYPTO_MD5			   -1008
#define ERR_NO_LIST_DISK		   -1009
#define ERR_NO_SPACE_LEFT		   -1010
#define ERR_COPY_SIZE			   -1011
#define ERR_MD5_COMPARE			   -1012
#define ERR_INIT_GUI			   -1013
#define ERR_WORK_RUNNING		   -1014
#define ERR_WHILE_COPY			   -1015
#define NO_MORE_BYTES_TO_COPY      -1016
#define ERR_RESTORE_TO_SPLIT	   -1017
#define ERR_DECRYPT_CONTENT		   -1018
#define ERR_RESTORE_CRYPTO_MISSING -1019
#define ERR_CRYPTO_KEY_MISSING	   -1020
#define ERR_CRYPTO_GENERIC  	   -1021
#define ERR_CRYPTO_NOT_ENCRYPTED   -1022
#define ERR_CRYPTO_ENCRYPTED_YET   -1023
#define ERR_CRYPTO_DECRYPTED_YET   -1024
#define ERR_RESTORE_CRYPTO_MISSIN2 -1025
#define ERROR_DECRYPT_FAILED	   -1026

typedef struct ErrorLabel ErrorLabel;
struct ErrorLabel {
	int error;
	const char* label;
};

static ErrorLabel ErrorLabelArr[] =
{
	{ ERR_WORK_RUNNING, "Work already in process"},
	{ ERR_INPUT_HANDLE, "Failed to get handle to input file/disk"},
	{ ERR_OUTPUT_HANDLE, "Failed to get handle to output file/disk"},
	{ ERR_NO_SPACE_LEFT, "Output disk : not enough space !"},
	{ ERR_CRYPTO_MD5, "Crypto provider error"},
	{ ERR_MD5_COMPARE, "Data integrity error : checksums are differents.\nAn error must have occurred during the copy"},
	{ ERR_RESTORE_TO_SPLIT, "Restore to splitted dump is not supported"},
	{ ERR_WHILE_COPY, "An error occured during copy"},
	{ ERR_IO_MISMATCH, "Input type/size doesn't match output size/type"},
	{ ERR_INVALID_INPUT, "Input is not a valid NX storage"},
	{ ERR_DECRYPT_CONTENT, "Failed to validate decrypted content (wrong keys ?)"},
    { ERR_RESTORE_CRYPTO_MISSING, "Trying to restore decrypted input to encrypted output and keyset missing to encrypt data"},
	{ ERR_RESTORE_CRYPTO_MISSIN2, "Trying to restore encrypted input to decrypted output (decrypt content to restore first)"},
    { ERR_CRYPTO_KEY_MISSING, "Trying to decrypt/encrypt content but some keys are missing (configure keyset)"},
	{ ERROR_DECRYPT_FAILED, "Decryption validation failed (wrong keys ?)"},
	{ ERR_CRYPTO_NOT_ENCRYPTED, "Input file is not encrypted"},
	{ ERR_CRYPTO_ENCRYPTED_YET, "Input file is already encrypted"},
	{ ERR_CRYPTO_DECRYPTED_YET, "Input file is already decrypted"}
};

typedef struct KeySet KeySet;
struct KeySet {
	char crypt0[33];
	char tweak0[33];
	char crypt1[33];
	char tweak1[33];
	char crypt2[33];
	char tweak2[33];
	char crypt3[33];
	char tweak3[33];
};


wchar_t *convertCharArrayToLPCWSTR(const char* charArray);
LPWSTR convertCharArrayToLPWSTR(const char* charArray);
u64 GetFilePointerEx (HANDLE hFile);
unsigned long sGetFileSize(std::string filename);
std::string GetLastErrorAsString();
std::string hexStr(unsigned char *data, int len);
BOOL AskYesNoQuestion(const char* question);
std::string GetReadableSize(u64 size);
std::string GetReadableElapsedTime(std::chrono::duration<double> elapsed_seconds);
void throwException(int rc, const char* errorStr=NULL);
void throwException(const char* errorStr=NULL);
std::string ListPhysicalDrives(BOOL noError=FALSE);
char * flipAndCodeBytes(const char * str, int pos, int flip, char * buf);
std::string ExePath();
HMODULE GetCurrentModule();
bool is_file_exist(const wchar_t *fileName);
int digit_to_int(char d);

template<class T>
T base_name(T const & path, T const & delims = "/\\")
{
	return path.substr(path.find_last_of(delims) + 1);
}
template<class T>
T remove_extension(T const & filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}
template<class T>
T get_extension(T const & filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(p, T::npos) : filename;
}

template< typename T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') //<< std::setw(sizeof(T) * 2)
		<< std::uppercase
		<< std::hex << i;

	return stream.str();
}

std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);
int parseKeySetFile(const char *keyset_file, KeySet *biskeys);
#endif
