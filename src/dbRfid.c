#include <avr/io.h>
#include <string.h>

#include <mfrc522.h>
#include <tb.h>
#include <uart.h>
#include <eeprom.h>

#include "dbRfid.h"

#define DB_RFID_MAX_CARD_NO    8

static volatile uint8_t _dbRfid_handle = 0;
static void (*_dbRfid_cardDetectedCallback)(uint32_t uid);
static uint16_t _dbRfid_continuousDetection();
static uint16_t _dbRfid_intervalTime_ms;
static uint32_t _dbRfid_uid = 0;
static uint8_t _dbRfid_initialized = 0;
static uint16_t _dbRfid_eepromAddress;
static uint8_t _dbRfid_cardNo = 0;
static uint32_t _dbRfid_uids[DB_RFID_MAX_CARD_NO];

uint8_t dbRfid_init()
{
  if (_dbRfid_initialized)    return 0;
  _dbRfid_initialized = 1;

  _dbRfid_eepromAddress = DB_RFID_EEPROM_ADDRESS;

  mfrc522_init(&PORTA, 7);

  // read information from the eeprom
  _dbRfid_cardNo = eeprom_read(_dbRfid_eepromAddress);

  for (uint8_t i=0; i<DB_RFID_MAX_CARD_NO; i++)
  {
    _dbRfid_uids[i]  = (uint32_t)eeprom_read(_dbRfid_eepromAddress+1+i*4+0) << 24;
    _dbRfid_uids[i] |= (uint32_t)eeprom_read(_dbRfid_eepromAddress+1+i*4+1) << 16;
    _dbRfid_uids[i] |= (uint32_t)eeprom_read(_dbRfid_eepromAddress+1+i*4+2) << 8;
    _dbRfid_uids[i] |= (uint32_t)eeprom_read(_dbRfid_eepromAddress+1+i*4+3) << 0;
  }

  return mfrc522_getVersion();
}

uint8_t dbRfid_isInitialized()
{
  return _dbRfid_initialized;
}

uint8_t dbRfid_getNumberOfRegisteredCards()
{
  return _dbRfid_cardNo;
}

// Stops the continuous measurement of distances
void dbRfid_stopContinuousDetection()
{
  if (!_dbRfid_initialized)
  {
    uart0_msg("dbRfid_stopContinuousDetection: dbRfid_init missing\n");
    return;
  }

  if (_dbRfid_handle != 0)
  {
    tb_unregister(_dbRfid_handle);
    _dbRfid_handle = 0;
  }
}

// stops continuous measurements
uint8_t dbRfid_getUid(uint32_t* uid)
{
  if (!_dbRfid_initialized)
  {
    uart0_msg("dbRfid_getUid: dbRfid_init missing\n");
    return 0;
  }

  // detectCard must be called twice - don't know why
  if (!mfrc522_detectCard() && !mfrc522_detectCard())
  {
    return 0;
  }
  if (!mfrc522_getUID(uid))
  {
    return 0;
  }
  return 1;
}

void dbRfid_unregisterCards()
{
  if (!_dbRfid_initialized)
  {
    uart0_msg("dbRfid_unregisterCards: dbRfid_init missing\n");
    return;
  }

  _dbRfid_cardNo = 0;
  eeprom_write(_dbRfid_eepromAddress, _dbRfid_cardNo);
}

int8_t dbRfid_registerCard(uint32_t uid)
{
  if (!_dbRfid_initialized)
  {
    uart0_msg("dbRfid_registerCard: dbRfid_init missing\n");
    return -1;
  }

  if (_dbRfid_cardNo >= DB_RFID_MAX_CARD_NO)
  {
    return -1;
  }

  if (dbRfid_getIndexOfUid(uid) < 0)
  {
    _dbRfid_uids[_dbRfid_cardNo] = uid;
    eeprom_write(_dbRfid_eepromAddress+1+_dbRfid_cardNo * 4 + 0, uid >> 24);
    eeprom_write(_dbRfid_eepromAddress+1+_dbRfid_cardNo * 4 + 1, uid >> 16);
    eeprom_write(_dbRfid_eepromAddress+1+_dbRfid_cardNo * 4 + 2, uid >> 8);
    eeprom_write(_dbRfid_eepromAddress+1+_dbRfid_cardNo * 4 + 3, uid >> 0);
    _dbRfid_cardNo++;
    eeprom_write(_dbRfid_eepromAddress, _dbRfid_cardNo);
    return _dbRfid_cardNo-1;
  }
  return -1;
}

int8_t dbRfid_getIndexOfUid(uint32_t uid)
{
  uint8_t i;
  for (i=0; i<_dbRfid_cardNo; i++)
  {
    if (_dbRfid_uids[i] == uid)
    {
      return i;
    }
  }
  return -1;
}

uint32_t dbRfid_getUidOfIndex(uint8_t index)
{
  if (index >= _dbRfid_cardNo)
    return 0;
  else
    return _dbRfid_uids[index-1];
}

uint16_t _dbRfid_continuousDetection()
{
  uint32_t uid;

  if (dbRfid_getUid(&uid))
  {
    if (_dbRfid_uid != uid)
    {
      _dbRfid_uid = uid;
      (*_dbRfid_cardDetectedCallback)(uid);
    }
  }
  else
  {
    _dbRfid_uid = 0;
  }
  return _dbRfid_intervalTime_ms;
}

// Starts the continuous detection of cards
void dbRfid_startContinuousDetection(uint16_t time_ms, void (*cardDetectedCallback)(uint32_t uid))
{
  if (!_dbRfid_initialized)
  {
    uart0_msg("dbRfid_startContinuousDetection: dbRfid_init missing\n");
    return;
  }

  _dbRfid_cardDetectedCallback = cardDetectedCallback;
  _dbRfid_intervalTime_ms = time_ms;

  dbRfid_stopContinuousDetection();
  _dbRfid_handle = tb_register(_dbRfid_continuousDetection, _dbRfid_intervalTime_ms);
  if (!_dbRfid_handle)
  {
    uart0_msg("dbRfid_startContinuousDetection: could not register tb-callback\n");
  }
};

uint8_t dbRfid_doesContinuouslyDetect()
{
  return (_dbRfid_handle != 0);
}

