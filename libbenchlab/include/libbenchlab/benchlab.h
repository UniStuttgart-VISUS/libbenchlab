// <copyright file="benchlab.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "libbenchlab/api.h"
#include "libbenchlab/serial.h"


#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */


/// <summary>
/// Closes the handle to the given Benchlab telemetry system.
/// </summary>
/// <param name="handle">The handle of the device to close.</param>
/// <returns><c>S_OK</c> in case of success,
/// <c>E_HANDLE</c> if <paramref name="handle" /> is invalid.</returns>
HRESULT LIBBENCHLAB_API benchlab_close(_In_ const benchlab_handle handle);

/// <summary>
/// Opens a handle to the Benchöab telemetry system connected to the specified
/// serial port.
/// </summary>
/// <param name="out_handle">Receives the handle for the power measurement
/// device in case of success,</param>
/// <param name="com_port">The path to the COM port. The format of this path
/// is platform-specific. For instance, on Windows, this would be something
/// like &quot;\\.\COM3&quot;, whereas on Linux, you would use something
/// like &quot;/dev/ttyACM0&quot;.</param>
/// <param name="config">The configuration used for the serial port. It is safe
/// to pass <c>nullptr</c>, in which case the function will obtain the default
/// configuration by calling
/// <see cref="benchlab_initialise_serial_configuration" />.</param>
/// <returns><c>S_OK</c> in case of success,
/// <c>E_INVALIDARG</c> if either <paramref name="out_handle "/> or
/// <paramref name="com_port" /> is <c>nullptr</c>,
/// <c>E_NOT_VALID_STATE</c> if the device has already been opened
/// before, a platform-specific error code if accessing the selected
/// serial port failed.</returns>
HRESULT LIBBENCHLAB_API benchlab_open(_Out_ benchlab_handle *out_handle,
    _In_z_ const benchlab_char *com_port,
    _In_opt_ const benchlab_serial_configuration *config);

/// <summary>
/// Opens at most <paramref name="cnt" /> Benchlab telemetry devices connected
/// to the local machine.
/// </summary>
/// <param name="out_handles">A buffer to receive at least
/// <paramref name="cnt" /> handles to devices.</param>
/// <param name="cnt">On entry, the number of handles that can be saved to
/// <paramref name="out_handles" />, on exit, the number of handles that have
/// actually been saved.</param>
/// <returns><c>S_OK</c> in case the operation succeeded,
/// <c>HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)</c> if there were more
/// devices than could be stored to <paramref name="out_handles" />, 
/// <c>HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)</c> if no device at all
/// was found, another error code if establishing the connection to the
/// device failed.</returns>
HRESULT LIBBENCHLAB_API benchlab_probe(
    _Out_writes_opt_(*cnt) benchlab_handle *out_handles,
    _Inout_ size_t *cnt);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* defined(__cplusplus) */
