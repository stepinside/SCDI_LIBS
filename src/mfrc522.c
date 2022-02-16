#include "mfrc522.h"
#include <spi.h>
#include <string.h>


uint8_t	mfrc522_request(uint8_t req_mode, uint8_t * tag_type);
uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len);

#define CARD_FOUND		        1
#define CARD_NOT_FOUND	      2
#define ERROR			            3

#define MAX_LEN			          16

// Card types
#define Mifare_UltraLight 	  0x4400
#define Mifare_One_S50		    0x0400
#define Mifare_One_S70		    0x0200
#define Mifare_Pro_X		      0x0800
#define Mifare_DESFire		    0x4403

// Mifare_One card command word
#define PICC_REQIDL           0x26               // find the antenna area does not enter hibernation
#define PICC_REQALL           0x52               // find all the cards antenna area
#define PICC_ANTICOLL         0x93               // anti-collision
#define PICC_SELECTTAG        0x93               // selection card
#define PICC_AUTHENT1A        0x60               // authentication key A
#define PICC_AUTHENT1B        0x61               // authentication key B
#define PICC_READ             0x30               // Read Block
#define PICC_WRITE            0xA0               // write block
#define PICC_DECREMENT        0xC0               // debit
#define PICC_INCREMENT        0xC1               // recharge
#define PICC_RESTORE          0xC2               // transfer block data to the buffer
#define PICC_TRANSFER         0xB0               // save the data in the buffer
#define PICC_HALT             0x50               // Sleep

volatile uint8_t *mfrc522SpiPort;
uint8_t mfrc522SpiPortPin;

void mfrc522_init(volatile uint8_t *pSpiPort, uint8_t spiPortPin)
{
  uint8_t byte;

  mfrc522SpiPort    = pSpiPort;
  mfrc522SpiPortPin = spiPortPin;

  *mfrc522SpiPort |= (1 << mfrc522SpiPortPin);          // PORTx pin (slave select) set to 1
  *(mfrc522SpiPort-1) |= (1 << mfrc522SpiPortPin);      // DDRx pin set to output

  if (!spi_isInitialized())
  {
    spi_init();
  }

  mfrc522_reset();

  mfrc522_write(MFRC522_TModeReg, 0x8D);
  mfrc522_write(MFRC522_TPrescalerReg, 0x3E);
  mfrc522_write(MFRC522_TReloadReg_1, 30);
  mfrc522_write(MFRC522_TReloadReg_2, 0);
  mfrc522_write(MFRC522_TxASKReg, 0x40);
  mfrc522_write(MFRC522_ModeReg, 0x3D);
  mfrc522_write(MFRC522_RFCfgReg, 0x78);          // max. gain

  // new
  mfrc522_write(MFRC522_ComIEnReg, 0x7F);

  byte = mfrc522_read(MFRC522_TxControlReg);
  if(!(byte&0x03))
  {
    mfrc522_write(MFRC522_TxControlReg,byte|0x03);
  }
}

void mfrc522_write(uint8_t reg, uint8_t data)
{
  *mfrc522SpiPort &= ~(1 << mfrc522SpiPortPin);          // PORTx pin (slave select) set to 0
  spi_transceive((reg<<1)&0x7E);
  spi_transceive(data);
  *mfrc522SpiPort |= (1 << mfrc522SpiPortPin);           // PORTx pin (slave select) set to 1
}

uint8_t mfrc522_read(uint8_t reg)
{
  uint8_t data;
  *mfrc522SpiPort &= ~(1 << mfrc522SpiPortPin);          // PORTx pin (slave select) set to 0
  spi_transceive(((reg<<1)&0x7E)|0x80);
  data = spi_transceive(0x00);
  *mfrc522SpiPort |= (1 << mfrc522SpiPortPin);           // PORTx pin (slave select) set to 1
  return data;
}

void mfrc522_reset()
{
  mfrc522_write(MFRC522_CommandReg,MFRC522_SoftReset_CMD);
}

uint8_t	mfrc522_request(uint8_t req_mode, uint8_t * tag_type)
{
  uint8_t  status;
  uint32_t backBits;//The received data bits

  mfrc522_write(MFRC522_BitFramingReg, 0x07);//TxLastBists = BitFramingReg[2..0]	???

  tag_type[0] = req_mode;
  status = mfrc522_to_card(MFRC522_Transceive_CMD, tag_type, 1, tag_type, &backBits);

  if ((status != CARD_FOUND) || (backBits != 0x10))
  {
    status = ERROR;
  }

  return status;
}

uint8_t	mfrc522_detectCard()
{
  uint8_t  status;
  uint32_t backBits;//The received data bits
  uint8_t temp[16];

  mfrc522_write(MFRC522_BitFramingReg, 0x07);//TxLastBists = BitFramingReg[2..0]	???

  temp[0] = PICC_REQALL;
  status = mfrc522_to_card(MFRC522_Transceive_CMD, temp, 1, temp, &backBits);

  if ((status != CARD_FOUND) || (backBits != 0x10))
  {
    return 0;
  }

  return 1;
}


uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len)
{
  uint8_t status = ERROR;
  uint8_t irqEn = 0x00;
  uint8_t waitIRq = 0x00;
  uint8_t lastBits;
  uint8_t n;
  uint8_t	tmp;
  uint32_t i;

  switch (cmd)
  {
    case MFRC522_MFAuthent_CMD:		//Certification cards close
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case MFRC522_Transceive_CMD:	//Transmit FIFO data
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
    break;
  }

  //mfrc522_write(ComIEnReg, irqEn|0x80);	//Interrupt request
  n=mfrc522_read(MFRC522_ComIrqReg);
  mfrc522_write(MFRC522_ComIrqReg,n&(~0x80));//clear all interrupt bits
  n=mfrc522_read(MFRC522_FIFOLevelReg);
  mfrc522_write(MFRC522_FIFOLevelReg,n|0x80);//flush FIFO data

  mfrc522_write(MFRC522_CommandReg, MFRC522_Idle_CMD);	//NO action; Cancel the current cmd???

  //Writing data to the FIFO
  for (i=0; i<send_data_len; i++)
  {
    mfrc522_write(MFRC522_FIFODataReg, send_data[i]);
  }

  //Execute the cmd
  mfrc522_write(MFRC522_CommandReg, cmd);
  if (cmd == MFRC522_Transceive_CMD)
  {
    n=mfrc522_read(MFRC522_BitFramingReg);
    mfrc522_write(MFRC522_BitFramingReg,n|0x80);
  }

  //Waiting to receive data to complete
  i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
  do
  {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = mfrc522_read(MFRC522_ComIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x01) && !(n&waitIRq));

  tmp=mfrc522_read(MFRC522_BitFramingReg);
  mfrc522_write(MFRC522_BitFramingReg,tmp&(~0x80));

  if (i != 0)
  {
    if(!(mfrc522_read(MFRC522_ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
    {
      status = CARD_FOUND;
      if (n & irqEn & 0x01)
      {
        status = CARD_NOT_FOUND;			//??
      }

      if (cmd == MFRC522_Transceive_CMD)
      {
        n = mfrc522_read(MFRC522_FIFOLevelReg);
        lastBits = mfrc522_read(MFRC522_ControlReg) & 0x07;
        if (lastBits)
        {
          *back_data_len = (n-1)*8 + lastBits;
        }
        else
        {
          *back_data_len = n*8;
        }

        if (n == 0)
        {
          n = 1;
        }
        if (n > MAX_LEN)
        {
          n = MAX_LEN;
        }

        //Reading the received data in FIFO
        for (i=0; i<n; i++)
        {
          back_data[i] = mfrc522_read(MFRC522_FIFODataReg);
        }
      }
    }
    else
    {
      status = ERROR;
    }

  }

  //SetBitMask(ControlReg,0x80);           //timer stops
  //mfrc522_write(cmdReg, PCD_IDLE);

  return status;
}

uint8_t mfrc522_getVersion()
{
  uint8_t byte;

  byte = mfrc522_read(MFRC522_VersionReg);
  if (byte == 0x88 || byte == 0x92 || byte == 0x91 || byte==0x90)
  {
    return byte;
  }
  else
  {
    return 0;
  }
}

uint8_t mfrc522_getUID(uint32_t* pUid)
{
  uint8_t status;
  uint8_t i;
  uint8_t serNumCheck=0;
  uint32_t unLen;
  uint8_t temp[MAX_LEN];

  mfrc522_write(MFRC522_BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

  temp[0] = PICC_ANTICOLL;
  temp[1] = 0x20;
  status = mfrc522_to_card(MFRC522_Transceive_CMD, temp, 2, temp, &unLen);

  if (status == CARD_FOUND)
  {
    //Check card serial number
    for (i=0; i<4; i++)
    {
      serNumCheck ^= temp[i];
    }
    if (serNumCheck != temp[i])
    {
      return 0;
    }

    *pUid = *(uint32_t*)temp;
    return 1;
  }
  return 0;
}
