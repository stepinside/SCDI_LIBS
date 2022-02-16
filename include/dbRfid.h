// ----------------------------------------------------------------------------
/// @file         dbRfid.h
/// @addtogroup   DBRFID_LIB   DB-RFID Library (libdbrfid.a, dbrfid.h)
/// @{
/// @brief        The DB-RFID library provides functions to retrieve the id of
///               cards by the rfid-reader.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_RFID_H_
#define DB_RFID_H_

// ----------------------------------------------------------------------------
/// @brief			  the eeprom address, where data of registered cards are stored.
#define DB_RFID_EEPROM_ADDRESS 0x0200

#ifdef __cplusplus
extern "C" {
  #endif

  // ----------------------------------------------------------------------------
  /// @brief        Initializes the RFID-reader and the DB-RFID library
  /// @retval       0               the RFID-reader could not be found
  /// @retval       1               everything's fine
  // ----------------------------------------------------------------------------
  uint8_t dbRfid_init();

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the DB-RFID library got initialized
  /// @retval       0               the library was not initialized
  /// @retval       1               the library was initialized
  // ----------------------------------------------------------------------------
  uint8_t dbRfid_isInitialized();

  // ----------------------------------------------------------------------------
  /// @brief        gets the UID of a card
  /// @param[out]   uid             the retrieved uid
  /// @retval       0               no card could be found
  /// @retval       1               the uid of a card was read
  // ----------------------------------------------------------------------------
  uint8_t dbRfid_getUid(uint32_t* uid);

  // ----------------------------------------------------------------------------
  /// @brief        Registers a card with the given uid.
  /// @param[in]    uid             the uid to be registered
  /// @return                       the index of the registered card. If the return value
  ///                               is -1, the card could not be registered
  // ----------------------------------------------------------------------------
  int8_t dbRfid_registerCard(uint32_t uid);

  // ----------------------------------------------------------------------------
  /// @brief        Unregisters all registered cards.
  // ----------------------------------------------------------------------------
  void dbRfid_unregisterCards();

  // ----------------------------------------------------------------------------
  /// @brief        Get the index of a given UID
  /// @param[in]    uid             the uid to be checked
  /// @return                       the index of the registered card. If the return value
  ///                               is -1, the uid is unknown
  // ----------------------------------------------------------------------------
  int8_t dbRfid_getIndexOfUid(uint32_t uid);

  // ----------------------------------------------------------------------------
  /// @brief        Get the UID of a given index
  /// @param[in]    uid             the index to be checked
  /// @return                       the UID of the registered card. If the UID
  ///                               is 0, the index is invalid
  // ----------------------------------------------------------------------------
  uint32_t dbRfid_getUidOfIndex(uint8_t index);

  // ----------------------------------------------------------------------------
  /// @brief        Returns the number of registered cards.
  /// @return       the number of registered cards
  // ----------------------------------------------------------------------------
  uint8_t dbRfid_getNumberOfRegisteredCards();

  // ----------------------------------------------------------------------------
  /// @brief        Starts to continuously detect cards.
  /// @param[in]    time_ms               detection intervall; time_ms must be a multiple of the timebase's
  ///                                     basetime.
  /// @param[in]    cardDetectedCallback  function to be called, when a card was detected.
  // ----------------------------------------------------------------------------
  void dbRfid_startContinuousDetection(uint16_t time_ms, void (*cardDetectedCallback)(uint32_t uid));

  // ----------------------------------------------------------------------------
  /// @brief        Stops to continuously detect cards.
  // ----------------------------------------------------------------------------
  void dbRfid_stopContinuousDetection();

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the system continuously detects cards.
  // ----------------------------------------------------------------------------
  uint8_t dbRfid_doesContinuouslyDetect();

  #ifdef __cplusplus
};
#endif


#endif /* DB_RFID_H_ */
/// @}