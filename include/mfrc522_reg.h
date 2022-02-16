#ifndef _MFRC522_REG_H
#define _MFRC522_REG_H

//Page 0 ==> Command and Status
#define MFRC522_Page0_Reserved_1 	0x00
#define MFRC522_CommandReg			  0x01
#define MFRC522_ComIEnReg			    0x02
#define MFRC522_DivIEnReg			    0x03
#define MFRC522_ComIrqReg			    0x04
#define MFRC522_DivIrqReg			    0x05
#define MFRC522_ErrorReg			    0x06
#define MFRC522_Status1Reg			  0x07
#define MFRC522_Status2Reg			  0x08
#define MFRC522_FIFODataReg			  0x09
#define MFRC522_FIFOLevelReg		  0x0A
#define MFRC522_WaterLevelReg		  0x0B
#define MFRC522_ControlReg			  0x0C
#define MFRC522_BitFramingReg		  0x0D
#define MFRC522_CollReg				    0x0E
#define MFRC522_Page0_Reserved_2	0x0F

//Page 1 ==> Command
#define MFRC522_Page1_Reserved_1	0x10
#define MFRC522_ModeReg				    0x11
#define MFRC522_TxModeReg			    0x12
#define MFRC522_RxModeReg			    0x13
#define MFRC522_TxControlReg		  0x14
#define MFRC522_TxASKReg			    0x15
#define MFRC522_TxSelReg			    0x16
#define MFRC522_RxSelReg			    0x17
#define MFRC522_RxThresholdReg		0x18
#define	MFRC522_DemodReg			    0x19
#define MFRC522_Page1_Reserved_2	0x1A
#define MFRC522_Page1_Reserved_3	0x1B
#define MFRC522_MfTxReg				    0x1C
#define MFRC522_MfRxReg				    0x1D
#define MFRC522_Page1_Reserved_4	0x1E
#define MFRC522_SerialSpeedReg		0x1F

//Page 2 ==> CFG
#define MFRC522_Page2_Reserved_1	0x20
#define MFRC522_CRCResultReg_1		0x21
#define MFRC522_CRCResultReg_2		0x22
#define MFRC522_Page2_Reserved_2	0x23
#define MFRC522_ModWidthReg			  0x24
#define MFRC522_Page2_Reserved_3	0x25
#define MFRC522_RFCfgReg			    0x26
#define MFRC522_GsNReg				    0x27
#define MFRC522_CWGsPReg			    0x28
#define MFRC522_ModGsPReg			    0x29
#define MFRC522_TModeReg			    0x2A
#define MFRC522_TPrescalerReg		  0x2B
#define MFRC522_TReloadReg_1		  0x2C
#define MFRC522_TReloadReg_2		  0x2D
#define MFRC522_TCounterValReg_1	0x2E
#define MFRC522_TCounterValReg_2 	0x2F

//Page 3 ==> TestRegister
#define MFRC522_Page3_Reserved_1 	0x30
#define MFRC522_TestSel1Reg			  0x31
#define MFRC522_TestSel2Reg			  0x32
#define MFRC522_TestPinEnReg		  0x33
#define MFRC522_TestPinValueReg		0x34
#define MFRC522_TestBusReg			  0x35
#define MFRC522_AutoTestReg			  0x36
#define MFRC522_VersionReg			  0x37
#define MFRC522_AnalogTestReg		  0x38
#define MFRC522_TestDAC1Reg			  0x39
#define MFRC522_TestDAC2Reg			  0x3A
#define MFRC522_TestADCReg			  0x3B
#define MFRC522_Page3_Reserved_2 	0x3C
#define MFRC522_Page3_Reserved_3	0x3D
#define MFRC522_Page3_Reserved_4	0x3E
#define MFRC522_Page3_Reserved_5	0x3F

#endif
