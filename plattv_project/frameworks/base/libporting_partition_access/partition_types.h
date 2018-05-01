#ifndef _PORTING_PARTITION_TYPES_H_
#define _PORTING_PARTITION_TYPES_H_

typedef enum _FLASH_TYPE_E {
	FLASH_TYPE_SPI,
	FLASH_TYPE_NAND,
	FLASH_TYPE_EMMC,
	FLASH_TYPE_BUTT
} FLASH_TYPE_E;

#define		FLASH_RW_FLAG_RAW           0x0   /* read/write without oob, such as write kernel/uboot/ubi/cramfs.. */
#define		FLASH_RW_FLAG_WITH_OOB      0x1   /* read/write nand with oob, such as write yaffs2 filesystem image */
#define		FLASH_RW_FLAG_ERASE_FIRST   0x2   /* erase flash before write */

#define  INVALID_FLASH_HANDLE	-1
#define  OPERATION_SUCCESS		0
#define  OPERATION_FAILURE		-1

typedef signed int FLASH_HANDLE;
typedef unsigned char UINT_8;
typedef unsigned short UINT_16;
typedef unsigned int UINT_32;
typedef unsigned long UINT_64;
typedef char INT_8;
typedef short INT_16;
typedef int INT_32;
typedef long  INT_64;

typedef struct _flash_info {
	UINT_64 TotalSize; /**<Flash Partition Total Size*/
	UINT_64 PartSize; /**<Flash Partition Size*/
	UINT_32 BlockSize; /**<One Block Size*/
	UINT_32 PageSize; /**<One Page Size*/
	UINT_32 OobSize; /**<One Oob Size*/
	INT_32 fd; /**<file Handle*/
	UINT_64 OpenAddr;
	UINT_64 OpenLeng;
	FLASH_TYPE_E FlashType;
} FlashInfo;

#endif /* _PORTING_PARTITION_TYPES_H_ */
