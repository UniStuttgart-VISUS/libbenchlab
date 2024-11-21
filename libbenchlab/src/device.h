// <copyright file="deivce.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "libbenchlab/types.h"

#include <cstddef>


/// <summary>
/// Represents the connection to a Benchlab device and implements the
/// communication with that device.
/// </summary>
struct LIBBENCHLAB_TEST_API benchlab_device final {

public:

    benchlab_device(void) noexcept;

private:

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
    /// Reads at mode <paramref name="cnt" /> bytes from the serial port.
    /// </summary>
    /// <remarks>
    /// This method must not be called while the device is streaming. Only
    /// the streaming thread within the object may read at this point.
    /// </remarks>
    /// <param name="dst">A buffer that is able to receive at least
    /// <paramref name="cnt" /> bytes.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> on entry, the
    /// number of bytes written on successful exit.</param>
    /// <returns></returns>
    HRESULT read(_Out_writes_bytes_(cnt) void *dst,
        _Inout_ std::size_t& cnt) noexcept;

    handle_type _handle;
};
