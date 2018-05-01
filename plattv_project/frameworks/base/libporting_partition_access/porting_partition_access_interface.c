#include <stdio.h>
#include "partition_types.h"
#include "porting_partition_access_interface.h"
#include "hi_flash.h"
#include "hi_type.h"
#include <android/log.h>


/*****************	根据flash类型及分区名称打开flash*********************/
/* [in]FlashType:分区类型												*/
/* [in]pPartitionName:分区名称									        */
/* [in]addr:分区内偏移													*/
/* [in]length:打开长度													*/
/* [out] 成功：flash handle；失败：INVALID_FLASH_HANDLE                 */
/************************************************************************/
FLASH_HANDLE Porting_Flash_Open(FLASH_TYPE_E flashType, UINT_8 *pPartitionName,
		UINT_64 addr, UINT_64 length)
{
	FLASH_HANDLE ret = INVALID_FLASH_HANDLE;
	ret = HI_Flash_Open( flashType, pPartitionName, addr,length);
	if(ret == INVALID_FD)
	{
		return -1;
	}

	return ret;
}

/*****************		根据分区名称打开flash		*********************/
/* [in]pPartitionName:分区名称									        */
/* [out] 成功：flash handle；失败：INVALID_FLASH_HANDLE                 */
/************************************************************************/
FLASH_HANDLE	Porting_Flash_OpenByName(UINT_8 *pPartitionName)
{
	FLASH_HANDLE ret = INVALID_FLASH_HANDLE;
	
	if(!pPartitionName)
	{
		printf("Porting_Flash_OpenByName param error\n");
		return -1;
	}
	ret = HI_Flash_OpenByName((HI_CHAR*)pPartitionName);
	if(ret == INVALID_FD)
	{
		return -1;
	}
	return ret;
}

/*****************根据分区类型及偏移地址打开flash	*********************/
/* [in]FlashType:分区类型												*/
/* [in]addr:分区内偏移													*/
/* [in]length:打开长度													*/
/* [out] 成功：flash handle；失败：INVALID_FLASH_HANDLE                 */
/************************************************************************/
FLASH_HANDLE Porting_Flash_OpenByTypeAndAddr(FLASH_TYPE_E FlashType,
		UINT_64 addr, UINT_64 length)
{
	FLASH_HANDLE ret = INVALID_FLASH_HANDLE;
	ret = HI_Flash_OpenByTypeAndAddr( FlashType, addr, length);
	if(ret == INVALID_FD)
	{
		return -1;
	}

	return ret;
}

/*****************		擦除指定内容				*********************/
/* [in]handle:flash handle												*/
/* [in]addr:分区内偏移													*/
/* [in]length:擦除长度													*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_Erase(FLASH_HANDLE handle, UINT_64 addr, UINT_64 length)
{

	int ret;
	if((addr < 0) || (length < 0))
	{
		printf("Porting_Flash_Erase param error:handle=%0x,addr=%0x,length=%0x\n",handle,addr,length);
		return -1;
	}
	ret = HI_Flash_Erase(handle, addr, length);
	if(ret == HI_FAILURE)
	{
		return -1;
	}

	return ret;
}

/*****************		读取分区内容				*********************/
/* [in]handle:flash handle												*/
/* [in]addr:分区内偏移													*/
/* [out]pBuf:数据缓存													*/
/* [in]length:读取长度													*/
/* [in]rwFlags：读写方式(见上面常量定义：FLASH_RW_FLAG_XXX)				*/
/* [out] 成功：实际数据长度；失败：-1               */
/************************************************************************/
INT_32 Porting_Flash_Read(FLASH_HANDLE handle, UINT_64 addr, UINT_8 *pBuf,
		UINT_64 length, UINT_32 rwFlags)
{
	int ret;
	
	printf("Porting_Flash_Read...in length:%d\n",length);
	if((addr < 0) || (pBuf == NULL) || (length < 0))
	{
		printf("Porting_Flash_Read param error:\n");
		return -1;
	}

	ret = HI_Flash_Read(handle, addr, pBuf, length, rwFlags);

	if(ret == HI_FAILURE)
	{
		return -1;
	}

	printf("Porting_Flash_Read...out pBuf:%s length:%d ret:%d\n",pBuf,length,ret);
	return ret;
	
}


/*****************		擦写分区					*********************/
/* [in]handle:flash handle												*/
/* [in]addr:分区内偏移													*/
/* [in]pBuf:数据缓存													*/
/* [in]length:写长度													*/
/* [in]rwFlags：读写方式(见上面常量定义：FLASH_RW_FLAG_XXX)				*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_Write(FLASH_HANDLE handle, UINT_64 addr, UINT_8 *pBuf,
		UINT_64 length, UINT_32 rwFlags)
{
	int ret;
	printf("Porting_Flash_Write...in pBuf:%s length:%d\n",pBuf,length);
	ret = HI_Flash_Write(handle, addr, pBuf, length, rwFlags);
	printf("Porting_Flash_Write...out ret:%d\n",ret);
	if(ret == HI_FAILURE)
	{
		return -1;
	}else{
		return OPERATION_SUCCESS;
	}
	return ret;
}

/*****************		获取分区信息				*********************/
/* [in]handle:flash handle												*/
/* [out]info:分区信息													*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_GetInfo(FLASH_HANDLE handle, FlashInfo* info)
{
	int ret;
	HI_Flash_InterInfo_S  getInfo[1];
	if(!info)
	{
		printf("Porting_Flash_GetInfo param error:\n");
		return -1;
	}

	ret = HI_Flash_GetInfo(handle, getInfo);
	if(ret == HI_FAILURE)
	{
		return -1;
	}else{
		ret = OPERATION_SUCCESS;
	}

	info->TotalSize = getInfo->TotalSize;
	info->PartSize  = getInfo->PartSize;
	info->BlockSize = getInfo->BlockSize;
	info->PageSize  = getInfo->PageSize;
	info->OobSize   = getInfo->OobSize;
	info->fd        = getInfo->fd;
	info->OpenAddr  = getInfo->OpenAddr;
	info->OpenLeng  = getInfo->OpenLeng;
	info->FlashType = getInfo->FlashType;

	return ret;
}

/*****************		关闭分区					*********************/
/* [in]handle:flash handle												*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_Close(FLASH_HANDLE handle)
{
	int ret= 0;
	ret = HI_Flash_Close(handle);
	if(ret == HI_FAILURE)
	{
		return -1;
	}
	
	return ret ;
}



/*
int main()
{
		char* name ="misc";
		char miscbuf[1024] = { 0 };

		int handle;
		strcpy(miscbuf, "boot-recovery");
		printf("miscbuf %s\n",miscbuf);
		handle = Porting_Flash_OpenByName(name);
		if(handle == -1)
		{
			printf("handle == -1\n");
			return -1;
		}
		int ret	= Porting_Flash_Write(handle,0,	miscbuf,sizeof(miscbuf),0);
		if(ret == -1)
		{
				printf("write failed\n");
		}
		return 0;
}
*/