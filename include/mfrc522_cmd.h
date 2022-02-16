#ifndef MFRC522_CMD_H
#define MFRC522_CMD_H

//command set
#define MFRC522_Idle_CMD 				      0x00
#define MFRC522_Mem_CMD					      0x01
#define MFRC522_GenerateRandomId_CMD	0x02
#define MFRC522_CalcCRC_CMD				    0x03
#define MFRC522_Transmit_CMD			    0x04
#define MFRC522_NoCmdChange_CMD			  0x07
#define MFRC522_Receive_CMD				    0x08
#define MFRC522_Transceive_CMD			  0x0C
#define MFRC522_Reserved_CMD			    0x0D
#define MFRC522_MFAuthent_CMD			    0x0E
#define MFRC522_SoftReset_CMD			    0x0F

#endif
