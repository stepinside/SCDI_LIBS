#include "dbLs.h"

#include <stdlib.h>

#include <tb.h>
#include <uart.h>

#define LOUDSPEAKER (1 << 6) // the loudspeaker is connected to pinH.6

static void _dbLs_setPitch(enum DbLsPitch pitch);

struct DbLsNote dbLs_song[] = {{DB_LS_C4, DB_LS_1_8}, {DB_LS_D4, DB_LS_1_8}, {DB_LS_E4, DB_LS_1_4}, {DB_LS_E4, DB_LS_1_8}, {DB_LS_G4, DB_LS_1_8}, {DB_LS_F4, DB_LS_1_4}, {DB_LS_F4, DB_LS_1_8}, {DB_LS_A4, DB_LS_1_8}, {DB_LS_G4, DB_LS_1_4}, {DB_LS_G4, DB_LS_1_8}, {DB_LS_F4, DB_LS_1_8}, {DB_LS_E4, DB_LS_1_2}, {DB_LS_G4, DB_LS_1_4}, {DB_LS_G4, DB_LS_1_8}, {DB_LS_F4, DB_LS_1_8}, {DB_LS_E4, DB_LS_1_4}, {DB_LS_E4, DB_LS_1_8}, {DB_LS_G4, DB_LS_1_8}, {DB_LS_F4, DB_LS_1_4}, {DB_LS_F4, DB_LS_1_8}, {DB_LS_E4, DB_LS_1_8}, {DB_LS_D4, DB_LS_1_4}, {DB_LS_G4, DB_LS_1_4}, {DB_LS_E4, DB_LS_1_1}, {DB_LS_LAST, 0}};
struct DbLsNote dbLs_siren[] = {{DB_LS_C5, DB_LS_1_8}, {DB_LS_C4, DB_LS_1_8}, {DB_LS_LAST, 0}};
struct DbLsNote dbLs_beep[] = {{DB_LS_C6, DB_LS_1_8}, {DB_LS_LAST, 0}};
struct DbLsNote dbLs_beepBeep[] = {{DB_LS_C6, DB_LS_1_16}, {DB_LS_PAUSE, DB_LS_1_16}, {DB_LS_C6, DB_LS_1_16}, {DB_LS_LAST, 0}};

struct DbLsPlay
{
    enum DbLsPlayMode mode;
    struct DbLsNote *pNotes;
    uint16_t actNote;
    uint8_t (*readyCallback)();
    uint8_t callbackHandle;
    uint8_t toneLength;
    uint8_t actToneLength;
};
static struct DbLsPlay dbLsPlay;

static uint8_t _dbLs_initialized = 0;

void dbLs_init()
{
    if (_dbLs_initialized)
        return;
    _dbLs_initialized = 1;

    DDRH |= LOUDSPEAKER;

    TCCR2A = ((1 << COM2B0) | (1 << WGM21));
    TCCR2B = 0x0;
    TIMSK2 = 0;
}

uint8_t dbLs_isInitialized()
{
    return _dbLs_initialized;
}

void _dbLs_setPitch(enum DbLsPitch pitch)
{
    static uint32_t prescaler[] = {3, 5, 6, 7, 8, 10};
    uint8_t prescalerIndex;
    uint16_t ocrValue;

    prescalerIndex = 0;
    while ((ocrValue = ((uint32_t)pitch >> prescaler[prescalerIndex]) - 1) > 255)
    {
        prescalerIndex++;
    }
    OCR2B = ocrValue;
    OCR2A = ocrValue;
    switch (prescalerIndex)
    {
    case 0:
        TCCR2B = 0b010;
        break;
    case 1:
        TCCR2B = 0b011;
        break;
    case 2:
        TCCR2B = 0b100;
        break;
    case 3:
        TCCR2B = 0b101;
        break;
    case 4:
        TCCR2B = 0b110;
        break;
    case 5:
        TCCR2B = 0b111;
        break;
    }
    TCNT2 = 0;
}

void dbLs_buzzerOn(enum DbLsPitch pitch)
{
    if (!_dbLs_initialized)
    {
        uart0_msg("dbLs_buzzerOn: dbLs_init missing\n");
        return;
    }

    if (dbLsPlay.callbackHandle)
    {
        tb_stopTimeout(dbLsPlay.callbackHandle);
    }

    _dbLs_setPitch(pitch);
}

void dbLs_buzzerOff()
{
    if (!_dbLs_initialized)
    {
        uart0_msg("dbLs_buzzerOff: dbLs_init missing\n");
        return;
    }

    TCCR2B &= ~(0b111);
    if (dbLsPlay.callbackHandle)
    {
        tb_startTimeout(dbLsPlay.callbackHandle);
    }
}

uint16_t _dbLs_playCallback()
{
    dbLsPlay.actToneLength++;
    if ((dbLsPlay.mode == DB_LS_REGULAR && dbLsPlay.actToneLength == dbLsPlay.toneLength) || (dbLsPlay.mode != DB_LS_REGULAR && dbLsPlay.actToneLength == dbLsPlay.mode))
    {
        TCCR2B &= ~(0b111);
    }
    else if (dbLsPlay.actToneLength > dbLsPlay.toneLength)
    {
        dbLsPlay.actNote++;
        if (dbLsPlay.pNotes[dbLsPlay.actNote].pitch == DB_LS_LAST)
        {
            if (dbLsPlay.readyCallback && (*dbLsPlay.readyCallback)())
            {
                dbLsPlay.actNote = 0;
            }
            else
            {
                dbLsPlay.callbackHandle = 0;
                TCCR2B &= ~(0b111);
                return 0;
            }
        }
        if (dbLsPlay.pNotes[dbLsPlay.actNote].pitch == DB_LS_PAUSE)
        {
            TCCR2B &= ~(0b111);
        }
        else
        {
            _dbLs_setPitch(dbLsPlay.pNotes[dbLsPlay.actNote].pitch);
        }
        dbLsPlay.toneLength = dbLsPlay.pNotes[dbLsPlay.actNote].length;
        dbLsPlay.actToneLength = 0;
    }
    return 50;
}

void dbLs_playSong(struct DbLsNote *pNotes, enum DbLsPlayMode playMode, uint8_t (*readyCallback)())
{
    if (!_dbLs_initialized)
    {
        uart0_msg("dbLs_playSong: dbLs_init missing\n");
        return;
    }

    dbLs_stopSong();

    if (pNotes->pitch != DB_LS_LAST)
    {
        dbLsPlay.pNotes = pNotes;
        dbLsPlay.actNote = 0;
        dbLsPlay.readyCallback = readyCallback;
        dbLsPlay.mode = playMode;

        _dbLs_setPitch(dbLsPlay.pNotes[dbLsPlay.actNote].pitch);
        dbLsPlay.toneLength = dbLsPlay.pNotes[dbLsPlay.actNote].length;
        dbLsPlay.actToneLength = 0;
        dbLsPlay.callbackHandle = tb_register(_dbLs_playCallback, 50);
        if (!dbLsPlay.callbackHandle)
        {
            uart0_msg("dbLs_playSong: could not register tb-callback\n");
            return;
        }
    }
}

void dbLs_stopSong()
{
    if (!_dbLs_initialized)
    {
        uart0_msg("dbLs_stopSong: dbLs_init missing\n");
        return;
    }

    if (dbLsPlay.callbackHandle)
    {
        tb_unregister(dbLsPlay.callbackHandle);
        dbLsPlay.callbackHandle = 0;
    }
    TCCR2B &= ~(0b111);
    dbLsPlay.pNotes = NULL;
}

struct DbLsNote *dbLs_getSong()
{
    return dbLsPlay.pNotes;
}
