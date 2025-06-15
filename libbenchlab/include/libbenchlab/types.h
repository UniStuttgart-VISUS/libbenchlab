// <copyright file="types.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(__cplusplus)
#include <cinttypes>
#endif /* defined(__cplusplus) */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#if defined(_WIN32)
#include <sal.h>
#include <Windows.h>
#endif /* defined(_WIN32) */

#include "libbenchlab/api.h"
#include "libbenchlab/constants.h"
#include "libbenchlab/timestamp.h"

#if !defined(_WIN32)
#include "libbenchlab/nocom.h"
#include "libbenchlab/nosal.h"
#endif /* !defined(_WIN32) */


/// <summary>
/// The character type uses in strings in the API.
/// </summary>
#if defined(_WIN32)
typedef wchar_t benchlab_char;
#define BENCHLAB_STR(s) L##s
#else /* defined(_WIN32) */
typedef char benchlab_char;
#define BENCHLAB_STR(s) s
#endif /* defined(_WIN32) */


// LIBBENCHLAB_ENUM enables us to treat our enumerations as enum classes in C++
// and as "normal" enums in C. LIBBENCHLAB_ENUM_BEGIN enables us to define
// enumerations that have a specific underlying type that is required by the
// hardware.
#if defined(__cplusplus)
#define LIBBENCHLAB_ENUM class
#define LIBBENCHLAB_ENUM_SCOPE(scope, field) field
#define LIBBENCHLAB_ENUM_BEGIN(name, type) enum class name : std::type {
#define LIBBENCHLAB_ENUM_END() };
#else /* defined(__cplusplus) */
#define LIBBENCHLAB_ENUM
#define LIBBENCHLAB_ENUM_SCOPE(scope, field) scope##_##field
#define LIBBENCHLAB_ENUM_BEGIN(name, type) typedef type name;
#define LIBBENCHLAB_ENUM_END()
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


/// <summary>
/// The type of the unique device ID.
/// </summary>
#if defined(_WIN32)
typedef GUID benchlab_device_uid_type;
#else /* defined(_WIN32) */
typedef struct benchlab_device_uid_type_t {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
} benchlab_device_uid_type;
#endif /* defined(_WIN32) */

/// <summary>
/// Defines the buttons that can be pressed via software.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_button, uint8_t)
#if defined(__cplusplus)
    power = 0,
    reset = 1,
    other = 2
#else /* defined(__cplusplus) */
#define benchlab_button_power ((benchlab_button) 0)
#define benchlab_button_reset ((benchlab_button) 1)
#define benchlab_button_other ((benchlab_button) 2)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// Defines possible fan control modes.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_fan_mode, uint8_t)
#if defined(__cplusplus)
    temperature_control = 0,
    fixed = 1,
    extended = 2,
#else /* defined(__cplusplus) */
#define benchlab_fan_mode_temperature_control ((benchlab_fan_mode) 0)
#define benchlab_fan_mode_fixed ((benchlab_fan_mode) 1)
#define benchlab_fan_mode_extended ((benchlab_fan_mode) 2)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// Defines possible fan states.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_fan_stop, uint8_t)
#if defined(__cplusplus)
    off = 0,
    on
#else /* defined(__cplusplus) */
#define benchlab_fan_stop_off ((benchlab_fan_stop) 0)
#define benchlab_fan_stop_on ((benchlab_fan_stop) 1)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// What is this?
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_fan_switch_status, uint8_t)
#if defined(__cplusplus)
    automatic = 0,
    half,
    full,
#else /* defined(__cplusplus) */
#define benchlab_fan_switch_status uint8_t
#define benchlab_fan_switch_status_automatic ((benchlab_fan_switch_status) 0)
#define benchlab_fan_switch_status_half ((benchlab_fan_switch_status) 1)
#define benchlab_fan_switch_status_full ((benchlab_fan_switch_status) 2)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// Defines the directions of the LED effects.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_rgb_direction, uint8_t)
#if defined(__cplusplus)
    clockwise = 0,
    anti_clockwise
#else /* defined(__cplusplus) */
#define benchlab_rgb_direction_clockwise ((benchlab_rgb_direction) 0)
#define benchlab_rgb_direction_anti_clockwise ((benchlab_rgb_direction) 1)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// Defines possible exteded LED status.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_rgb_extended_status, uint8_t)
#if defined(__cplusplus)
    not_detected = 0,
    detected
#else /* defined(__cplusplus) */
#define benchlab_rgb_extended_status_not_detected ((benchlab_rgb_extended_status) 0)
#define benchlab_rgb_extended_status_detected ((benchlab_rgb_extended_status) 1)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// Defines the LED effects that can be used with the Benchlab device.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_rgb_mode, uint8_t)
#if defined(__cplusplus)
    rainbow_cycle = 0,
    rainbow_colour_chase,
    rainbow,
    twinkle,
    meteor_shower,
    colour_wipe,
    theatre_chase,
    fade_in_out,
    single_colour_chase,
    single_colour
#else /* defined(__cplusplus) */
#define benchlab_rgb_mode_rainbow_cycle ((benchlab_rgb_mode) 0)
#define benchlab_rgb_mode_rainbow_colour_chase ((benchlab_rgb_mode) 1)
#define benchlab_rgb_mode_rainbow ((benchlab_rgb_mode) 2)
#define benchlab_rgb_mode_twinkle ((benchlab_rgb_mode) 3)
#define benchlab_rgb_mode_meteor_shower ((benchlab_rgb_mode) 4)
#define benchlab_rgb_mode_colour_wipe ((benchlab_rgb_mode) 5)
#define benchlab_rgb_mode_theatre_chase ((benchlab_rgb_mode) 6)
#define benchlab_rgb_mode_fade_in_out ((benchlab_rgb_mode) 7)
#define benchlab_rgb_mode_single_colour_chase ((benchlab_rgb_mode) 8)
#define benchlab_rgb_mode_single_colour ((benchlab_rgb_mode) 9)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()


/// <summary>
/// Defines the LED mode.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_rgb_switch_status, uint8_t)
#if defined(__cplusplus)
    work = 0,
    play
#else /* defined(__cplusplus) */
#define benchlab_rgb_switch_status_work ((benchlab_rgb_switch_status) 0)
#define benchlab_rgb_switch_status_play ((benchlab_rgb_switch_status) 1)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()

/// <summary>
/// Defines the temperature sensors on the board.
/// </summary>
LIBBENCHLAB_ENUM_BEGIN(benchlab_temperature_source, uint8_t)
#if defined(__cplusplus)
    automatic = 0,
    sensor1,
    sensor2,
    sensor3,
    sensor4,
    ambient
#else /* defined(__cplusplus) */
#define benchlab_temperature_source_automatic ((benchlab_temperature_source) 0)
#define benchlab_temperature_source_sensor1 ((benchlab_temperature_source) 1)
#define benchlab_temperature_source_sensor2 ((benchlab_temperature_source) 2)
#define benchlab_temperature_source_sensor3 ((benchlab_temperature_source) 3)
#define benchlab_temperature_source_sensor4 ((benchlab_temperature_source) 4)
#define benchlab_temperature_source_ambient ((benchlab_temperature_source) 5)
#endif /* defined(__cplusplus) */
LIBBENCHLAB_ENUM_END()

//
//public enum TS_B : byte {
//    TS_B_3950,
//    TS_B_3380
//};


/// <summary>
/// Configures the behaviour of the fans controlled by the Benchlab device.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_fan_config_t {
    benchlab_fan_mode fan_mode;
    benchlab_temperature_source temperature_source;
    int16_t temperature[BENCHLAB_FAN_CURVE_POINTS];
    int16_t duty[BENCHLAB_FAN_CURVE_POINTS];
    uint8_t ramp_step;
    uint8_t fixed_duty;
    uint8_t min_duty;
    uint8_t max_duty;
    benchlab_fan_stop fan_stop;
} benchlab_fan_config;


/// <summary>
/// Receives the raw readings of a single fan sensor.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_fan_reading_t {
    uint8_t enable;
    uint8_t duty;
    uint16_t tach;
} benchlab_fan_reading;


/// <summary>
/// Receives the raw readings of a single power sensor.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_power_reading_t {
    int16_t voltage;
    int32_t current;
    int32_t power;
} benchlab_power_reading;


/// <summary>
/// Configures the behaviour of the RGB LEDs at the bottom of the Benchlab
/// device.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_rgb_config_t {
    benchlab_rgb_mode mode;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    benchlab_rgb_direction direction;
    uint8_t speed;
} benchlab_rgb_config;


/// <summary>
/// Specifies the raw sensor readings obtained at once from a Benchlab device.
/// </summary>
/// <remarks>
/// The structure is defined to map the exact memory layout of what we read from
/// the hardware. Therefore, the data in the fields might be reinterpreted to
/// make sense from them. It is not recommended using this structure directly in
/// client applications.
/// </remarks>
typedef struct LIBBENCHLAB_API benchlab_sensor_readings_t {
    int16_t vin[BENCHLAB_VIN_SENSORS];
    uint16_t vdd;
    uint16_t vref;
    int16_t tchip;
    int16_t ts[BENCHLAB_TEMPERATURE_SENSORS];
    int16_t tamb;
    uint16_t hum;
    benchlab_fan_switch_status fan_switch;
    benchlab_rgb_switch_status rgb_switch;
    benchlab_rgb_extended_status rgb_extended_status;
    uint8_t external_fan_duty;
    benchlab_power_reading power_readings[BENCHLAB_POWER_SENSORS];
    benchlab_fan_reading fans[BENCHLAB_FANS];
} benchlab_sensor_readings;


/// <summary>
/// Represents the post-processed sensor readings derived from
/// <see cref="benchlab_sensor_readings" />.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_sample_t {
    benchlab_timestamp timestamp;
    float input_voltage[BENCHLAB_VIN_SENSORS];
    float supply_voltage;
    float reference_voltage;
    float chip_temperature;
    float temperatures[BENCHLAB_TEMPERATURE_SENSORS];
    float ambient_temperature;
    float humidity;
    uint8_t external_fan_duty;
    float voltages[BENCHLAB_POWER_SENSORS];
    float currents[BENCHLAB_POWER_SENSORS];
    float power[BENCHLAB_POWER_SENSORS];
    uint16_t fan_speeds[BENCHLAB_FANS];
    uint8_t fan_duties[BENCHLAB_FANS];
} benchlab_sample;


/// <summary>
/// The callback to be invoked when a new sample arrives.
/// </summary>
typedef void (*benchlab_sample_callback)(
    _In_ benchlab_handle source,
    _In_ const benchlab_sample *sample,
    _In_opt_ void *context);
