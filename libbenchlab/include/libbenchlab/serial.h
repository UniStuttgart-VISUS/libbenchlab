// <copyright file="serial.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "libbenchlab/api.h"
#include "libbenchlab/types.h"


/// <summary>
/// Specifies the control protocol used in establishing a serial port
/// communication.
/// </summary>
/// <remarks>
/// See https://github.com/dotnet/runtime/blob/9d5a6a9aa463d6d10b0b0ba6d5982cc82f363dc3/src/libraries/System.IO.Ports/src/System/IO/Ports/SerialStream.Windows.cs
/// for how this needs to be handled.
/// </remarks>
typedef enum LIBBENCHLAB_ENUM benchlab_handshake_t {
    LIBBENCHLAB_ENUM_SCOPE(benchlab_handshake, none) = 0,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_handshake, xon_xoff),
    LIBBENCHLAB_ENUM_SCOPE(benchlab_handshake, request_to_send),
    LIBBENCHLAB_ENUM_SCOPE(benchlab_handshake, request_to_send_xon_xoff),
} benchlab_handshake;


/// <summary>
/// Possible parities that can be configured in
/// <see cref="benchlab_serial_configuration" />.
/// </summary>
/// <remarks>
/// The numeric values of the enumeration are compatible with the
/// <see cref="DCB" /> structure on Windows.
/// </remarks>
typedef enum LIBBENCHLAB_ENUM benchlab_parity_t {
    LIBBENCHLAB_ENUM_SCOPE(benchlab_parity, none) = 0,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_parity, odd) = 1,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_parity, even) = 2,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_parity, mark) = 3,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_parity, space) = 4
} benchlab_parity;


/// <summary>
/// Possible stop bits that can be set in
/// <see cref="benchlab_serial_configuration" />.
/// </summary>
/// <remarks>
/// The numeric values of the enumeration are compatible with the
/// <see cref="DCB" /> structure on Windows.
/// </remarks>
typedef enum LIBBENCHLAB_ENUM benchlab_stop_bits_t {
    LIBBENCHLAB_ENUM_SCOPE(benchlab_stop_bits, one) = 0,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_stop_bits, one_point_five) = 1,
    LIBBENCHLAB_ENUM_SCOPE(benchlab_stop_bits, two) = 2
} benchlab_stop_bits;


/// <summary>
/// Configures the serial port for talking with a Benchlab device.
/// </summary>
typedef struct LIBBENCHLAB_API benchlab_serial_configuration_t {

    /// <summary>
    /// The version of the structure.
    /// </summary>
    /// <remarks>
    /// <para>This member allows the library to discern between future versions
    /// of the structure. It must be initialised to 1 in the first version of
    /// the library.</para>
    /// <para>This must be the first member of the struct and any future version
    /// of it.</para>
    /// </remarks>
    uint32_t version;

    /// <summary>
    /// The baud rate.
    /// </summary>
    uint32_t baud_rate;

    /// <summary>
    /// The number of data bits.
    /// </summary>
    uint8_t data_bits;

    /// <summary>
    /// Enables the use of the data terminal ready control signal.
    /// </summary>
    bool dtr_enable;

    /// <summary>
    /// Specifies the control protocol used in establishing a serial port
    /// communication.
    /// </summary>
    benchlab_handshake handshake;

    /// <summary>
    /// The parity bit.
    /// </summary>
    benchlab_parity parity;

    /// <summary>
    /// The read timeout in milliseconds.
    /// </summary>
    uint32_t read_timeout;

    /// <summary>
    /// Enabkes the use of the request to send control signal.
    /// </summary>
    bool rts_enable;

    /// <summary>
    /// The number of stop bits.
    /// </summary>
    benchlab_stop_bits stop_bits;

    /// <summary>
    /// The write timeout in milliseconds.
    /// </summary>
    uint32_t write_timeout;
} benchlab_serial_configuration;


#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/// <summary>
/// Applies the default configuration for a Benchlab device to the strucuture
/// passed to the method.
/// </summary>
/// <param name="config">A pointer to the structure to be filled. The version
/// of the structure must have been initialised before the call.</param>
/// <returns><c>S_OK</c> in case of success,
/// <c>E_POINTER</c> if <paramref name="config" /> is <c>nullptr</c>,
/// <c>E_INVALIDARG</c> if the version of the configuration has not been
/// initialised or is unsupported by the function.</returns>
HRESULT LIBBENCHLAB_API benchlab_initialise_serial_configuration(
    _In_ benchlab_serial_configuration *config);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */
