#pragma once
#include <windows.h>
#include <Wincrypt.h>
#include <iostream>
#include <fstream> 
#include "utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cassert>
#include "hex_string.h"
#include "xts_crypto.h"
#include "Shlwapi.h"
#include <string>
#include <handleapi.h>
#include <openssl/sha.h>

using namespace std;

#define BUFSIZE   0x4000
#define MD5LEN	16
#define INVALID   1000
#define BOOT0	 1001
#define BOOT1	 1002
#define RAWNAND	  1003
#define PARTITION 1005
#define RAWMMC   1006
#define TXNAND   1007
#define PRODINFO 1008
#define PRODINFOF 1009
#define UNKNOWN   1004
#define NX_GPT_FIRST_LBA 1
#define NX_GPT_NUM_BLOCKS 33
#define NX_EMMC_BLOCKSIZE 512
#define GPT_PART_NAME_LEN 36
#define DEFAULT_BUFF_SIZE 0x4000
#define CLUSTER_SIZE 0x4000
#define CRYPTO_NONE 0
#define CRYPTO_ENCRYPT 1
#define CRYPTO_DECRYPT 2

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long int u64;
typedef char s8;

// GUID Partition Table structures
typedef struct _GptHeader
{
	u64 signature;
	u32 revision;
	u32 size;
	u32 crc32;
	u32 res1;
	u64 my_lba;
	u64 alt_lba;
	u64 first_use_lba;
	u64 last_use_lba;
	u8 disk_guid[0x10];
	u64 part_ent_lba;
	u32 num_part_ents;
	u32 part_ent_size;
	u32 part_ents_crc32;
	u8 res2[420];
} GptHeader;

typedef struct _GptEntry
{
	u8 type_guid[0x10];
	u8 part_guid[0x10];
	u64 lba_start;
	u64 lba_end;
	u64 attrs;
	u16 name[36];
} GptEntry;

typedef struct GptPartition GptPartition;
struct GptPartition {
	u32 lba_start;
	u32 lba_end;
	u64 attrs;
	s8 name[37];
	bool isEncrypted = false;
	bool bad_crypto = false;
	GptPartition *next;
};

// FAT32 structures
struct fs_attr {
	unsigned short bytes_per_sector;
	unsigned char sectors_per_cluster;
	unsigned char num_fats;
	unsigned short reserved_sector_count;
	unsigned int fat_size;
	char label[11];
};

typedef struct fat32_entry { 
	char filename[11];
	unsigned char attributes;
	unsigned char reserved;
	unsigned char creation_ms;
	unsigned short creation_time;
	unsigned short creation_date;
	unsigned short last_access_time;
	unsigned short cluster_hi;
	unsigned short modified_time;
	unsigned short modified_date;
	unsigned short first_cluster;
	unsigned int file_size;
} fat32_entry __attribute__((__packed__));

typedef struct fat32_LFNentry {
	BYTE sequenceNo;            
	BYTE fileName_Part1[10];    
	BYTE fileattribute;         
	BYTE reserved_1;
	BYTE checksum;              
	BYTE fileName_Part2[12];    
	BYTE FstClusLO[2];
	BYTE fileName_Part3[4];
}LFN;

typedef struct fat32_dir_entry fat32_dir_entry;
struct fat32_dir_entry {
	std::string filename;
	bool is_directory = FALSE;
	fat32_entry entry;
	fat32_dir_entry *subdir = NULL;
	fat32_dir_entry *next = NULL;
};

typedef struct MagicOffsets MagicOffsets;
struct MagicOffsets {
	u64 offset;
	const char* magic;
	u64 size;
	int type;
	float fw;
};

typedef struct NxPartition NxPartition;
struct NxPartition {
	s8 name[37];
	u64 size;
};

typedef struct NxSplitFile NxSplitFile;
struct NxSplitFile {
	u64 offset;
	u64 size;
	wchar_t file_path[_MAX_PATH];
	NxSplitFile *next = NULL;
};

typedef struct NxHandle NxHandle;
struct NxHandle {
	HANDLE h;
	wchar_t path[_MAX_PATH];
	u64 off_start = 0;
	u64 off_end = 0;
	u64 off_max = 0;
	u64 size = 0;
	u64 readAmount = 0;
	bool encrypt = false;
	bool decrypt = false;
	int block_num;
};

typedef struct NxMMC NxMMC;
struct NxMMC {
	u64 lba_start;
	u64 lba_end;
	u64 boot0_lba_start;
	u64 boot1_lba_start;
	u64 rawnand_lba_start;
};

static MagicOffsets mgkOffArr[] =
{
	// { offset, magic, size, type, firwmare }
	{ 0, "43414C30", 4, PRODINFO}, // PRODINFO ("CAL0" at offset 0x0)
	{ 0x680, "434552544946", 6, PRODINFOF}, // PRODINFOF ("CERTIF at offset 0x680")
	{ 0x200, "4546492050415254", 8, RAWNAND, 0 }, // RAWNAND ("EFI PART" at offset 0x200)	
	{ 0x200, "54584E414E44", 6, TXNAND, 0}, // TX hidden paritition ("TXNAND" at offset 0x200)	
	{ 0x800200, "4546492050415254", 8, RAWMMC, 0}, // RAWMMC ("EFI PART" at offset 0x80000, i.e after 2 x 0x40000 for each BOOT)	
	{ 0x0530, "010021000E00000009000000", 12, BOOT0, 0}, // BOOT0 (boot_data_version + block_size_log2 + page_size_log2 at offset 0x530)
	// BOOT1 => Look for PK11 magic
	{ 0x13B4, "504B3131", 4, BOOT1, 1},
	{ 0x13F0, "504B3131", 4, BOOT1, 2},
	{ 0x1424, "504B3131", 4, BOOT1, 3},
	{ 0x12E8, "504B3131", 4, BOOT1, 4},
	{ 0x12D0, "504B3131", 4, BOOT1, 5},
	{ 0x12F0, "504B3131", 4, BOOT1, 6},
	{ 0x40AF8,"504B3131", 4, BOOT1, 7},
	{ 0x40ADC,"504B3131", 4, BOOT1, 8},
	{ 0x40ACC,"504B3131", 4, BOOT1, 8.1},
	{ 0x40AC0,"504B3131", 4, BOOT1, 9}	
};

static NxPartition partInfoArr[] =
{
	{ "PRODINFO",			    0x003FBC00  },
	{ "CAL0",			        0x003FBC00  },
	{ "PRODINFOF",			    0x00400000  },
	{ "BCPKG2-1-Normal-Main",   0x00800000  },
	{ "BCPKG2-2-Normal-Sub",	0x00800000  },
	{ "BCPKG2-3-SafeMode-Main", 0x00800000  },
	{ "BCPKG2-4-SafeMode-Sub",  0x00800000  },
	{ "BCPKG2-5-Repair-Main",   0x00800000  },
	{ "BCPKG2-6-Repair-Sub",	0x00800000  },
	{ "SAFE",				    0x04000000  },
	{ "SYSTEM",				    0xA0000000  },
	{ "USER",				    0x680000000 }
};

typedef struct NxSytemTitles NxSytemTitles;
struct NxSytemTitles {
	const char fw_version[48];
	const char nca_filename[40];
};


static NxSytemTitles sytemTitlesArr[] = {	
	{ "9.0.0", "a6af05b33f8f903aab90c8b0fcbcc6a4.nca"},
	{ "8.1.0", "7eedb7006ad855ec567114be601b2a9d.nca"},
	{ "8.0.1", "6c5426d27c40288302ad616307867eba.nca"},
	{ "8.0.0", "4fe7b4abcea4a0bcc50975c1a926efcb.nca"}, 
	{ "7.0.1", "e6b22c40bb4fa66a151f1dc8db5a7b5c.nca"},
	{ "7.0.0", "c613bd9660478de69bc8d0e2e7ea9949.nca"},
	{ "6.2.0", "6dfaaf1a3cebda6307aa770d9303d9b6.nca"},
	{ "6.1.0", "1d21680af5a034d626693674faf81b02.nca"},
	{ "6.0.1", "663e74e45ffc86fbbaeb98045feea315.nca"},
	{ "6.0.0", "258c1786b0f6844250f34d9c6f66095b.nca"},
	{ "6.0.0 (pre-release)", "286e30bafd7e4197df6551ad802dd815.nca"},
	{ "5.1.0", "fce3b0ea366f9c95fe6498b69274b0e7.nca"},
	{ "5.0.2", "c5758b0cb8c6512e8967e38842d35016.nca"},
	{ "5.0.1", "7f5529b7a092b77bf093bdf2f9a3bf96.nca"},
	{ "5.0.0", "faa857ad6e82f472863e97f810de036a.nca"},
	{ "4.1.0", "77e1ae7661ad8a718b9b13b70304aeea.nca"},
	{ "4.0.1", "d0e5d20e3260f3083bcc067483b71274.nca"},
	{ "4.0.0", "f99ac61b17fdd5ae8e4dda7c0b55132a.nca"},
	{ "3.0.2", "704129fc89e1fcb85c37b3112e51b0fc.nca"},
	{ "3.0.1", "9a78e13d48ca44b1987412352a1183a1.nca"},
	{ "3.0.0", "7bef244b45bf63efb4bf47a236975ec6.nca"},
	{ "2.3.0", "d1c991c53a8a9038f8c3157a553d876d.nca"},
	{ "2.2.0", "7f90353dff2d7ce69e19e07ebc0d5489.nca"},
	{ "2.1.0", "e9b3e75fce00e52fe646156634d229b4.nca"},
	{ "2.0.0", "7a1f79f8184d4b9bae1755090278f52c.nca"},
	{ "1.0.0", "a1b287e07f8455e8192f13d0e45a2aaf.nca"} 
};

static NxSytemTitles sytemExFatTitlesArr[] = {
	{"9.0.0", "3b444768f8a36d0ddd85635199f9676f.nca" },
	{"8.1.0", "96f4b8b729ade072cc661d9700955258.nca" },
	{"8.0.1", "b2708136b24bbe206e502578000b1998.nca" }, 
	{"8.0.0", "b2708136b24bbe206e502578000b1998.nca" },
	{"7.0.1", "02a2cbfd48b2f2f3a6cec378d20a5eff.nca" },
	{"7.0.0", "58c731cdacb330868057e71327bd343e.nca" },
	{"6.2.0", "97cb7dc89421decc0340aec7abf8e33b.nca" },
	{"6.1.0", "d5186022d6080577b13f7fd8bcba4dbb.nca" },
	{"6.0.1", "d5186022d6080577b13f7fd8bcba4dbb.nca" },
	{"6.0.0", "d5186022d6080577b13f7fd8bcba4dbb.nca" },
	{"6.0.0 (pre-release)", "711b5fc83a1f07d443dfc36ba606033b.nca" },
	{"5.1.0", "c9e500edc7bb0fde52eab246028ef84c.nca" },
	{"5.0.2", "432f5cc48e6c1b88de2bc882204f03a1.nca" },
	{"5.0.1", "432f5cc48e6c1b88de2bc882204f03a1.nca" },
	{"5.0.0", "432f5cc48e6c1b88de2bc882204f03a1.nca" },
	{"4.1.0", "458a54253f9e49ddb044642286ca6485.nca" },
	{"4.0.1", "090b012b110973fbdc56a102456dc9c6.nca" },
	{"4.0.0", "090b012b110973fbdc56a102456dc9c6.nca" },
	{"3.0.2", "e7dd3c6cf68953e86cce54b69b333256.nca" },
	{"3.0.1", "17f9864ce7fe3a35cbe3e3b9f6185ffb.nca" },
	{"3.0.0", "9e5c73ec938f3e1e904a4031aa4240ed.nca" },
	{"2.3.0", "4a94289d2400b301cbe393e64831f84c.nca" },
	{"2.2.0", "4a94289d2400b301cbe393e64831f84c.nca" },
	{"2.1.0", "4a94289d2400b301cbe393e64831f84c.nca" },
	{"2.0.0", "f55a04978465ebf5666ca93e21b26dd2.nca" },
	{"1.0.0", "3b7cd379e18e2ee7e1c6d0449d540841.nca" }
};


class NxStorage {
public:
	NxStorage(const char* storage = NULL, KeySet *p_biskeys = NULL, bool debug_mode = FALSE);

	void ClearHandles();
	BOOL GetSplitFile(NxSplitFile* pFile, const char* partition);
	BOOL GetSplitFile(NxSplitFile* pFile, u64 offset);
	int ReadBufferAtOffset(BYTE *buffer, u64 offset, int length = CLUSTER_SIZE);
	int WriteBufferAtOffset(BYTE *buffer, u64 offset, int length = CLUSTER_SIZE);
	int DumpToStorage(NxStorage *out, const char* partition, u64* readAmount, u64* writeAmount, u64* bytesToWrite, HCRYPTHASH* hHash = NULL);
	int RestoreFromStorage(NxStorage *in, const char* partition, u64* readAmount, u64* writeAmount, u64* bytesToWrite);
	const char* GetNxStorageTypeAsString();
	void InitStorage();
	void InitKeySet(KeySet *p_biskeys=NULL);
	int GetMD5Hash(HCRYPTHASH *hHash, u64* readAmount = NULL);
	std::string GetMD5Hash(const char* partition = NULL);
	u64 IsValidPartition(const char * part_name, u64 part_size = NULL);
	int setCrypto(const char * partition);
	bool ValidateDecryptBuf(unsigned char *buf, const char* partition);
	bool setAutoRCM(bool enable);
	int fat32_read(const char* partition = NULL);
	int fat32_read_attr(BYTE *cluster, fs_attr *fat32_attr);
	std::string get_longfilename(BYTE *buffer, int offset, int length);
	int prodinfo_read();
	GptPartition* GetPartitionByName(const char * partition);
	int Incognito();

private:
	BOOL ParseGpt(unsigned char* gptHeader);

public:
	bool DEBUG_MODE;
	const char* path;
	LPWSTR pathLPWSTR;
	int type;
	u64 size;
	u64 raw_size;
	u64 fileDiskTotalBytes;
	u64 fileDiskFreeBytes;
	BOOL isDrive;
	BOOL backupGPTfound;
	DISK_GEOMETRY pdg;
	GptPartition *firstPartion;
	GptPartition *curPartition;
	GptPartition *nextPartition;
	int partCount;
	BOOL autoRcm;
	s8 partitionName[37];
	BOOL isSplitted = FALSE;
	NxSplitFile *lastSplitFile;
	int splitFileCount = 0;
	HCRYPTPROV h_Prov = 0;
	HCRYPTHASH h_Hash = 0;
	NxHandle handle;
	NxMMC mmc;
	HANDLE handle_out;
	u64 bytesToRead;
	u64 bytesAmount;
	KeySet* biskeys;
	BOOL crypto = FALSE, encrypt = FALSE, do_crypto = FALSE, bad_crypto = FALSE;
	BOOL isEncrypted = FALSE;
	std::vector<unsigned char> key_crypto;
	std::vector<unsigned char> key_tweak;
	xts_crypto *p_crypto = NULL;
	bool fw_detected = FALSE;
	s8 fw_version[48];
	unsigned char bootloader_ver = 0;
	bool exFat_driver = FALSE;
	s8 serial_number[18] = { 0 };
	fs_attr fs;
	char last_boot[20] = { 0 };
	u64 journal_report_off = 0;
	u64 journal_report_off_end = 0;
	u64 play_report_off = 0;
	u64 play_report_off_end = 0;
	s8 deviceId[21] = { 0 };
	s8 wlanMacAddress[12] = { 0 };
	std::string macAddress;
	bool b_MayBeNxStorage = false;

};

std::string BuildChecksum(HCRYPTHASH hHash);

