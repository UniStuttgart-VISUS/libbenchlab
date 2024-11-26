// <copyright file="constants.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <inttypes.h>

#if defined(__cplusplus)
/// <summary>
/// The vendor ID we expect to get from the device when retrieving the
/// vendor data.
/// </summary>
constexpr auto BENCHLAB_VENDOR_ID = static_cast<uint8_t>(0xEE);

/// <summary>
/// The product ID we expect to get from the device when retrieving the vendor
/// data.
/// </summary>
constexpr auto BENCHLAB_PRODUCT_ID = static_cast<uint8_t>(0x10);

/// <summary>
/// The number of fan profiles the device supports.
/// </summary>
constexpr int BENCHLAB_FAN_PROFILES = 3;

/// <summary>
/// The number of fan connectors on the device.
/// </summary>
constexpr int BENCHLAB_FANS = 9;

/// <summary>
/// The number of control points of a fan curve.
/// </summary>
constexpr int BENCHLAB_FAN_CURVE_POINTS = 2;

/// <summary>
/// The number of temperature sensors on the device.
/// </summary>
constexpr int BENCHLAB_TEMPERATURE_SENSORS = 4;

/// <summary>
/// The number of input voltage sensors on the device.
/// </summary>
constexpr int BENCHLAB_VIN_SENSORS = 13;

/// <summary>
/// The number of power sensors on the device.
/// </summary>
constexpr int BENCHLAB_POWER_SENSORS = 11;

/// <summary>
/// The number of RGB profiles supported.
/// </summary>
constexpr int BENCHLAB_RGB_PROFILES = 2;

#else /* defined(__cplusplus) */
#define BENCHLAB_VENDOR_ID ((uint8_t) 0xEE)
#define BENCHLAB_PRODUCT_ID ((uint8_t) 0x10)
#define BENCHLAB_FAN_PROFILES ((int) 3)
#define BENCHLAB_FANS ((int) 9)
#define BENCHLAB_FAN_CURVE_POINTS ((int) 2)
#define BENCHLAB_TEMPERATURE_SENSORS ((int) 4)
#define BENCHLAB_VIN_SENSORS ((int) 13)
#define BENCHLAB_POWER_SENSORS ((int) 11)
#define BENCHLAB_RGB_PROFILES ((int) 2)
#endif /* defined(__cplusplus) */


//public const int CAL_NUM = 2;
//public const int CAL_FACTORY = 0;
//public const int CAL_USER = 1;


