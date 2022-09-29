#include <REGX52.H>
#include "LCD1602.h"
#include "MatrixKey.h"
#include "Buzzer.h"
#include "UART.h"
#include "Delay.h"
#include "Servo.h"


unsigned char KeyNum_FP;														//���󰴼����
unsigned char ConnectFlag=1;												//δʹ�õ��ı�־�Ա���
unsigned char Unlocked_FPM=0;												//ָ��ģ�鿪����־����


//as608ͨѶЭ�鶨��
volatile unsigned char FPM10A_RECEICE_BUFFER[32];
int finger_id = 0;
//������֤
code unsigned char FPM10A_Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};
//Э���ͷ
code unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  
//���ָ��ͼ��
code unsigned char FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    
//���ģ������
code unsigned char FPM10A_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; 
//����ָ��������Χ0 - 999,ʹ��BUFFER1�е�����������
code unsigned char FPM10A_Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; 
//����0-9��ָ��
code unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; 
//��ͼ����뵽BUFFER1
code unsigned char FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; 
//��ͼ����뵽BUFFER2
code unsigned char FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; 
//��BUFFER1��BUFFER2�ϳ�����ģ��
code unsigned char FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; 
//ɾ��ָ��ģ�������е�ģ��
code unsigned char FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0D,0x00,0x11};
//��BUFFER1�е��������ŵ�ָ����λ��
volatile unsigned char  FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};
//volatile:ϵͳ�������´������ڵ��ڴ��ȡ���ݣ���ʹ��ǰ���ָ��ոմӸô���ȡ������

/*------------------ FINGERPRINT������ --------------------------*/
 //���Ͱ�ͷ
void FPM10A_Cmd_Send_Pack_Head(void){
	int i;	
	for(i=0;i<6;i++){ 																	//��ͷ 
     Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    }		
}
//����ָ��
void FPM10A_Cmd_Check(void){
	int i=0;
	FPM10A_Cmd_Send_Pack_Head(); 												//����ͨ��Э���ͷ
	for(i=0;i<10;i++){		
		Uart_Send_Byte(FPM10A_Get_Device[i]);
	  }
}
//���շ������ݻ���
void FPM10A_Receive_Data(unsigned char ucLength){
  unsigned char i;
  for (i=0;i<ucLength;i++)
     FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();
}
//FINGERPRINT_���ָ��ͼ������
void FPM10A_Cmd_Get_Img(void){
    unsigned char i;
    FPM10A_Cmd_Send_Pack_Head(); 											//����ͨ��Э���ͷ
    for(i=0;i<6;i++){ 																//�������� 0x1d
		Uart_Send_Byte(FPM10A_Get_Img[i]);
	}
}
//��ͼ��ת��������������Buffer1��
void FINGERPRINT_Cmd_Img_To_Buffer1(void){
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); 												//����ͨ��Э���ͷ      
   	for(i=0;i<7;i++){   															//�������� ��ͼ��ת���� ������ ����� CHAR_buffer1     
		Uart_Send_Byte(FPM10A_Img_To_Buffer1[i]);
	}
}
//��ͼ��ת��������������Buffer2��
void FINGERPRINT_Cmd_Img_To_Buffer2(void){
     unsigned char i;
     for(i=0;i<6;i++){    														//���Ͱ�ͷ	
		Uart_Send_Byte(FPM10A_Pack_Head[i]);   
   	 }
     for(i=0;i<7;i++){   															//�������� ��ͼ��ת���� ������ ����� CHAR_buffer1     
		Uart_Send_Byte(FPM10A_Img_To_Buffer2[i]);
   	  }
}
//����ȫ���û�999ö
void FPM10A_Cmd_Search_Finger(void){
	unsigned char i;	   	    
	FPM10A_Cmd_Send_Pack_Head(); 												//����ͨ��Э���ͷ
	for(i=0;i<11;i++){         
		Uart_Send_Byte(FPM10A_Search[i]);   
	}
}
void FPM10A_Cmd_Reg_Model(void){
	unsigned char i;	   	    
	FPM10A_Cmd_Send_Pack_Head(); 												//����ͨ��Э���ͷ
	for(i=0;i<6;i++){          
		Uart_Send_Byte(FPM10A_Reg_Model[i]);   
	}
}
//ɾ��ָ��ģ���������ָ��ģ��
void FINGERPRINT_Cmd_Delete_All_Model(void){
	unsigned char i;    
    for(i=0;i<6;i++){																	//��ͷ
		Uart_Send_Byte(FPM10A_Pack_Head[i]); 
	} 		  
    for(i=0;i<6;i++){ 																//����ϲ�ָ��ģ��	
		Uart_Send_Byte(FPM10A_Delete_All_Model[i]);   
	}	
}
//����ָ��
void FPM10A_Cmd_Save_Finger( unsigned int storeID ){
	unsigned long temp = 0;
	unsigned char i;
	FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
	FPM10A_Save_Finger[6] = (storeID&0x00FF);
	for(i=0;i<7;i++){
		temp = temp + FPM10A_Save_Finger[i];							//����У���			 
	}   
	FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; 			//���У������
	FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
	FPM10A_Cmd_Send_Pack_Head(); 												//����ͨ��Э���ͷ	
	for(i=0;i<9;i++){
		Uart_Send_Byte(FPM10A_Save_Finger[i]);      			//�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
	}
}

//���ָ��
void FPM10A_Add_Fingerprint(){
	LCD_Init();
	LCD_ShowString(1,0,"Finger ID:");
	LCD_ShowString(2,0,"S9:+S10:_S12:OK");
	LCD_ShowNum(1,14,finger_id,3);
	while(1)
	{
		KeyNum_FP=MatrixKey();
			if(KeyNum_FP==9)																//ָ��id����
			{
				Buzzer_Time(100);
				finger_id++;
				finger_id%=300;
				LCD_ShowNum(1,14,finger_id,3);
				}
			if(KeyNum_FP==10)																//ָ��id����
			{
				Buzzer_Time(100);
				finger_id--;
				if(finger_id<0)
				{
					finger_id=299;															//����0-299
				}
				LCD_ShowNum(1,14,finger_id,3);
			}
			if(KeyNum_FP==12)
			{
				Buzzer_Time(100);
				LCD_Init();
				LCD_ShowString(1,0,"PleasePutFinger1");
				FPM10A_Cmd_Get_Img(); 												//���ָ��ͼ��
				FPM10A_Receive_Data(12);											//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
				if(FPM10A_RECEICE_BUFFER[9]==0)
				{
					Delay(100);
					FINGERPRINT_Cmd_Img_To_Buffer1();
					FPM10A_Receive_Data(12);
					LCD_Init();
					LCD_ShowString(1,0,"SuccessfulEntry1");
					Buzzer_Time(100);														//��������һ��
					Delay(100);
					LCD_ShowString(1,0,"PleasePutFinger2");
					FPM10A_Cmd_Get_Img(); 											//���ָ��ͼ��
					FPM10A_Receive_Data(12);										//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
				}	 
				if(FPM10A_RECEICE_BUFFER[9]==0)
				{
					Delay(200);
					LCD_Init();
					LCD_ShowString(1,0,"SuccessfulEntry2");
					LCD_ShowString(2,0,"    ID is");
					LCD_ShowNum(2,11,finger_id,3);
					FINGERPRINT_Cmd_Img_To_Buffer2();
					FPM10A_Receive_Data(12);
					FPM10A_Cmd_Reg_Model();											//ת����������
					FPM10A_Receive_Data(12); 
					FPM10A_Cmd_Save_Finger(finger_id);                		         
					FPM10A_Receive_Data(12);
					Buzzer_Time(100);														//��������һ��
					Delay(1000);
					finger_id+=1;
				}
			}
			if(KeyNum_FP==14)
			{
				LCD_ShowString(1,0,"   Add  finger  ");
				LCD_ShowString(2,0,"    ID is");
				LCD_ShowNum(2,11,finger_id,3);
			}
			if(KeyNum_FP==16){break;}
	}
}


//����ָ��
void FPM10A_Find_Fingerprint()
{
	unsigned int find_fingerid = 0;
	unsigned char id_show[]={0,0,0};
	LCD_Init();
	LCD_ShowString(1,0," Finger Please ");
	LCD_ShowString(2,0,"S13:OK  S16:Esc");
	while(1)
	{
		KeyNum_FP=MatrixKey();															//��ü�ֵ
		switch(KeyNum_FP)
		{
			case 13:
			{
				Buzzer_Time(100);
				FPM10A_Cmd_Get_Img(); 													//���ָ��ͼ��
				FPM10A_Cmd_Get_Img(); 													//���ָ��ͼ��
				FPM10A_Receive_Data(12);												//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
				if(FPM10A_RECEICE_BUFFER[9]==0)
					{
							Delay(100);
							FINGERPRINT_Cmd_Img_To_Buffer1();
							FPM10A_Receive_Data(12);
							FPM10A_Cmd_Search_Finger();
							FPM10A_Receive_Data(16);
							if(FPM10A_RECEICE_BUFFER[9]==0){ 					//������
								LCD_Init();
								LCD_ShowString(1,0," Search success ");
								LCD_ShowString(2,0,"ID:");
								Buzzer_Time(100);												//��������һ��
								find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];
								LCD_ShowNum(2,4,find_fingerid,3);
								LCD_ShowString(2,10,"S16:Esc");
								Delay(1000);
								if(Unlocked_FPM==0)
								{
									Unlock();															//��������
									Unlocked_FPM=1;
								}
							}
							else{
								LCD_Init();
								LCD_ShowString(1,0," Search  failed ");
								LCD_ShowString(2,10,"S16:ESC");
								Buzzer_Time(100);												//��������һ��}
							}
					}
					break;
			}
			case 16:
			{
				Unlocked_FPM=0;
				Buzzer_Time(100);
				return;
			}
		}
	}
}


//ɾ�����д�����ָ�ƿ�
void FPM10A_Delete_All_Fingerprint()
{
	LCD_Init();
	LCD_ShowString(1,0,"   Empty All?   ");
	LCD_ShowString(2,0,"S15:YES  S16:NO");
	while(1)
	{
		KeyNum_FP=MatrixKey();															//��ȡ��ֵ
			if(KeyNum_FP==15)
				{
					Delay(1000);
					KeyNum_FP=0;
					KeyNum_FP=MatrixKey();												//��ȡ��ֵ
					if(KeyNum_FP==15)
						{
							LCD_ShowString(1,0,"   emptying     ");							LCD_Init();

							FINGERPRINT_Cmd_Delete_All_Model();
							FPM10A_Receive_Data(12);
							LCD_Init();
							LCD_ShowString(1,0,"   All empty    ");
							Delay(300);
							Buzzer_Time(100);													//��������һ��}
					}
				}
			if(KeyNum_FP==16)
			{
				break;
			}
	}
}



//�豸���
void Device_Check(void){
	unsigned char i=0;
	FPM10A_RECEICE_BUFFER[9]=1;														//��������ھ�λ���ж��Ƿ�ͨ������
		LCD_ShowString(2,0,"Loading");
		for(i=0;i<8;i++)
		{
			LCD_ShowString(2,i+9,"*");
			Delay(50);
		}
		LCD_ShowString(2,0,"Docking  failure");
		FPM10A_Cmd_Check();																	//��Ƭ����ָ��ģ�鷢��У������
		FPM10A_Receive_Data(12);														//�����ڽ��յ�������ת��
	if(FPM10A_RECEICE_BUFFER[9] == 0)
	{
		LCD_ShowString(2,0,"Docking  success");
		ConnectFlag=0;																			//��ȷ���ӵı�־����
	}
}
//ָ��ģʽ��ʼ��
void F_Model_Init(){
	{
		LCD_Init();
		Uart_Init();																				//��ʼ������
		LCD_ShowString(1,0,"Fingerprint Test");
		Delay(500);																					//��ʱ500MS���ȴ�ָ��ģ�鸴λ
		Device_Check();		   																//У��ָ��ģ���Ƿ������ȷ��Һ��������Ӧ����ʾ
		Delay(1000);																				//�Խӳɹ�����ͣ��һ��ʱ��
	}
	LCD_ShowString(1,0,"S13:Fin  S14:Add");
	LCD_ShowString(2,0,"S15:Del  S16:ESC");
	
}











