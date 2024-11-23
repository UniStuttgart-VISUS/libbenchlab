// <copyright file="deivce.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <array>
#include <chrono>
#include <cinttypes>
#include <cstddef>
#include <thread>

#include "libbenchlab/serial.h"
#include "libbenchlab/types.h"



/// <summary>
/// Represents the connection to a Benchlab device and implements the
/// communication with that device.
/// </summary>
struct LIBBENCHLAB_TEST_API benchlab_device final {

public:

    benchlab_device(void) noexcept;

    ~benchlab_device(void) noexcept;

    /// <summary>
    /// Close the serial port connection to the device.
    /// </summary>
    HRESULT close(void) noexcept;

    /// <summary>
    /// Opens and configures the specified COM port if the device has not
    /// yet been opened.
    /// </summary>
    HRESULT open(_In_z_ const benchlab_char *com_port,
        _In_ const benchlab_serial_configuration *config) noexcept;

private:

    enum class command : std::uint8_t {
        welcome = 0,
        read_sensors,
        action,
        read_name,
        write_name,
        read_fan_profile,
        write_fan_profile,
        read_rgb,
        write_rgb,
        read_calibration,
        write_calibration,
        load_calibration,
        store_calibration,
        read_uid,
        read_vendor_data,
    };

#if defined(_WIN32)
    typedef HANDLE handle_type;
#else /* defined(_WIN32) */
    typedef int handle_type;
#endif /* defined(_WIN32) */

#if defined(_WIN32)
    static constexpr handle_type invalid_handle = INVALID_HANDLE_VALUE;
#else /* defined(_WIN32) */
    static constexpr handle_type invalid_handle = -1;
#endif /* defined(_WIN32) */

    /// <summary>
    /// Requests the vendor data that include the version of the hardware and
    /// checks whether the response is as expected.
    /// </summary>
    /// <returns><c>S_OK</c> in case of success, <c>E_NOTIMPL</c> if a response
    /// was retrieved, but it was not the expected one, an error code if the
    /// underlying I/O operations failed.</returns>
    HRESULT check_vendor_data(void) noexcept;

    /// <summary>
    /// Requests the welcome message from the device and checks that the response
    /// is as expected.
    /// </summary>
    /// <returns><c>S_OK</c> in case of success, <c>E_NOTIMPL</c> if a response
    /// was retrieved, but it was not the expected one, an error code if the
    /// underlying I/O operations failed.</returns>
    HRESULT check_welcome(void) const noexcept;

    /// <summary>
    /// Reads at most <paramref name="cnt" /> bytes from the serial port.
    /// </summary>
    /// <remarks>
    /// This method must not be called while the device is streaming. Only
    /// the streaming thread within the object may read at this point.
    /// </remarks>
    /// <param name="dst">A buffer that is able to receive at least
    /// <paramref name="cnt" /> bytes.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> on entry, the
    /// number of bytes written on successful exit.</param>
    /// <returns><c>S_OK</c> in case of success, an error code otherwise.
    /// </returns>
    HRESULT read(_Out_writes_bytes_(cnt) void *dst,
        _Inout_ std::size_t& cnt) const noexcept;

    /// <summary>
    /// Reads <paramref name="cnt" /> bytes from the serial port or fails
    /// after <paramref name="timeout" /> milliseconds.
    /// </summary>
    /// <remarks>
    /// This method must not be called while the device is streaming. Only
    /// the streaming thread within the object may read at this point.
    /// </remarks>
    /// <param name="dst">A buffer that is able to receive at least
    /// <paramref name="cnt" /> bytes.</param>
    /// <param name="cnt">The number of bytes to read.</param>
    /// <param name="timeout">The timeout after which the operation will
    /// fail if it could not read <paramref name="cnt"/> bytes.</param>
    /// <returns><c>S_OK</c> in case of success, an error code otherwise.
    /// </returns>
    HRESULT read(_Out_writes_bytes_(cnt) void *dst,
        _In_ const std::size_t cnt,
        _In_ const std::chrono::milliseconds timeout) const noexcept;

    /// <summary>
    /// Reads all of <paramref name="dst" /> from the serial port or fails
    /// after <paramref name="timeout" /> milliseconds.
    /// </summary>
    /// <remarks>
    /// This method must not be called while the device is streaming. Only
    /// the streaming thread within the object may read at this point.
    /// </remarks>
    /// <typeparam name="TType"></typeparam>
    /// <typeparam name="Size"></typeparam>
    /// <param name="dst"></param>
    /// <param name="timeout"></param>
    /// <returns><c>S_OK</c> in case of success, an error code otherwise.
    /// </returns>
    template<class TType, std::size_t Size>
    inline HRESULT read(_Out_ std::array<TType, Size>& dst,
            _In_ const std::chrono::milliseconds timeout) const noexcept {
        return this->read(dst.data(), sizeof(TType) * dst.size(), timeout);
    }

    /// <summary>
    /// Synchronously write the given data to the serial port.
    /// </summary>
    /// <remarks>
    /// The method makes best effort to write all <paramref name="cnt" />
    /// bytes. If this is not possible, it will fail with an error code
    /// indicating the reason.
    /// </remarks>
    /// <param name="data">A pointer to at least <paramref name="cnt" />
    /// bytes of valid data.</param>
    /// <param name="cnt">The number of bytes to write.</param>
    /// <returns><c>S_OK</c> in case of success, an error code otherwise.
    /// </returns>
    HRESULT write(_In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) const noexcept;

    /// <summary>
    /// Writes the given <paramref name="command" /> to the device, potentially
    /// followed by a command <paramref name="parameter" />.
    /// </summary>
    /// <param name="command"></param>
    /// <param name="parameter"></param>
    /// <param name="cnt"></param>
    /// <returns><c>S_OK</c> in case of success, an error code otherwise.
    /// </returns>
    HRESULT write(_In_ const command command,
        _In_reads_bytes_opt_(cnt) const void *parameter = nullptr,
        _In_ const std::size_t cnt = 0) const noexcept;

    handle_type _handle;
    std::chrono::milliseconds _timeout;
    std::uint8_t _version;
};
