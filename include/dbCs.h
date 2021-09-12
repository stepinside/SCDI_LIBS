// ----------------------------------------------------------------------------
/// @file         dbCs.h
/// @addtogroup   DBCS_LIB   DB-CS Library (libdbcs.a, dbcs.h)
/// @{
/// @brief        The DB-CS library provides functions to detect colors by the color sensors
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_CS_H_
#define DB_CS_H_

// ----------------------------------------------------------------------------
/// @brief			  a structure for the red(=r), green(=g) and blue(=b) component of a color

struct DbCsColor
{
    uint8_t r;
    uint8_t b;
    uint8_t c;
    uint8_t g;
};

struct DbCsColors
{
    struct DbCsColor left;
    struct DbCsColor middle;
    struct DbCsColor right;
};

struct DbCsColorIndexes
{
    int8_t left;
    int8_t middle;
    int8_t right;
};

struct DbCsColorIndexesQuality
{
    uint8_t left;
    uint8_t middle;
    uint8_t right;
};

// ----------------------------------------------------------------------------
/// @brief			  the eeprom address, where data for the color detection are stored.
#define DB_CS_EEPROM_ADDRESS 0x0100

#define DB_CS_DATA_SIZE 12
#define DB_CS_MAX_COLORS 8

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the color sensors and DB-CS library
    // ----------------------------------------------------------------------------
    void dbCs_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the DB-CS library got initialized
    /// @retval       0               the library was not initialized
    /// @retval       1               the library was initialized
    // ----------------------------------------------------------------------------
    uint8_t dbCs_isInitialized();

    // ----------------------------------------------------------------------------
    /// @brief        Turns the white LEDs on/off
    /// @param[in]    onOff           0: turns LEDs off
    ///                               1: turns LEDs on
    // ----------------------------------------------------------------------------
    void dbCs_setLeds(uint8_t onOff);

    // ----------------------------------------------------------------------------
    /// @brief        Calibrates the sensors. When calling this function, the sensors must be placed
    ///               over a white sheet of paper. All colors that were already registered
    ///               are deleted.
    /// @param[in]    readyCallback   function to be called when the sensors got calibrated
    // ----------------------------------------------------------------------------
    void dbCs_calibrate(void (*readyCallback)(uint16_t calibrationData[DB_CS_DATA_SIZE]));

    // ----------------------------------------------------------------------------
    /// @brief        Registers the color that is just detected by the color sensors.
    /// @param[in]    registeredCallback    function to be called once the colors have been
    ///                                     detected and stored. The function will be called
    ///                                     with the colorIndex of the new color and its rgb values.
    ///                                     if registeredCallback is NULL, the no function will
    ///                                     be called, when a color got registered
    /// @retval       0               the color cannot be registered
    /// @retval       1               the color will be registered
    // ----------------------------------------------------------------------------
    uint8_t dbCs_registerColor(void (*readyCallback)(int8_t colorIndex, const struct DbCsColors *colors));

    // ----------------------------------------------------------------------------
    /// @brief        Returns the number of registered colors.
    /// @return       the number of registered colors
    // ----------------------------------------------------------------------------
    uint8_t dbCs_getNumberOfColors();

    // ----------------------------------------------------------------------------
    /// @brief        Retrieves the rgb-values of a registered color.
    /// @param[in]    index       the index of the registered color
    /// @param[in]    colors      the registered rgb values of the given index
    // ----------------------------------------------------------------------------
    uint8_t dbCs_getColorOfIndex(uint8_t index, const struct DbCsColors **colors);

    // ----------------------------------------------------------------------------
    /// @brief        Retrieves the color indexes and rgb-values of the current colors.
    /// @param[in]    readyCallback     function to be called once the color detection is done
    ///                                 if readyCallback is NULL, then no function will be called,
    ///                                 when a color got detected
    /// @param[in]    changedCallback   function to be called when the detected color has changed
    ///                                 if changedCallback is NULL, then no function will be called,
    ///                                 when the colors changed
    // ----------------------------------------------------------------------------
    void dbCs_getColorIndexes(void (*readyCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality), void (*changedCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality));

    // ----------------------------------------------------------------------------
    /// @brief        Sets the minimal quality a color must have to be recognized as a new color
    ///               compared to the last comparison.
    /// @param[in]    minQuality        the new minimum quality
    // ----------------------------------------------------------------------------
    void dbCs_setMinColorIndexesQuality(uint8_t minQuality);

    // ----------------------------------------------------------------------------
    /// @brief        Starts to continuously detect the colors.
    /// @param[in]    time_ms           measurement intervall; time_ms must be a multiple of the timebase's
    ///                                 basetime; the minimal period is 100ms
    /// @param[in]    readyCallback     function to be called once a single color detection is done
    ///                                 if readyCallback is NULL, then no function will be called
    /// @param[in]    changedCallback   function to be called once detected color has changed
    ///                                 if changedCallback is NULL, then no function will be called
    // ----------------------------------------------------------------------------
    void dbCs_startContinuousMeasurements(uint16_t time_ms, void (*changedCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality));

    // ----------------------------------------------------------------------------
    /// @brief        Stops to continuously detect the colors.
    // ----------------------------------------------------------------------------
    void dbCs_stopContinuousMeasurements();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the system continuously measure the colors.
    // ----------------------------------------------------------------------------
    uint8_t dbCs_doesContinuouslyMeasure();

#ifdef __cplusplus
};
#endif

#endif /* DB_CS_H_ */
       /// @}