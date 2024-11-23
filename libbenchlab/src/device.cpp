// <copyright file="device.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "device.h"

#include <cassert>
#include <cerrno>


/*
 * benchlab_device::benchlab_device
 */
benchlab_device::benchlab_device(void) noexcept
        : _handle(invalid_handle),
        _version(0) { }


/*
 * benchlab_device::~benchlab_device
 */
benchlab_device::~benchlab_device(void) noexcept {
    this->close();
}


/*
 * benchlab_device::close
 */
HRESULT benchlab_device::close(void) noexcept {
#if defined(_WIN32)
    auto retval = ::CloseHandle(this->_handle)
        ? S_OK
        : HRESULT_FROM_WIN32(::GetLastError());
#else /* defined(_WIN32) */
    auto retval = (::close(this->_handle) == 0)
        ? S_OK
        : static_cast<HRESULT>(-errno);
#endif /* defined(_WIN32) */

    this->_handle = invalid_handle;
    return retval;
}


/*
 * benchlab_device::open
 */
HRESULT benchlab_device::open(_In_z_ const benchlab_char *com_port,
        _In_ const benchlab_serial_configuration *config) noexcept {
    assert(com_port != nullptr);
    assert(config != nullptr);

    if (this->_handle != invalid_handle) {
        //_powenetics_debug("Tried opening a powenetics_device that is already "
        //    "connected...\r\n");
        return E_NOT_VALID_STATE;
    }

    this->_timeout = std::chrono::milliseconds(config->read_timeout);


#if defined(_WIN32)
    this->_handle = ::CreateFileW(com_port, GENERIC_READ | GENERIC_WRITE, 0,
        nullptr, OPEN_EXISTING, 0, NULL);
    if (this->_handle == invalid_handle) {
        auto retval = HRESULT_FROM_WIN32(::GetLastError());
        //_powenetics_debug("CreateFile on COM port failed.\r\n");
        return retval;
    }

    {
        DCB dcb { 0 };
        dcb.DCBlength = sizeof(dcb);

        if (!::GetCommState(this->_handle, &dcb)) {
            auto retval = HRESULT_FROM_WIN32(::GetLastError());
            //_powenetics_debug("Retrieving state of COM port failed.\r\n");
            return retval;
        }

        dcb.BaudRate = static_cast<DWORD>(config->baud_rate);
        dcb.ByteSize = static_cast<BYTE>(config->data_bits);
        dcb.Parity = static_cast<BYTE>(config->parity);
        dcb.StopBits = static_cast<BYTE>(config->stop_bits);
        // TODO

        if (!::SetCommState(this->_handle, &dcb)) {
            auto retval = HRESULT_FROM_WIN32(::GetLastError());
            //_powenetics_debug("Updating state of COM port failed.\r\n");
            return retval;
        }
    }
#else /* defined(_WIN32) */
    throw "TODO";
#endif /* defined(_WIN32) */

    {
        auto hr = this->check_welcome();
        if (FAILED(hr)) {
            this->close();
            return hr;
        }
    }

    {
        auto hr = this->check_vendor_data();
        if (FAILED(hr)) {
            this->close();
            return hr;
        }
    }
}


/*
 * benchlab_device::check_vendor_data
 */
HRESULT benchlab_device::check_vendor_data(void) noexcept {

    auto hr = this->write(command::read_vendor_data);
    if (FAILED(hr)) {
        return hr;
    }

    std::array<char, 3> response{ 0 };
    hr = this->read(response, this->_timeout);
    if (FAILED(hr)) {
        return hr;
    }

    if ((response[0] != 0xee) || (response[1] != 0x10)) {
        return E_NOTIMPL;
    }

    this->_version = response[2];
    return S_OK;
}


/*
 * benchlab_device::check_welcome
 */
HRESULT benchlab_device::check_welcome(void) const noexcept {
    static const std::string expected("BENCHLAB");

    auto hr = this->write(command::welcome);
    if (FAILED(hr)) {
        return hr;
    }

    std::array<char, 9> response { 0 };
    hr = this->read(response, this->_timeout);
    if (FAILED(hr)) {
        return hr;
    }

    if (expected != response.data()) {
        return E_NOTIMPL;
    }

    return S_OK;
}


/*
 * benchlab_device::read
 */
HRESULT benchlab_device::read(_Out_writes_bytes_(cnt) void *dst,
        _Inout_ std::size_t& cnt) const noexcept {
    assert(this->_handle != invalid_handle);
    assert(dst != nullptr);

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


/*
 * benchlab_device::read
 */
HRESULT benchlab_device::read(_Out_writes_bytes_(cnt) void *dst,
        _In_ const std::size_t cnt,
        _In_ const std::chrono::milliseconds timeout) const noexcept {
    assert(this->_handle != invalid_handle);
    assert(dst != nullptr);

    auto cur = static_cast<std::uint8_t *>(dst);
    auto rem = cnt;
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    while (true) {
        auto read = rem;
        auto hr = this->read(cur, read);
        if (FAILED(hr)) {
            return hr;
        }

        assert(rem >= read);
        rem -= read;

        if (rem == 0) {
            return S_OK;
        }

        if (std::chrono::steady_clock::now() > deadline) {
#if defined(_WIN32)
            return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
#else /* defined(_WIN32) */
            return (-ETIMEDOUT);
#endif /* defined(_WIN32) */
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


/*
 * benchlab_device::write
 */
HRESULT benchlab_device::write(_In_reads_bytes_(cnt) const void *data,
        _In_ const std::size_t cnt) const noexcept {
    assert(this->_handle != invalid_handle);
    assert(data != nullptr);

#if defined(_WIN32)
    auto cur = static_cast<const std::uint8_t *>(data);
    auto rem = static_cast<DWORD>(cnt);
    DWORD written = 0;

    while (::WriteFile(this->_handle, cur, rem, &written, nullptr)) {
        if (written == 0) {
            return S_OK;
        }

        assert(written <= rem);
        cur += written;
        rem -= written;
    }

    auto retval = HRESULT_FROM_WIN32(::GetLastError());
    //_powenetics_debug("I/O error while sending a command to Powenetics "
    //    "v2 device.\r\n");
    return retval;

#else /* defined(_WIN32) */
    auto cur = static_cast<const std::uint8_t *>(data);
    auto rem = cnt;

    while (rem > 0) {
        auto written = ::write(this->_handle, cur, sizeof(rem));

        if (written < 0) {
            auto hr = static_cast<HRESULT>(-errno);
            //_powenetics_debug("I/O error while sending a command to Powenetics "
            //    "v2 device.\r\n");
            return hr;
        }

        assert(written <= rem);
        cur += written;
        rem -= written;
    }

    return S_OK;
#endif /* defined(_WIN32) */
}


/*
 * benchlab_device::write
 */
HRESULT benchlab_device::write(_In_ const command command,
        _In_reads_bytes_opt_(cnt) const void *parameter,
        _In_ const std::size_t cnt) const noexcept {
    auto hr = this->write(&command, sizeof(command));
    if (FAILED(hr)) {
        return hr;
    }

    if ((parameter != nullptr) && (cnt > 0)) {
        hr = this->write(parameter, cnt);
    }

    return hr;
}
