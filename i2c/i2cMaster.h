#ifndef I2CMASTER
#define I2CMASTER

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define F_SCL 400000UL
#define I2C_PRES 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdlib.h>

// Single Byte Write Sequence (MTSR)
// Master Transmitter      S|SLA+W|-|DATA|-|DATA|-|P or S               sendStart();   |sendSLA_W();   |receiveSLA_W_ACK_NACK();|sendData();   |receiveDataACK_NACK();|sendData();|receiveDataACK_NACK();|sendStop();
// Slave Receiver          -|-----|a|----|a|----|a|------               receiveStart();|receiveSLA_W();|sendSLA_W_ACK_NACK();   |receiveData();|sendDataACK_NACK();|receiveData();|sendDataACK_NACK();   |receiveStop();

// Burst Write Sequence (MTSR)
// Master Transmitter      S|SLA+W|-|DATA|-|DATA|-|DATA|-|P or S        sendStart();   |sendSLA_W();   |receiveSLA_W_ACK_NACK();|sendData();   |receiveDataACK_NACK();|sendData();   |receiveDataACK_NACK();|sendData();   |receiveDataACK_NACK();|sendStop();
// Slave Receiver          -|-----|a|----|a|----|a|----|a|------        receiveStart();|receiveSLA_W();|sendSLA_W_ACK_NACK();   |receiveData();|sendDataACK_NACK();   |receiveData();|sendDataACK_NACK();   |receiveData();|sendDataACK_NACK();   |receiveStop();

// Single Byte Read Sequence (MRST)
// Master Transmitter      S|SLA+W|-|DATA|-|S|SLA+R|-|----|~A|P         sendStart();   |sendSLA_W();   |receiveSLA_W_ACK_NACK();|sendRepeatedData();|receiveDataACK_NACK();|sendStart();   |sendSLA_R();   |receiveSLA_R_ACK_NACK();|receiveData();|sendDataNACK();       |sendStop();
// Slave Receiver          -|-----|a|----|a|-|-----|a|data|--|-         receiveStart();|receiveSLA_W();|sendSLA_W_ACK_NACK();   |receiveData();     |sendDataACK_NACK();   |receiveStart();|receiveSLA_R();|sendSLA_R_ACK_NACK();   |sendData();   |receiveDataACK_NACK();|receiveStop();

// Burst Read Sequence (MRST)
// Master Transmitter      S|SLA+W|-|DATA|-|S|SLA+R|-|----|A|----|~A|P  sendStart();   |sendSLA_W();   |receiveSLA_W_ACK_NACK();|sendData();   |receiveDataACK_NACK();|sendRepeatedStart();|sendSLA_R();   |receiveSLA_R_ACK_NACK();|receiveData();|sendDataACK_NACK();   |receiveData();|sendDataNACK();       |sendStop();
// Slave Receiver          -|-----|a|----|a|-|-----|a|data|-|data|--|-  receiveStart();|receiveSLA_W();|sendSLA_W_ACK_NACK();   |receiveData();|sendDataACK_NACK();   |receiveStart();     |receiveSLA_R();|sendSLA_R_ACK_NACK();   |sendData();   |receiveDataACK_NACK();|sendData();   |receiveDataACK_NACK();|receiveStop();

#if I2C_PRES == 1
TWSR &= ~(1<<TWSP1)|(1<<TWSP0);
#elif I2C_PRES == 4
TWSR &= ~(1<<TWSP1);
TWSR |= (1<<TWSP0);
#elif I2C_PRES == 16
TWSR &= ~(1<<TWSP0);
TWSR |= (1<<TWSP1);
#elif I2C_PRES == 64
TWSR |= (1<<TWSP1)|(1<<TWSP0);
#endif

TWBR = (F_CPU/F_SCL - 16)/(2*I2C_PRES);

void sendStart(void);//1
void sendRepeatedStart(void);//1
void receiveStart(void);//1
void startTransmitted(void);//2
void checkStart(void);//3
void checkRepeatedStart(void);//3
void sendSLA_W(int SLA_W);//4
void sendSLA_R(int SLA_R);//4
void receiveSLA_W(void);//4
void receiveSLA_R(void);//4
void sendSLA_W_ACK_NACK(void);
void sendSLA_R_ACK_NACK(void);
void receiveSLA_W_ACK_NACK(void);//5
void receiveSLA_R_ACK_NACK(void);//5
void checkMT_SLA_ACK(void);//6
void checkMR_SLA_ACK(void);//6
void sendData(int DATA);//7
int receiveData(void);//7
void sendDataACK_NACK(void);//8
void sendDataNACK(void);//8
void receiveDataACK_NACK(void);//8
void checkMT_DATA_ACK(void);//9
void checkMR_DATA_ACK(void);//9
void sendStop(void);//10
void receiveStop(void);//10
void SingleMTSR(int slaveAddress, int registerAddress, int data);
void BurstMTSR(int slaveAddress, int registerAddress, int firstData, int secondData);
void SingleMRST(int slaveAddress, int registerAddress);
void BurstMRST(int slaveAddress, int registerAddress);

void sendStart(void)  //1
{
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
  startTransmitted(); //2
  checkStart(); //3
}

void sendRepeatedStart(void)
{
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
  startTransmitted(); //2
  checkRepeatedStart(); //3
}

void startTransmitted(void) //2
{
  while (!(TWCR & (1<<TWINT)));
}

void checkStart(void) //3
{
  if((TWSR & 0xF8) != TW_START)
    continue;
}

void checkRepeatedStart(void)
{
  if((TWSR & 0xF8) != TW_REP_START)
  {
    sendStop();
    continue;
  }
}

void sendSLA_W(int SLA_W)  //4
{
  SLA_W = (SLA_W<<1) | (TW_WRITE<<0);
  TWDR = SLA_W;
  TWCR = (1<<TWINT) | (1<<TWEN);
}

void sendSLA_R(int SLA_R)  //4
{
  SLA_R = (SLA_R<<1) | (TW_READ<<0);
  TWDR = SLA_R;
  TWCR = (1<<TWINT) | (1<<TWEN);
}

void receiveSLA_W_ACK_NACK(void) //5
{
  while(!(TWCR & (1<<TWINT)));
  checkMT_SLA_ACK();
}

void receiveSLA_R_ACK_NACK(void) //5
{
  while(!(TWCR & (1<<TWINT)));
  checkMR_SLA_ACK();
}

void checkMT_SLA_ACK(void) //6
{
  if ((TWSR & 0xF8) == TW_MT_SLA_NACK)
  {
    sendStop();
    continue;
  }
}

void checkMR_SLA_ACK(void) //6
{
  if ((TWSR & 0xF8) == TW_MR_SLA_NACK)
  {
    sendStop();
    continue;
  }
}

void sendData(int DATA) //7
{
  while(!(TWCR & (1<<TWINT)));
  TWDR = DATA;
  TWCR = (1<<TWINT) | (1<<TWEN);
}

int receiveData(void) //7
{
  while(!(TWCR & (1<<TWINT)));
  int data = TWDR;
  TWCR = (1<<TWINT) | (1<<TWEN);
  return(data);
}

void receiveDataACK_NACK(void) //8
{
  while (!(TWCR & (1<<TWINT)));
  checkMT_DATA_ACK();
}

void sendDataACK_NACK(void) //8
{
  //send ack bit when master receive data from slave.
  checkMR_DATA_ACK();
}

void sendDataNACK(void)
{
  //if 0x50
  TWCR = (1<<TWINT);
  //else if others
  //TWCR = ();

}

void checkMT_DATA_ACK(void) //9
{
  if ((TWSR & 0xF8) == TW_MT_DATA_NACK)
  {
    sendStop();
    continue;
  }
}

void checkMR_DATA_ACK(void) //9
{
  if ((TWSR & 0xF8) != TW_MR_DATA_NACK)
  {
    sendStop();
    continue;
  }
  else if ((TWSR & 0xF8) == TW_MR_DATA_ACK)
  {
    TWCR |= (1<<TWINT);
  }
}

void sendStop(void)  //10
{
  TWCR = (1<<TWINT) | (1<<TWEN)| (1<<TWSTO);
}

void SingleMTSR(int slaveAddress, int registerAddress, int data)
{
  sendStart();
  sendSLA_W(slaveAddress);
  receiveSLA_W_ACK_NACK();
  sendData(registerAddress);
  receiveDataACK_NACK();
  sendData(data);
  receiveDataACK_NACK();
  sendStop();
}

void BurstMTSR(int slaveAddress, int registerAddress, int firstData, int secondData)
{
  sendStart();                //0x08 (TW_START)
  sendSLA_W(slaveAddress);
  receiveSLA_W_ACK_NACK();    //0x18 (TW_MT_SLA_ACK) | 0x20 (TW_MT_SLA_NACK)
  sendData(registerAddress);
  receiveDataACK_NACK();      //0x28 (TW_MT_DATA_ACK) | 0x30 (TW_MT_DATA_NACK)
  sendData(firstData);
  receiveDataACK_NACK();      //0x28 (TW_MT_DATA_ACK) | 0x30 (TW_MT_DATA_NACK)
  sendData(secondData);
  receiveDataACK_NACK();      //0x28 (TW_MT_DATA_ACK) | 0x30 (TW_MT_DATA_NACK)
  sendStop();
}

void SingleMRST(int slaveAddress, int registerAddress)
{
  int data;
  sendStart();
  sendSLA_W(slaveAddress);
  receiveSLA_W_ACK_NACK();
  sendData(registerAddress);
  receiveDataACK_NACK();
  sendRepeatedStart();
  sendSLA_R(slaveAddress);
  receiveSLA_R_ACK_NACK();
  data = receiveData();
  sendDataNACK();
  sendStop();
  return(data);
}

void BurstMRST(int slaveAddress, int registerAddress)
{
  int data[2];
  sendStart();              //0x08 (TW_START)
  sendSLA_W(slaveAddress);
  receiveSLA_W_ACK_NACK();  //0x18 (TW_MT_SLA_ACK) | 0x20 (TW_MT_SLA_NACK)
  sendData(registerAddress);
  receiveDataACK_NACK();    //0x28 (TW_MT_DATA_ACK) | 0x30 (TW_MT_DATA_NACK)
  sendRepeatedStart();      //0x10 (TW_REP_START)
  sendSLA_R(slaveAddress);
  receiveSLA_R_ACK_NACK();  //0x40 (TW_MR_SLA_ACK) | 0x48 (TW_MR_SLA_NACK)
  data[0] = receiveData();
  sendDataACK_NACK();       //0x50 (TW_MR_DATA_ACK)
  data[1] = receiveData();
  sendDataNACK();           //0x58 (TW_MR_DATA_NACK)
  sendStop();
  return(data);
}

#endif
