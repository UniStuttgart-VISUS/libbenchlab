// <copyright file="benchlab.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "libbenchlab/benchlab.h"

#include "device.h"


/*
 * ::benchlab_button_press
 */
HRESULT LIBBENCHLAB_API benchlab_button_press(_In_ benchlab_handle handle,
        _In_ const benchlab_button button,
        _In_ const uint8_t duration) {
    if (handle == nullptr) {
        return E_HANDLE;
    }

    return handle->press(button, std::chrono::milliseconds(duration));
}


/*
 * ::benchlab_close
 */
HRESULT LIBBENCHLAB_API benchlab_close(_In_ const benchlab_handle handle) {
    if (handle == nullptr) {
        return E_HANDLE;
    }

    delete handle;
    return S_OK;
}


/*
 * benchlab_get_device_name
 */
HRESULT LIBBENCHLAB_API benchlab_get_device_name(
        _Out_writes_z_(*cnt) char *out_name,
        _Inout_ size_t *cnt,
        _In_ benchlab_handle handle) {
    if (cnt == nullptr) {
        return E_POINTER;
    }

    const auto available = *cnt;
    if ((available > 0) && (out_name == nullptr)) {
        return E_POINTER;
    }

    if (handle == nullptr) {
        return E_HANDLE;
    }

    std::vector<char> name;
    auto hr = handle->name(name);
    if (FAILED(hr)) {
        return hr;
    }

    *cnt = name.size() + 1;
    if (available < *cnt) {
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    ::memcpy(out_name, name.data(), name.size() * sizeof(char));
    out_name[name.size()] = 0;
    return S_OK;
}


/*
 * ::benchlab_get_device_uid
 */
HRESULT LIBBENCHLAB_API benchlab_get_device_uid(
        _Out_ benchlab_device_uid_type *out_uid,
        _In_ benchlab_handle handle) {
    if (out_uid == nullptr) {
        return E_POINTER;
    }
    if (handle == nullptr) {
        return E_HANDLE;
    }

    return handle->uid(*out_uid);
}


/*
 * ::benchlab_get_firmware
 */
HRESULT LIBBENCHLAB_API benchlab_get_firmware(
        _Out_ uint8_t *out_version,
        _In_ benchlab_handle handle) {
    if (out_version == nullptr) {
        return E_POINTER;
    }
    if (handle == nullptr) {
        return E_HANDLE;
    }

    *out_version = handle->version();
    return S_OK;
}


/*
 * benchlab_get_power_sensors
 */
HRESULT LIBBENCHLAB_API benchlab_get_power_sensors(
        _Out_writes_opt_(*cnt) benchlab_char *out_sensors,
        _Inout_ size_t *cnt) {
    typedef std::char_traits<benchlab_char> traits_type;
    static constexpr const benchlab_char *const names[] = {
        BENCHLAB_STR("EPS1"),
        BENCHLAB_STR("EPS2"),
        BENCHLAB_STR("ATX3V"),
        BENCHLAB_STR("ATX5V"),
        BENCHLAB_STR("ATX5VSB"),
        BENCHLAB_STR("ATX12V"),
        BENCHLAB_STR("PCIE1"),
        BENCHLAB_STR("PCIE2"),
        BENCHLAB_STR("PCIE3"),
        BENCHLAB_STR("HPWR1"),
        BENCHLAB_STR("HPWR2")
    };
    static_assert(std::size(names) == BENCHLAB_POWER_SENSORS, "Names are "
        "provided for all power sensors.");

    if (cnt == nullptr) {
        return E_POINTER;
    }

    if ((*cnt > 0) && (out_sensors == nullptr)) {
        return E_POINTER;
    }

    // Determine how many characters we need for all the names as multi-sz
    // string.
    std::size_t required = 1;
    for (auto n : names) {
        required += traits_type::length(n) + 1;
    }

    // If the user buffer is not big enough, report the required size and bail
    // out with an appropriate error.
    if (*cnt < required) {
        *cnt = required;
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    // The output buffer is valid and large enough, so we actually copy the
    // strings.
    *cnt = required;
    auto dst = out_sensors;
    for (auto n : names) {
        while ((*dst++ = *n++));
    }
    *dst = static_cast<benchlab_char>(0);

    return S_OK;
}


/*
 * ::benchlab_open
 */
HRESULT LIBBENCHLAB_API benchlab_open(_Out_ benchlab_handle *out_handle,
        _In_z_ const benchlab_char *com_port,
        _In_opt_ const benchlab_serial_configuration *config) {
    if (out_handle == nullptr) {
        //_powenetics_debug("Invalid storage location for handle provided.\r\n");
        return E_POINTER;
    }
    if (com_port == nullptr) {
        //_powenetics_debug("Invalid COM port provided.\r\n");
        return E_INVALIDARG;
    }

    benchlab_serial_configuration dft_conf;
    if (config == nullptr) {
        dft_conf.version = 1;
        auto retval = ::benchlab_initialise_serial_configuration(&dft_conf);
        if (retval != S_OK) {
            //_powenetics_debug("Failed to initialise default serial "
            //    "configuration.\r\n");
            return retval;
        }
    }

    std::unique_ptr<benchlab_device> device(
        new (std::nothrow) benchlab_device());
    if (device == nullptr) {
        //_powenetics_debug("Insufficient memory for powenetics_device.\r\n");
        return E_OUTOFMEMORY;
    }

    auto conf = (config != nullptr) ? config : &dft_conf;
    auto retval = device->open(com_port, conf);
    if (retval != S_OK) {
        //_powenetics_debug("Failed to open powenetics_device.\r\n");
    }

    // If everything went OK so far, detach from unique_ptr.
    *out_handle = device.release();

    return retval;
}


/*
 * benchlab_probe
 */
HRESULT LIBBENCHLAB_API benchlab_probe(
        _Out_writes_opt_(*cnt) benchlab_handle *out_handles,
        _Inout_ size_t *cnt) {
    if (cnt == nullptr) {
        //_powenetics_debug("A valid number of handles must be provided.\r\n");
        return E_POINTER;
    }

    // Ensure that the counter is never valid if the output buffer is invalid.
    if (out_handles == nullptr) {
        *cnt = 0;
    }

    // Get all serial ports as candidates where a device could be attached to.
    std::vector<std::basic_string<benchlab_char>> ports;
    {
        auto hr = benchlab_device::ports(std::back_inserter(ports));
        if (FAILED(hr)) {
            return hr;
        }
    }

    // Bail out if we cannot return all devices, but tell the caller how big
    // the output array must be to hold all devices.
    if (*cnt < ports.size()) {
        *cnt = ports.size();
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    // Open the devices and count how many of it are actually working.
    *cnt = 0;
    for (std::size_t i = 0; i < ports.size(); ++i) {
        _Analysis_assume_(out_handles != nullptr);
        if (SUCCEEDED(::benchlab_open(out_handles + *cnt,
                ports[i].c_str(),
                nullptr))) {
            ++cnt;
        }
    }

    return (*cnt > 0) ? S_OK : E_NOT_SET;
}


/*
 * ::benchlab_read_rgb
 */
HRESULT LIBBENCHLAB_API benchlab_read_rgb(
        _Out_ benchlab_rgb_config *out_config,
        _In_ benchlab_handle handle,
        _In_ const uint8_t profile) {
    if (out_config == nullptr) {
        return E_POINTER;
    }
    if (handle == nullptr) {
        return E_HANDLE;
    }

    return handle->read(*out_config, profile);
}


/*
 * ::benchlab_read_sensors
 */
HRESULT LIBBENCHLAB_API benchlab_read_sensors(
        _Out_ benchlab_sensor_readings *out_readings,
        _In_ benchlab_handle handle) {
    if (out_readings == nullptr) {
        return E_POINTER;
    }
    if (handle == nullptr) {
        return E_HANDLE;
    }

    return handle->read(*out_readings);
}


/*
 * ::benchlab_write_rgb
 */
HRESULT LIBBENCHLAB_API benchlab_write_rgb(
        _In_ benchlab_handle handle,
        _In_ const benchlab_rgb_config *config,
        _In_ const uint8_t profile) {
    if (handle == nullptr) {
        return E_HANDLE;
    }
    if (config == nullptr) {
        return E_INVALIDARG;
    }

    return handle->write(*config, profile);
}
