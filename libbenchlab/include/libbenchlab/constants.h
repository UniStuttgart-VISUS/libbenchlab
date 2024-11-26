// <copyright file="constants.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once


#if defined(__cplusplus)
#include <cinttypes>
#include <cstddef>

/// <summary>
/// The vendor ID we expect to get from the device when retrieving the
/// vendor data.
/// </summary>
constexpr auto BENCHLAB_VENDOR_ID = static_cast<std::uint8_t>(0xEE);

/// <summary>
/// The product ID we expect to get from the device when retrieving the vendor
/// data.
/// </summary>
constexpr auto BENCHLAB_PRODUCT_ID = static_cast<std::uint8_t>(0x10);

/// <summary>
/// The number of fan profiles the device supports.
/// </summary>
constexpr std::size_t BENCHLAB_FAN_PROFILES = 3;

/// <summary>
/// The number of fan connectors on the device.
/// </summary>
constexpr std::size_t BENCHLAB_FANS = 9;

/// <summary>
/// The number of control points of a fan curve.
/// </summary>
constexpr std::size_t BENCHLAB_FAN_CURVE_POINTS = 2;

/// <summary>
/// The number of temperature sensors on the device.
/// </summary>
constexpr std::size_t BENCHLAB_TEMPERATURE_SENSORS = 4;

/// <summary>
/// The number of input voltage sensors on the device.
/// </summary>
constexpr std::size_t BENCHLAB_VIN_SENSORS = 13;

/// <summary>
/// The number of power sensors on the device.
/// </summary>
constexpr std::size_t BENCHLAB_POWER_SENSORS = 11;

/// <summary>
/// The number of RGB profiles supported.
/// </summary>
constexpr std::size_t BENCHLAB_RGB_PROFILES = 2;

#else /* defined(__cplusplus) */
#include <inttypes.h>
#include <stddef.h>

#define BENCHLAB_VENDOR_ID ((uint8_t) 0xEE)
#define BENCHLAB_PRODUCT_ID ((uint8_t) 0x10)
#define BENCHLAB_FAN_PROFILES ((size_t) 3)
#define BENCHLAB_FANS ((size_t) 9)
#define BENCHLAB_FAN_CURVE_POINTS ((size_t) 2)
#define BENCHLAB_TEMPERATURE_SENSORS ((size_t) 4)
#define BENCHLAB_VIN_SENSORS ((size_t) 13)
#define BENCHLAB_POWER_SENSORS ((size_t) 11)
#define BENCHLAB_RGB_PROFILES ((size_t) 2)
#endif /* defined(__cplusplus) */

//public const int CAL_NUM = 2;
//public const int CAL_FACTORY = 0;
//public const int CAL_USER = 1;
