// ----------------------------------------------------------------------------
/// @file         dbLs.h
/// @addtogroup   DBLS_LIB   DB-LS Library (libdbls.a, dbls.h)
/// @{
/// @brief        The DB-LS library provides functions to control the loudspeaker
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_LS_H_
#define DB_LS_H_

#include <avr/io.h>
#ifndef F_CPU
  #define F_CPU 16000000
#endif
#include <stdlib.h>

// ----------------------------------------------------------------------------
/// @brief			  used to create tones of a certain pitch
enum DbLsPitch
{
  DB_LS_LAST    = 0,              ///< used to indicate the last tone of a song
  DB_LS_PAUSE   = 1,              ///< used for a pause
  DB_LS_C2      = (F_CPU/65),
  DB_LS_D2      = (F_CPU/73),
  DB_LS_E2      = (F_CPU/82),
  DB_LS_F2      = (F_CPU/87),
  DB_LS_G2      = (F_CPU/98),
  DB_LS_A2      = (F_CPU/110),
  DB_LS_B2      = (F_CPU/123),
  DB_LS_C3      = (F_CPU/131),
  DB_LS_D3      = (F_CPU/147),
  DB_LS_E3      = (F_CPU/165),
  DB_LS_F3      = (F_CPU/174),
  DB_LS_G3      = (F_CPU/196),
  DB_LS_A3      = (F_CPU/220),
  DB_LS_B3      = (F_CPU/247),
  DB_LS_C4      = (F_CPU/261),
  DB_LS_D4      = (F_CPU/294),
  DB_LS_E4      = (F_CPU/330),
  DB_LS_F4      = (F_CPU/349),
  DB_LS_G4      = (F_CPU/392),
  DB_LS_A4      = (F_CPU/440),
  DB_LS_H4      = (F_CPU/493),
  DB_LS_C5      = (F_CPU/523),
  DB_LS_D5      = (F_CPU/587),
  DB_LS_E5      = (F_CPU/659),
  DB_LS_F5      = (F_CPU/698),
  DB_LS_G5      = (F_CPU/784),
  DB_LS_A5      = (F_CPU/880),
  DB_LS_H5      = (F_CPU/988),
  DB_LS_C6      = (F_CPU/1047),
  DB_LS_D6      = (F_CPU/1175),
  DB_LS_E6      = (F_CPU/1319),
  DB_LS_F6      = (F_CPU/1397),
  DB_LS_G6      = (F_CPU/1568),
  DB_LS_A6      = (F_CPU/1760),
  DB_LS_H6      = (F_CPU/1976),
  DB_LS_C7      = (F_CPU/2093),
  DB_LS_D7      = (F_CPU/2349),
  DB_LS_E7      = (F_CPU/2637),
  DB_LS_F7      = (F_CPU/2794),
  DB_LS_G7      = (F_CPU/3136),
  DB_LS_A7      = (F_CPU/3520),
  DB_LS_H7      = (F_CPU/3951),

};

// ----------------------------------------------------------------------------
/// @brief			  used to define the length of tones
enum DbLsLength
{
  DB_LS_1_1 = 47,       ///< whole note
  DB_LS_1_2 = 23,       ///< half note
  DB_LS_1_4 = 11,        ///< quarter note
  DB_LS_1_8 = 5,        ///< eighth note
  DB_LS_1_16 = 2,       ///< sixteenth note
  DB_LS_10_SEC = 200,   ///< ten second
  DB_LS_5_SEC = 100,    ///< five second
  DB_LS_3_SEC = 60,     ///< three second
  DB_LS_2_SEC = 40,     ///< two second
  DB_LS_1_SEC = 20     ///< one second
};

// ----------------------------------------------------------------------------
/// @brief			  used to declare a single note
struct DbLsNote
{
  enum DbLsPitch      pitch;      ///< the pitch of the note
  enum DbLsLength     length;     ///< the length of the note
};

// ----------------------------------------------------------------------------
/// @brief			  used to declare different ways to play a song
enum DbLsPlayMode
{
  DB_LS_REGULAR   = 0,        ///< play the song a regular way
  DB_LS_STACCATO1 = 1,        ///< play the song a staccato way
  DB_LS_STACCATO2 = 2,
  DB_LS_STACCATO3 = 3,
  DB_LS_SMOOTH    = 10        ///< play the song smoothly
};

extern struct DbLsNote dbLs_song[];
extern struct DbLsNote dbLs_siren[];
extern struct DbLsNote dbLs_beep[];
extern struct DbLsNote dbLs_beepBeep[];

#ifdef __cplusplus
extern "C" {
  #endif

  // ----------------------------------------------------------------------------
  /// @brief        Initializes the DB-LS library
  // ----------------------------------------------------------------------------
  void dbLs_init();

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the DB-LS library got initialized
  /// @retval       0               the library was not initialized
  /// @retval       1               the library was initialized
  // ----------------------------------------------------------------------------
  uint8_t dbLs_isInitialized();

  // ----------------------------------------------------------------------------
  /// @brief        Play a tone with the given pitch
  /// @param[in]    tone            the tone to be played
  // ----------------------------------------------------------------------------
  void dbLs_buzzerOn(enum DbLsPitch tone);

  // ----------------------------------------------------------------------------
  /// @brief        Stop playing the tone
  // ----------------------------------------------------------------------------
  void dbLs_buzzerOff();

  // ----------------------------------------------------------------------------
  /// @brief        Play a song
  /// @param[in]    pNotes          the notes of the song. There are following predefined
  ///                               songs: dbLsSong1, dbLsSirene, dbLsBeep
  /// @param[in]    playMode        the mode the song shall be played
  /// @param[in]    readyCallback   function to be called when the song's over. The return
  ///                               value of the callback function determines whether the song
  ///                               shall be played once more (return value>0) or not (return
  ///                               value=0). If readyCallback == NULL, the song will not be
  ///                               repeated.
  // ----------------------------------------------------------------------------
  void dbLs_playSong(struct DbLsNote *pNotes, enum DbLsPlayMode playMode, uint8_t (*readyCallback)());

  // ----------------------------------------------------------------------------
  /// @brief        Gets the notes of the currently played song
  /// @retval       NULL            no song is currently played
  /// @retval       !NULL           the notes of the currently played song
  // ----------------------------------------------------------------------------
  struct DbLsNote* dbLs_getSong();

  // ----------------------------------------------------------------------------
  /// @brief        Stop playing the song
  // ----------------------------------------------------------------------------
  void dbLs_stopSong();

  #ifdef __cplusplus
};
#endif


#endif /* DB_LS_H_ */
/// @}
