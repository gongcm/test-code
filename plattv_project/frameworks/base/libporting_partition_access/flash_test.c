#include <stdio.h>
#include <unistd.h>
#include "partition_types.h"
#include "porting_partition_access_interface.h"

int main(void)
{
	FLASH_HANDLE f_handle = -1;
	FlashInfo f_info;
	memset(&f_info, 0, sizeof(FlashInfo));
	
	printf("start test porting flash api.\n");
	
	f_handle = Porting_Flash_OpenByName("misc");	
	if(f_handle == INVALID_FLASH_HANDLE)
	{
			printf("open misc failure.\n");
			return -1;
	}
	
	Porting_Flash_GetInfo(f_handle, &f_info);
	
	unsigned char cmdbuf[100];
	unsigned char recoverybuf[100];
	unsigned char *cmdptr = "boot-recovery";
//	unsigned char *recoveryptr = "recovery\n--update_package=/otapackage/update.zip";
	unsigned char *recoveryptr = "recovery\n--force_upgrade";
	memset(cmdbuf, 0, 100);
	memset(recoverybuf, 0, 100);
/*	char i;
	for(i=0; i<10; i++)
	{
		buf[i] = i;
		}*/
	memcpy(cmdbuf, cmdptr, strlen(cmdptr));
	memcpy(recoverybuf, recoveryptr, strlen(recoveryptr));
	int rev;
	rev = Porting_Flash_Write(f_handle,0, cmdbuf, strlen(cmdptr), FLASH_RW_FLAG_RAW); 
	if(rev == OPERATION_SUCCESS)
	{
		printf("flash write success.\n");
		}
		
	rev = Porting_Flash_Write(f_handle,64, recoverybuf, strlen(recoveryptr), FLASH_RW_FLAG_RAW); 
	if(rev == OPERATION_SUCCESS)
	{
		printf("flash write success.\n");
		}
		
		
	sleep(1);
	memset(cmdbuf, 0, 100);
	memset(recoverybuf, 0, 100);
	rev = Porting_Flash_Read(f_handle,0, cmdbuf, strlen(cmdptr), FLASH_RW_FLAG_RAW); 
	if(rev != -1)
	{
		printf("flash read cmd success.\n");
		printf("cmd:%s\n", cmdbuf);
		}
	rev = Porting_Flash_Read(f_handle,64, recoverybuf, strlen(recoveryptr), FLASH_RW_FLAG_RAW); 
	if(rev != -1)
	{
		printf("flash read recovery success.\n");
		printf("recovery:%s\n", recoverybuf);
		}

	Porting_Flash_Close(f_handle);
	return 0;
}