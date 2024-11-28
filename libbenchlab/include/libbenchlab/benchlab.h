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
/// Press the specified <paramref name="button" /> for the given
/// <pararmef name="duration" /> in milliseconds.
/// </summary>
/// <remarks>
/// <para>This function cannot be called while the device is asynchronously
/// streaming measurement data.</para>
/// </remarks>
/// <param name="handle">The handle of the device to press the button of.
/// </param>
/// <param name="button">The button to be pressed.</param>
/// <param name="duration">The duration to hold the button in milliseconds. Note
/// that the hardware only supports multiples of 100 ms, to any value below 100
/// will be clamped to 100. The original software uses 200 ms, so we recommend
/// using 200 here.</param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_button_press(
    _In_ benchlab_handle handle,
    _In_ const benchlab_button button,
    _In_ const uint8_t duration);

/// <summary>
/// Closes the handle to the given Benchlab telemetry system.
/// </summary>
/// <param name="handle">The handle of the device to close.</param>
/// <returns><c>S_OK</c> in case of success,
/// <c>E_HANDLE</c> if <paramref name="handle" /> is invalid.</returns>
HRESULT LIBBENCHLAB_API benchlab_close(_In_ const benchlab_handle handle);

/// <summary>
/// Gets the user-defined device name of the Benchlab device.
/// </summary>
/// <remarks>
/// <para>This function cannot be called while the device is asynchronously
/// streaming measurement data.</para>
/// </remarks>
/// <param name="out_name">Receives the name of the device, which is
/// ASCII-encoded.</param>
/// <param name="cnt">On entry, the number of characters that can be written
/// to <paramref name="out_name" />. On exit, the required number of characters
/// for the device name.</param>
/// <param name="handle">The handle of the device to get the name of.</param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_get_device_name(
    _Out_writes_z_(*cnt) char *out_name,
    _Inout_ size_t *cnt,
    _In_ benchlab_handle handle);

/// <summary>
/// Retrieve the unique hardware ID of the Benchlab.
/// </summary>
/// <remarks>
/// <para>This function cannot be called while the device is asynchronously
/// streaming measurement data.</para>
/// </remarks>
/// <param name="out_uid">Receives the hardware GUID of the device.</param>
/// <param name="handle">The handle of the device to retrieve the GUID of.
/// </param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_get_device_uid(
    _Out_ benchlab_device_uid_type *out_uid,
    _In_ benchlab_handle handle);

/// <summary>
/// Gets the version of the firmware of the Benchlab.
/// </summary>
/// <param name="out_version">Receives the firmware version.</param>
/// <param name="handle">The handle of the device to get the firmware
/// of.</param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_get_firmware(
    _Out_ uint8_t *out_version,
    _In_ benchlab_handle handle);

/// <summary>
/// Gets the names of the power sensors available as a multi-sz string.
/// </summary>
/// <param name="out_sensors">A buffer to receive the multi-sz string of the
/// sensor names. The buffer must be able to hold at least
/// <paramref name="cnt" /> characters if not <c>nullptr</c>.</param>
/// <param name="cnt">On entry, the number of characters in
/// <parmaref name="out_sensors" />, on exit the number of characters of the
/// actual names. If this parameter is non-zero on entry, a valid buffer must
/// be provided for <paramref name="out_sensors" />.</param>
/// <returns><c>S_OK</c> in case of success,
/// <c>HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)</c> if the buffer was too
/// small to hold all sensor names.</returns>
HRESULT LIBBENCHLAB_API benchlab_get_power_sensors(
    _Out_writes_opt_(*cnt) benchlab_char *out_sensors,
    _Inout_ size_t *cnt);

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
/// <c>E_POINTER</c> if <paramref name="out_handle "/> is <c>nullptr</c>,
/// <c>E_INVALIDARG</c> if <paramref name="com_port" /> is <c>nullptr</c>,
/// <c>E_NOT_VALID_STATE</c> if the device has already been opened
/// before, a platform-specific error code if accessing the selected
/// serial port failed.</returns>
HRESULT LIBBENCHLAB_API benchlab_open(_Out_ benchlab_handle *out_handle,
    _In_z_ const benchlab_char *com_port,
    _In_opt_ const benchlab_serial_configuration *config);

/// <summary>
/// Convert the given sensor <paramref name="readings" /> to a sample using
/// Volts, Amperes and Watts rather than the internal units.
/// </summary>
/// <param name="out_sample">A pointer to the sample to be filled.</param>
/// <param name="readings">A pointer to the sensor readings to be converted.
/// </param>
/// <param name="timestamp">An optional timestamp to be set in the sample. If
/// this parameter is <c>nullptr</c>, the function will create a new timestamp
/// from the current system time.</param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_readings_to_sample(
    _Out_ benchlab_sample *out_sample,
    _In_ const benchlab_sensor_readings *readings,
    _In_opt_ const benchlab_timestamp *timestamp);

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

/// <summary>
/// Read a RGB profile from the Benchlab.
/// </summary>
/// <remarks>
/// <para>This function cannot be called while the device is asynchronously
/// streaming measurement data.</para>
/// </remarks>
/// <param name="out_config">Receives the current RGB configuration in case of
/// success.</param>
/// <param name="handle">The handle of the device to get the configuration from.
/// </param>
/// <param name="profile">The zero-based ID of the profile to retrieve.</param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_read_rgb(
    _Out_ benchlab_rgb_config *out_config,
    _In_ benchlab_handle handle,
    _In_ const uint8_t profile);

/// <summary>
/// Performs a raw read of sensor data from the given Benchlab device.
/// </summary>
/// <remarks>
/// <para>This function cannot be called while the device is asynchronously
/// streaming measurement data.</para>
/// <para>It is not recommended to perform raw reads as the sensor readings
/// contain the internal representation of the sensor data rather than
/// in ready-to-use units. Put the device in streaming mode instead. If you
/// need to use raw samples, <see cref="benchlab_readings_to_sample" /> can be
/// used to convert the data.</para>
/// </remarks>
/// <param name="out_readings">Receives the sensor readings.</param>
/// <param name="handle">The handle for the device.</param>
/// <returns><c>S_OK</c> if the data have been returned into the output buffer,
/// <c>E_POINTER</c> if <paramref name="out_readings" /> is <c>nullptr</c>,
/// <c>E_HANDLE</c> if <paramref name="handle" /> is invalid, or an appropriate
/// error code in case of any other error.</returns>
HRESULT LIBBENCHLAB_API benchlab_read_sensors(
    _Out_ benchlab_sensor_readings *out_readings,
    _In_ benchlab_handle handle);

/// <summary>
/// Starts asynchronously streaming data from a Benchlab device to
/// <paramref name="callback" /> every <paramref name="period" /> milliseconds.
/// </summary>
/// <param name="handle">The handle of the device to stream from.</param>
/// <param name="period">The period in milliseconds between two samples.</param>
/// <param name="callback">The callback to receive the samples.</param>
/// <param name="context">A user-defined pointer to be passed to the
/// <paramref name="callback" />.</param>
/// <returns><c>S_OK</c> in case of success, <c>E_HANDLE</c> if
/// <paramref name="handle" /> is invalid, <c>E_POINTER</c> if the
/// <paramref name="callback" /> is an invalid pointer, 
/// <c>E_NOT_VALID_STATE</c> if the device was already streaming.</returns>
HRESULT LIBBENCHLAB_API benchlab_start_streaming(
    _In_ const benchlab_handle handle,
    _In_ const size_t period,
    _In_ const benchlab_sample_callback callback,
    _In_opt_ void *context);

/// <summary>
/// Stops the asynchronous streaming from the given Benchlab device.
/// </summary>
/// <remarks>
/// This method blocks until the thread delivering the samples actually stopped
/// and it is safe to invalidate any previously installed callback.
/// </remarks>
/// <param name="handle">The handle of the device to stop streaming from.</param>
/// <returns><c>S_OK</c> in case of success, <c>E_HANDLE</c> if
/// <paramref name="handle" /> is invalid, <c>E_NOT_VALID_STATE</c> if the device
/// was not streaming in the first place.</returns>
HRESULT LIBBENCHLAB_API benchlab_stop_streaming(
    _In_ const benchlab_handle handle);

/// <summary>
/// Updates the RGB configuration of the given Benchlab device.
/// </summary>
/// <remarks>
/// <para>This function cannot be called while the device is asynchronously
/// streaming measurement data.</para>
/// </remarks>
/// <param name="handle"></param>
/// <param name="config"></param>
/// <param name="profile"></param>
/// <returns></returns>
HRESULT LIBBENCHLAB_API benchlab_write_rgb(
    _In_ benchlab_handle handle,
    _In_ const benchlab_rgb_config *config,
    _In_ const uint8_t profile);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* defined(__cplusplus) */
