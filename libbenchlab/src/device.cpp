// <copyright file="device.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "device.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <limits>


/*
 * benchlab_device::benchlab_device
 */
benchlab_device::benchlab_device(void) noexcept
        : _command_sleep(10),
        _handle(invalid_handle),
        _timeout(0),
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
 * benchlab_device::name
 */
HRESULT benchlab_device::name(_Out_ std::vector<char>& name) const noexcept {
    auto hr = this->check_handle();
    if (FAILED(hr)) {
        return hr;
    }

    hr = this->write(command::read_name);
    if (FAILED(hr)) {
        return hr;
    }

    this->command_sleep();

    name.resize(32);
    hr = this->read(name, this->_timeout);
    if (FAILED(hr)) {
        return hr;
    }

    // Erase the padding at the end.
    const auto end = std::find(name.begin(), name.end(), '\0');
    name.erase(end, name.end());

    return S_OK;
}


/*
 * benchlab_device::name
 */
HRESULT benchlab_device::name(_In_ const std::string& name) noexcept {
    std::array<char, 32> parameter { 0 };

    // Make sure that we have exactly 32 ASCII characters to send to the device.
    if (name.size() > parameter.size()) {
        std::copy_n(name.begin(), parameter.size(), parameter.begin());
    } else {
        std::copy(name.begin(), name.end(), parameter.begin());
    }

    return this->write(command::write_name, parameter.data(), parameter.size());
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

        // Like in the .NET framework, mak the handshake stuff to DCB. Cf.
        // https://github.com/dotnet/runtime/blob/9d5a6a9aa463d6d10b0b0ba6d5982cc82f363dc3/src/libraries/System.IO.Ports/src/System/IO/Ports/SerialStream.Windows.cs#L191-L235
        const auto rts = (config->handshake
            == benchlab_handshake::request_to_send);
        const auto rts_xon_xoff = (config->handshake
            == benchlab_handshake::request_to_send_xon_xoff);
        const auto xon_xoff = (config->handshake
            == benchlab_handshake::xon_xoff);

        dcb.fInX = dcb.fOutX = (xon_xoff || rts_xon_xoff) ? 1 : 0;
        dcb.fOutxCtsFlow = (rts || rts_xon_xoff) ? 1 : 0;

        if (rts || rts_xon_xoff) {
            dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
        //} else if (_rtsEnable) {
        //    SetDcbFlag(Interop.Kernel32.DCBFlags.FRTSCONTROL, Interop.Kernel32.DCBRTSFlowControl.RTS_CONTROL_ENABLE);
        } else {
            dcb.fRtsControl = RTS_CONTROL_DISABLE;
        }

        if (!::SetCommState(this->_handle, &dcb)) {
            auto retval = HRESULT_FROM_WIN32(::GetLastError());
            //_powenetics_debug("Updating state of COM port failed.\r\n");
            return retval;
        }
    }

    {
        COMMTIMEOUTS cto { 0 };
        constexpr auto infinite = (std::numeric_limits<std::uint32_t>::max)();
        // Cf. https://github.com/dotnet/runtime/blob/9d5a6a9aa463d6d10b0b0ba6d5982cc82f363dc3/src/libraries/System.IO.Ports/src/System/IO/Ports/SerialStream.Windows.cs#L363-L364
        constexpr auto infinite_magic = -2;

        if (config->read_timeout == 0) {
            cto.ReadTotalTimeoutConstant = 0;
            cto.ReadTotalTimeoutMultiplier = 0;
            cto.ReadIntervalTimeout = MAXDWORD;

        } else if (config->read_timeout == infinite) {
            cto.ReadTotalTimeoutConstant = infinite_magic;
            cto.ReadTotalTimeoutMultiplier = 0;
            cto.ReadIntervalTimeout = MAXDWORD;

        } else {
            cto.ReadTotalTimeoutConstant = config->read_timeout;
            cto.ReadTotalTimeoutMultiplier = MAXDWORD;
            cto.ReadIntervalTimeout = MAXDWORD;
        }

        cto.WriteTotalTimeoutConstant = config->write_timeout;
        if (cto.WriteTotalTimeoutConstant == infinite) {
            cto.WriteTotalTimeoutConstant = 0;
        }

        if (!::SetCommTimeouts(this->_handle, &cto)) {
            return HRESULT_FROM_WIN32(::GetLastError());
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

    return S_OK;
}


/*
 * benchlab_device::press
 */
HRESULT benchlab_device::press(_In_ const benchlab_button button,
        _In_ const std::chrono::milliseconds duration) noexcept {
    constexpr std::chrono::milliseconds unit(100);
    constexpr std::chrono::microseconds::rep one = 1;

    // Arguments are: type of the action, the button, 1/0 for press/release, the
    // duration in 100 ms units.
    std::array<std::uint8_t, 4> parameters{
        static_cast<std::uint8_t>(action::button),
        static_cast<std::uint8_t>(button),
        static_cast<std::uint8_t>(1),
        static_cast<std::uint8_t>((std::max)(duration / unit, one))
    };

    return this->write(command::action, parameters.data(), parameters.size());
}


/*
 * benchlab_device::read
 */
HRESULT benchlab_device::read(
        _Out_ benchlab_fan_config& config,
        _In_ const std::uint8_t profile,
        _In_ const std::uint8_t fan) const noexcept {
    {
        auto hr = this->check_handle();
        if (FAILED(hr)) {
            return hr;
        }
    }

    if (profile >= BENCHLAB_FAN_PROFILES) {
        return E_INVALIDARG;
    }

    if (fan >= BENCHLAB_FANS) {
        return E_INVALIDARG;
    }

    {
        std::array<std::uint8_t, 2> parameters { profile, fan };
        auto hr = this->write(command::read_fan_profile,
            parameters.data(),
            parameters.size());
        if (FAILED(hr)) {
            return hr;
        }
    }

    this->command_sleep();

    return this->read(&config, sizeof(config), this->_timeout);
}


/*
 * benchlab_device::read
 */
HRESULT benchlab_device::read(
        _Out_ benchlab_rgb_config& config,
        _In_ const std::uint8_t profile) const noexcept {
    {
        auto hr = this->check_handle();
        if (FAILED(hr)) {
            return hr;
        }
    }

    if (profile >= BENCHLAB_RGB_PROFILES) {
        return E_INVALIDARG;
    }

    {
        auto hr = this->write(command::read_rgb, &profile, 1);
        if (FAILED(hr)) {
            return hr;
        }
    }

    this->command_sleep();

    return this->read(&config, sizeof(config), this->_timeout);
}


/*
 * benchlab_device::read
 */
HRESULT benchlab_device::read(
        _Out_ benchlab_sensor_readings& readings) const noexcept {
    auto hr = this->check_handle();
    if (FAILED(hr)) {
        return hr;
    }

    hr = this->write(command::read_sensors);
    if (FAILED(hr)) {
        return hr;
    }

    this->command_sleep();

    hr = this->read(&readings, sizeof(readings), this->_timeout);

    return hr;
}


/*
 * benchlab_device::uid
 */
HRESULT benchlab_device::uid(
        _Out_ benchlab_device_uid_type& uid) const noexcept {
    ::memset(&uid, 0, sizeof(uid));

    auto hr = this->check_handle();
    if (FAILED(hr)) {
        return hr;
    }

    hr = this->write(command::read_uid);
    if (FAILED(hr)) {
        return hr;
    }

    this->command_sleep();

    std::array<std::uint8_t, 12> response;  // sic.
    hr = this->read(response, this->_timeout);
    if (FAILED(hr)) {
        return hr;
    }

    ::memcpy(&uid, response.data(), response.size());
    return S_OK;
}


/*
 * benchlab_device::write
 */
HRESULT benchlab_device::write(
        _In_ const benchlab_rgb_config& config,
        _In_ const std::uint8_t profile) noexcept {
    {
        auto hr = this->check_handle();
        if (FAILED(hr)) {
            return hr;
        }
    }

    if (profile >= BENCHLAB_RGB_PROFILES) {
        return E_INVALIDARG;
    }

    {
        auto hr = this->write(command::write_rgb, &profile, 1);
        if (FAILED(hr)) {
            return hr;
        }
    }

    return this->write(&config, sizeof(config));
}


/*
 * benchlab_device::check_handle
 */
HRESULT benchlab_device::check_handle(void) const noexcept {
    return (this->_handle != invalid_handle)
        ? S_OK
        : HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
}


/*
 * benchlab_device::check_vendor_data
 */
HRESULT benchlab_device::check_vendor_data(void) noexcept {
    auto hr = this->write(command::read_vendor_data);
    if (FAILED(hr)) {
        return hr;
    }

    this->command_sleep();

    std::array<std::uint8_t, 3> response{ 0 };
    hr = this->read(response, this->_timeout);
    if (FAILED(hr)) {
        return hr;
    }

    if ((response[0] != BENCHLAB_VENDOR_ID)
            || (response[1] != BENCHLAB_PRODUCT_ID)) {
        return E_NOTIMPL;
    }

    this->_version = response[2];
    return S_OK;
}


/*
 * benchlab_device::check_welcome
 */
HRESULT benchlab_device::check_welcome(void) const noexcept {
    static constexpr std::array<char, 9> expected = {
        'B', 'E', 'N', 'C', 'H', 'L', 'A', 'B', '\0'
    };

    auto hr = this->write(command::welcome);
    if (FAILED(hr)) {
        return hr;
    }

    this->command_sleep();

    std::array<char, 9> response { 0 };
    hr = this->read(response, this->_timeout);
    if (FAILED(hr)) {
        return hr;
    }

    if (!std::equal(expected.begin(), expected.end(),
            response.begin(), response.end())) {
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
