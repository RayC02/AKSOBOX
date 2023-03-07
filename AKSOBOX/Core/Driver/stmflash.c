#include "stmflash.h"

#include "string.h"



/**
 * @brief	��ȡָ����ַ����(32λ����)
 *
 * @param   faddr	����ַ
 *
 * @return  u32		��ȡ������
 */
u32 STMFLASH_ReadWord(u32 faddr)
{
    return *(vu32*)faddr;
}


/**
 * @brief	ʶ��FLASH��ַ����BANK(��оƬ������BANK)
 *
 * @param   Addr	FLASH��ַ
 *
 * @return  u32		FLASH��ַ����BANK��
 */
static uint32_t Get_FLASH_Bank(uint32_t Addr)
{
    uint32_t bank = 0;

    /* No Bank swap */
    if(Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        bank = FLASH_BANK_1;
    }

    else
    {
        bank = FLASH_BANK_2;
    }

    return bank;
}


/**
 * @brief	������д������(64λ)
 *
 * @param  	WriteAddr	��ʼ��ַ(�˵�ַ����Ϊ8�ı���!!)
 * @param  	pBuffer		����ָ��
 * @param  	NumToWrite	����(64λ)��(����Ҫд���64λ���ݵĸ���)
 *
 * @return  void
 */
void STMFLASH_Write_NoCheck(u32 WriteAddr, uint64_t *pBuffer, u32 NumToWrite)
{
    u32 i;
	u32 res;

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
    for(i = 0; i < NumToWrite; i++)
    {
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WriteAddr, pBuffer[i]);
		
        if(  res != HAL_OK )
		{
			return;		//д����󣡣���
		}
        WriteAddr += 8; //��ַ����
    }
}


/**
 * @brief	��ָ����ַ��ʼд��ָ�����ȵ����ݣ�ÿ��д������������(64λ)
 *
 * @param  	WriteAddr	��ʼ��ַ(�˵�ַ����Ϊ8�ı���!!)
 * @param  	pBuffer		����ָ��
 * @param  	NumToWrite	����(64λ)��(����Ҫд���64λ���ݵĸ���)
 *
 * @return  void
 */
#define STM_SECTOR_SIZE	2048
static __align(8) uint64_t STMFLASH_BUF[STM_SECTOR_SIZE / 8]; //�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr, uint64_t *pBuffer, u32 NumToWrite)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    uint32_t	SectorError;

    if(WriteAddr < STM32_FLASH_BASE || WriteAddr % 8)return;	//�Ƿ���ַ

    u32 secpos;	   //������ַ
    u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
    u16 secremain; //������ʣ���ַ(16λ�ּ���)
    u32 offaddr;   //ȥ��0X08000000��ĵ�ַ

    offaddr = WriteAddr - STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
    secpos = offaddr / STM_SECTOR_SIZE;			//������ַ
    secoff = (offaddr % STM_SECTOR_SIZE) / 8;		//��ҳ�ڵ�ƫ��(8���ֽ�Ϊ������λ)
    secremain = STM_SECTOR_SIZE / 8 - secoff;		//����ʣ��ռ��С

    if(NumToWrite <= secremain)secremain = NumToWrite; //�����ڸ�������Χ

    HAL_FLASH_Unlock();             //����

    while(1)
    {
		//1.������ҳȫ������
        STMFLASH_Read(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, (u32*)STMFLASH_BUF, STM_SECTOR_SIZE / 4);	
		
		//2.������ҳ
        {
            FlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES; //�������ͣ���������
            FlashEraseInit.Banks = Get_FLASH_Bank(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE); //Ҫ������Bank
            FlashEraseInit.Page = secpos;    			//Ҫ������ҳ
            FlashEraseInit.NbPages = 1;      			//��Ҫɾ����ҳ��

            if(HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
            {
                break;//����������
            }
		}
		
		//3.д����
		{
			for(u16 i=0;i<(secremain*8);i++)
				((u8 *)STMFLASH_BUF)[i+secoff*8] = ((u8 *)pBuffer)[i];

			STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 8);
		}
  
        if(NumToWrite == secremain)break; //д�������
        else
        {
            secpos++;				//������ַ��1
            secoff = 0;				//ƫ��λ��Ϊ0
            pBuffer += secremain;  	//ָ��ƫ��
            WriteAddr += (secremain * 8); //д��ַƫ��
            NumToWrite -= secremain;	//�ֽ�(64λ)���ݼ�

            if(NumToWrite > (STM_SECTOR_SIZE / 8))secremain = STM_SECTOR_SIZE / 8; //��һ����������д����

            else secremain = NumToWrite; //��һ����������д����
        }
    }

    HAL_FLASH_Lock();           //����
}

/**
 * @brief	��ָ����ַ��ʼ����ָ�����ȵ����ݣ�ÿ�ζ���һ����(32λ)
 *
 * @param  	ReadAddr	��ʼ��ַ
 * @param  	pBuffer		����ָ��
 * @param  	NumToRead	������(32λ)��
 *
 * @return  void
 */
void STMFLASH_Read(u32 ReadAddr, u32 *pBuffer, u32 NumToRead)
{
    u32 i;

    for(i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); //��ȡ4���ֽ�.
        ReadAddr += 4; //ƫ��4���ֽ�.
    }
}



