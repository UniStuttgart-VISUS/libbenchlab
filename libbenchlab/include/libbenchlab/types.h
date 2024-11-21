// <copyright file="types.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#if defined(_WIN32)
#include <sal.h>
#include <Windows.h>
#endif /* defined(_WIN32) */

#include "libbenchlab/api.h"
#include "libbenchlab/constants.h"

#if !defined(_WIN32)
#include "libbenchlab/linuxhresult.h"
#endif /* !defined(_WIN32) */


/// <summary>
/// The character type uses in strings in the API.
/// </summary>
#if defined(_WIN32)
typedef wchar_t benchlab_char;
#else /* defined(_WIN32) */
typedef char benchlab_char;
#endif /* defined(_WIN32) */


// Enables us to treat our enumerations as enum classes in C++ and as "normal"
// enums in C.
#if defined(__cplusplus)
#define LIBBENCHLAB_ENUM class
#define LIBBENCHLAB_ENUM_SCOPE(scope, field) field
#else /* defined(__cplusplus) */
#define LIBBENCHLAB_ENUM
#define LIBBENCHLAB_ENUM_SCOPE(scope, field) scope##_##field
#endif /* defined(__cplusplus) */


/// <summary>
/// The opaque type used to represent a Benchlab device.
/// </summary>
/// <remarks>
/// This is a forward declaration of the internal type representing the
/// connection to a device. Callers must not make any assumptions about the
/// internal memory layout of this type.
/// </remarks>
struct benchlab_device;


/// <summary>
/// The handle to a Benchlab device.
/// </summary>
/// <remarks>
/// <c>nullptr</c> is used to represent an invalid handle.
/// </remarks>
typedef struct benchlab_device *benchlab_handle;


#if defined(__cplusplus)
enum class fan_mode : uint8_t {
    temperature_control = 0,
    fixed,
    extended,
};

#else /* defined(__cplusplus) */
#define fan_mode uint8_t
#define fan_mode_temperature_control ((fan_mode) 0);
#define fan_mode_fixed ((fan_mode) 1);
#define fan_mode_extended ((fan_mode) 2);
#endif /* defined(__cplusplus) */


//typedef enum LIBBENCHLAB_ENUM fan_stop_t : uint8_t {
//    LIBBENCHLAB_ENUM_SCOPE(fan_stop, off) = 0,
//    LIBBENCHLAB_ENUM_SCOPE(fan_stop, on),
//} fan_stop;


#if defined(__cplusplus)
enum class fan_switch_status : uint8_t {
    automatic = 0,
    half,
    full,
};

#else /* defined(__cplusplus) */
#define fan_switch_status uint8_t
#define fan_switch_status_automatic ((fan_switch_status) 0);
#define fan_switch_status_half ((fan_switch_status) 1);
#define fan_switch_status_full ((fan_switch_status) 2);
#endif /* defined(__cplusplus) */


#if defined(__cplusplus)
enum class rgb_switch_status : uint8_t {
    work = 0,
    play
};

#else /* defined(__cplusplus) */
#define rgb_switch_status uint8_t
#define rgb_switch_status_work ((rgb_switch_status) 0);
#define rgb_switch_status_play ((rgb_switch_status) 1);
#endif /* defined(__cplusplus) */


#if defined(__cplusplus)
enum class rgb_extended_status : uint8_t {
    not_detected = 0,
    detected
};

#else /* defined(__cplusplus) */
#define rgb_extended_status uint8_t
#define rgb_extended_status_not_detected ((rgb_extended_status) 0);
#define rgb_extended_status_detected ((rgb_extended_status) 1);
#endif /* defined(__cplusplus) */


//public enum TEMP_SRC : byte {
//    TEMP_SRC_AUTO,
//    TEMP_SRC_TS1,
//    TEMP_SRC_TS2,
//    TEMP_SRC_TS3,
//    TEMP_SRC_TS4,
//    TEMP_SRC_TAMB
//};
//
//
//
//

//
//private enum UART_CMD : byte {
//    UART_CMD_WELCOME,
//    UART_CMD_READ_SENSORS,
//    UART_CMD_ACTION,
//    UART_CMD_READ_NAME,
//    UART_CMD_WRITE_NAME,
//    UART_CMD_READ_FAN_PROFILE,
//    UART_CMD_WRITE_FAN_PROFILE,
//    UART_CMD_READ_RGB,
//    UART_CMD_WRITE_RGB,
//    UART_CMD_READ_CALIBRATION,
//    UART_CMD_WRITE_CALIBRATION,
//    UART_CMD_LOAD_CALIBRATION,
//    UART_CMD_STORE_CALIBRATION,
//    UART_CMD_READ_UID,
//    UART_CMD_READ_VENDOR_DATA,
//}
//
//public enum RGB_MODE : byte {
//    RGB_MODE_RAINBOW_CYCLE,
//    RGB_MODE_RAINBOW_COLOR_CHASE,
//    RGB_MODE_RAINBOW,
//    RGB_MODE_TWINKLE,
//    RGB_MODE_METEOR_SHOWER,
//    RGB_MODE_COLOR_WIPE,
//    RGB_MODE_THEATRE_CHASE,
//    RGB_MODE_FADE_IN_OUT,
//    RGB_MODE_SINGLE_COLOR_CHASE,
//    RGB_MODE_SINGLE_COLOR
//};
//
//public enum RGB_DIRECTION : byte {
//    RGB_DIRECTION_CLOCKWISE,
//    RGB_DIRECTION_COUNTER_CLOCKWISE
//};
//
//public enum TS_B : byte {
//    TS_B_3950,
//    TS_B_3380
//};


/// <summary>
/// Receives the readings of a single power sensor.
/// </summary>
typedef struct bechlab_power_reading_t {
    int16_t voltage;
    int32_t current;
    int32_t power;
} bechlab_power_reading;


/// <summary>
/// Receives the readings of a single fan sensor.
/// </summary>
typedef struct benchlab_fan_reading_t {
    uint8_t enable;
    uint8_t duty;
    uint16_t tach;
} benchlab_fan_reading;


/// <summary>
/// Specifies the sensor readings obtained at once from a Benchlab device.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_sensor_readings_t {
    int16_t vin[BENCHLAB_VIN_SENSORS];
    uint16_t vdd;
    uint16_t vref;
    int16_t tchip;
    int16_t ts[BENCHLAB_TEMPERATURE_SENSORS];
    int16_t tamb;
    uint16_t hum;
    fan_switch_status fan_switch;
    rgb_switch_status rgb_switch;
    rgb_extended_status rgb_extended_status;
    uint8_t fan_extended_duty;
    bechlab_power_reading power_readings[BENCHLAB_POWER_SENSORS];
    benchlab_fan_reading fans[BENCHLAB_FANS];
} benchlab_sensor_readings;


struct powenetics_sample_t;

/// <summary>
/// The callback to be invoked when a new sample arrives.
/// </summary>
typedef void (*powenetics_data_callback)(_In_ benchlab_handle source,
    _In_ const struct powenetics_sample_t *sample, _In_opt_ void *context);
