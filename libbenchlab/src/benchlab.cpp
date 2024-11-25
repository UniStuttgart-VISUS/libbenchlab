// <copyright file="benchlab.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "libbenchlab/benchlab.h"

#include "device.h"


/*
 * ::benchlab_close
 */
HRESULT LIBBENCHLAB_API benchlab_close(_In_ const benchlab_handle handle) {
    if (handle == nullptr) {
        return E_HANDLE;

    } else {
        delete handle;
        return S_OK;
    }
}


/*
 * ::benchlab_open
 */
HRESULT LIBBENCHLAB_API benchlab_open(_Out_ benchlab_handle *out_handle,
        _In_z_ const benchlab_char *com_port,
        _In_opt_ const benchlab_serial_configuration *config) {
    if (out_handle == nullptr) {
        //_powenetics_debug("Invalid storage location for handle provided.\r\n");
        return E_INVALIDARG;
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

    // Bail out if we cannot return all devices.
    if (*cnt < ports.size()) {
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    // Open the devices and count how many of it are working.
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
