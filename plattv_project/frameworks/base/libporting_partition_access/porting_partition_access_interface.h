#ifndef __PORTING_PARTITION_ACCESS_INTERFACE__
#define __PORTING_PARTITION_ACCESS_INTERFACE__

#include "partition_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************	根据flash类型及分区名称打开flash*********************/
/* [in]FlashType:分区类型												*/
/* [in]pPartitionName:分区名称									        */
/* [in]addr:分区内偏移													*/
/* [in]length:打开长度													*/
/* [out] 成功：flash handle；失败：INVALID_FLASH_HANDLE                 */
/************************************************************************/
FLASH_HANDLE Porting_Flash_Open(FLASH_TYPE_E flashType, UINT_8 *pPartitionName,
		UINT_64 addr, UINT_64 length);

/*****************		根据分区名称打开flash		*********************/
/* [in]pPartitionName:分区名称									        */
/* [out] 成功：flash handle；失败：INVALID_FLASH_HANDLE                 */
/************************************************************************/
FLASH_HANDLE	Porting_Flash_OpenByName(UINT_8 *pPartitionName);

/*****************根据分区类型及偏移地址打开flash	*********************/
/* [in]FlashType:分区类型												*/
/* [in]addr:分区内偏移													*/
/* [in]length:打开长度													*/
/* [out] 成功：flash handle；失败：INVALID_FLASH_HANDLE                 */
/************************************************************************/
FLASH_HANDLE Porting_Flash_OpenByTypeAndAddr(FLASH_TYPE_E FlashType,
		UINT_64 addr, UINT_64 length);

/*****************		擦除指定内容				*********************/
/* [in]handle:flash handle												*/
/* [in]addr:分区内偏移													*/
/* [in]length:擦除长度													*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_Erase(FLASH_HANDLE handle, UINT_64 addr, UINT_64 length);

/*****************		读取分区内容				*********************/
/* [in]handle:flash handle												*/
/* [in]addr:分区内偏移													*/
/* [out]pBuf:数据缓存													*/
/* [in]length:读取长度													*/
/* [in]rwFlags：读写方式(见上面常量定义：FLASH_RW_FLAG_XXX)				*/
/* [out] 成功：实际数据长度；失败：-1               */
/************************************************************************/
INT_32 Porting_Flash_Read(FLASH_HANDLE handle, UINT_64 addr, UINT_8 *pBuf,
		UINT_64 length, UINT_32 rwFlags);

/*****************		擦写分区					*********************/
/* [in]handle:flash handle												*/
/* [in]addr:分区内偏移													*/
/* [in]pBuf:数据缓存													*/
/* [in]length:写长度													*/
/* [in]rwFlags：读写方式(见上面常量定义：FLASH_RW_FLAG_XXX)				*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_Write(FLASH_HANDLE handle, UINT_64 addr, UINT_8 *pBuf,
		UINT_64 length, UINT_32 rwFlags);

/*****************		获取分区信息				*********************/
/* [in]handle:flash handle												*/
/* [out]info:分区信息													*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_GetInfo(FLASH_HANDLE handle, FlashInfo* info);

/*****************		关闭分区					*********************/
/* [in]handle:flash handle												*/
/* [out] 成功：OPERATION_SUCCESS；失败：OPERATION_FAILURE               */
/************************************************************************/
INT_32 Porting_Flash_Close(FLASH_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif/*__PORTING_PARTITION_ACCESS_INTERFACE__*/
