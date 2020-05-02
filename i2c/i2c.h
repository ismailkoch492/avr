#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Master Transmitter ---> S|SLA+W|a|DATA|a|P                     ---> sendStart(); 

// Slave Receiver     ---> S|SLA+W|a|DATA|a|DATA|A|P or S         ---> sendStart(); 

// Master Receiver    ---> S|SLA+R|a|data|A|data|~A|P             ---> sendStart(); 

// Slave Transmitter  ---> S|SLA+R|a|data|A|data|~A|P or S        ---> sendStart(); 

void sendStart(void);//1
void startTransmitted(void);//2
void checkStart(void);//3
void sendSLA_W(void);//4
void sendSLA_R(void);//4
void SLA_W_ACK_NACKreceived(void);//5
void SLA_R_ACK_NACKreceived();//5
void checkMT_SLA_ACK(void);//6
void checkMR_SLA_ACK(void);//6
void sendData(int DATA);//7
void receiveData(int DATA);//7
void dataACK_NACKreceived(void);//8
void checkMT_DATA_ACK(void);//9
void checkMR_DATA_ACK(void);//9
void stopTransmitted(void);//10

void sendStart(void)  //1
{
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
}

void startTransmitted(void) //2
{
  while (!(TWCR & (1<<TWINT)));
}
  
void checkStart(void) //3
{
  if((TWSR & 0xF8) != START)
    ERROR();
}

void sendSLA_W(void)  //4
{
  TWDR = SLA_W;
  TWCR = (1<<TWINT) | (1<<TWEN);
}

void sendSLA_R(void)  //4
{
  TWDR = SLA_R;
  TWCR = (1<<TWINT) | (1<<TWEN);
}
  
void SLA_W_ACK_NACKreceived(void) //5
{
  while(!(TWCR & (1<<TWINT)));
}

void SLA_R_ACK_NACKreceived //5
{
  while(!(TWCR & (1<<TWINT)));
}

void checkMT_SLA_ACK(void) //6
{
  if ((TWSR & 0xF8) != MT_SLA_ACK)
    ERROR();
}

void checkMR_SLA_ACK(void) //6
{
  if ((TWSR & 0xF8) != MR_SLA_ACK)
    ERROR();
}

void sendDAta(int DATA) //7
{
  TWDR = DATA;
  TWCR = (1<<TWINT) | (1<<TWEN);
}

void receiveData(void) //7
{
  
}

void dataACK_NACKreceived(void) //8
{
  while (!(TWCR & (1<<TWINT)));
}

void checkMT_DATA_ACK(void) //9
{
  if ((TWSR & 0xF8) != MT_DATA_ACK)
    ERROR();
}

void checkMR_DATA_ACK(void) //9
{
  
}

void stopTransmitted(void)  //10
{
  TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
}
