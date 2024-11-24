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
constexpr int BENCHLAB_VENDOR_ID = 0xEE;

/// <summary>
/// The product ID we expect to get from the device when retrieving the vendor
/// data.
/// </summary>
constexpr int BENCHLAB_PRODUCT_ID = 0x10;
constexpr int BENCHLAB_FIRMWARE_VERSION = 0xFF;

constexpr int BENCHLAB_FAN_PROFILES = 3;
constexpr int BENCHLAB_FANS = 9;
constexpr int BENCHLAB_FAN_CURVE_POINTS = 2;

constexpr int BENCHLAB_TEMPERATURE_SENSORS = 4;

constexpr int BENCHLAB_VIN_SENSORS = 13;

constexpr int BENCHLAB_POWER_SENSORS = 11;

constexpr int BENCHLAB_RGB_PROFILES = 2;

#else /* defined(__cplusplus) */
#define BENCHLAB_VENDOR_ID ((int) 0xEE)
#define BENCHLAB_PRODUCT_ID ((int) 0x10)
#define BENCHLAB_FIRMWARE_VERSION ((int) 0xFF)

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

//readonly string[] PowerSensorNames = { "EPS1", "EPS2", "ATX3V", "ATX5V", "ATX5VSB", "ATX12V", "PCIE1", "PCIE2", "PCIE3", "HPWR1", "HPWR2" };
