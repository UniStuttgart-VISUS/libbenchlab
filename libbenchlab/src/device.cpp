// <copyright file="device.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "device.h"


/*
 * benchlab_device::benchlab_device
 */
benchlab_device::benchlab_device(void) noexcept : _handle(invalid_handle) { }


/*
 * benchlab_device::read
 */
HRESULT benchlab_device::read(_Out_writes_bytes_(cnt) void *dst,
        _Inout_ std::size_t& cnt) noexcept {
#if defined(_WIN32)
    DWORD read;

    if (::ReadFile(this->_handle, dst, static_cast<DWORD>(cnt), &read,
        nullptr)) {
        cnt = read;
        return S_OK;
    } else {
        cnt = 0;
        return HRESULT_FROM_WIN32(::GetLastError());
    }

#else /* defined(_WIN32) */
    cnt = ::read(this->_handle, dst, cnt);
    if (cnt < 0) {
        cnt = 0;
        return static_cast<HRESULT>(-errno);
    } else {
        return S_OK;
    }
#endif /* defined(_WIN32) */
}
