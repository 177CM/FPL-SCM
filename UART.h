#ifndef _UART_H_
#define _UART_H_

void Uart_Init(void);															//��ʼ������ͨ��
void Uart_Send_Byte(unsigned char c);							//����һ��Byte
unsigned char Uart_Receive_Byte();								//����һ��Byte

#endif
