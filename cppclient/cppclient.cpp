// <copyright file="cppclient.c" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <libbenchlab/serial.h>

#include <cassert>
#include <iostream>
#include <vector>

#if defined(_WIN32)
#include <conio.h>
#include <objbase.h>
#include <Windows.h>
#include <tchar.h>

#define _tcout wcout
#else /* defined(_WIN32) */
#include <unistd.h>

#define _tmain main
#define _TCHAR char

#define tcout cout
#endif /* defined(_WIN32) */

#include "libbenchlab/benchlab.h"


/// <summary>
/// A callback to receive the samples from the device.
/// </summary>
/// <param name="src"></param>
/// <param name="sample"></param>
/// <param name="ctx"></param>
void on_sample(_In_ benchlab_handle src,
        _In_ const benchlab_sample *sample,
        _In_opt_ void *ctx) {
    typedef std::vector<std::basic_string<benchlab_char>> sensor_list;
    auto& sensors = *static_cast<sensor_list *>(ctx);
    _Analysis_assume_(sensors != nullptr);

    for (std::size_t i = 0; i < BENCHLAB_VIN_SENSORS; ++i) {
        std::_tcout << _T("Input voltage #") << i << _T(": ")
            << sample->input_voltage[i] << _T(" V") << std::endl;
    }

    std::_tcout << _T("Supply voltage: ")
        << sample->supply_voltage << _T(" V") << std::endl;
    std::_tcout << _T("Reference voltage: ")
        << sample->reference_voltage << _T(" V") << std::endl;
    std::_tcout << _T("Chip temperature: ")
        << sample->chip_temperature << _T("°C") << std::endl;

    for (std::size_t i = 0; i < BENCHLAB_TEMPERATURE_SENSORS; ++i) {
        std::_tcout << _T("Temperature #") << i << _T(": ")
            << sample->temperatures[i] << _T(" °C") << std::endl;
    }

    std::_tcout << _T("Ambient temperature: ")
        << sample->ambient_temperature << _T(" °C") << std::endl;
    std::_tcout << _T("Humidity: ")
        << sample->humidity << _T(" %") << std::endl;
    std::_tcout << _T("External fan duty: ")
        << sample->external_fan_duty << std::endl;

    for (std::size_t i = 0; i < BENCHLAB_POWER_SENSORS; ++i) {
        std::_tcout << _T("Voltage #") << i << _T(" (") << sensors[i]
            << _T("): ") << sample->voltages[i] << _T(" V") << std::endl;
        std::_tcout << _T("Current #") << i << _T(" (") << sensors[i]
            << _T("): ") << sample->currents[i] << _T(" A") << std::endl;
        std::_tcout << _T("Power #") << i << _T(" (") << sensors[i]
            << _T("): ") << sample->power[i] << _T(" W") << std::endl;
    }

    for (std::size_t i = 0; i < BENCHLAB_FANS; ++i) {
        std::_tcout << _T("Fan #") << i << _T(" speed:")
            << sample->fan_speeds[i] << std::endl;
        std::_tcout << _T("Fan #") << _T(" duty:")
            << sample->fan_duties[i] << std::endl;
    }
}


/// <summary>
/// The entry point of the test application for the C-style API.
/// </summary>
/// <remarks>
/// The main reason for this application existing at all is testing whether
/// it is actually working if linked against C code. We want to make this sure
/// in a test as we implement the library in C++ and only limit the interface
/// to C.
/// </remarks>
/// <param name="argc">The number of command line arguments.</param>
/// <param name="argv">The list of command line arguments.</param>
/// <returns>Zero, unconditionally.</returns>
int _tmain(int argc, _TCHAR **argv) {
    visus::benchlab::unique_handle handle;
    HRESULT hr = S_OK;
    auto sensors = visus::benchlab::get_power_sensors();

    // Initialisation phase: either open the user-defined port or probe for one
    // Powenetics device attached to the machine.
    if (SUCCEEDED(hr)) {
        if (argc < 2) {
            hr = visus::benchlab::probe(handle);

            // For the demo, we can live with having only one device, so if the
            // error indicates that there would be more, we just ignore that.
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                hr = S_OK;
            }

        } else {
            hr = visus::benchlab::open(handle, argv[1], nullptr);
        }
    }
    assert(handle || FAILED(hr));

    if (SUCCEEDED(hr)) {
        benchlab_device_uid_type uid;
        if (SUCCEEDED(hr = ::benchlab_get_device_uid(&uid, handle.get()))) {
#if defined(_WIN32)
            wchar_t str[128];
            if (::StringFromGUID2(uid, str, sizeof(str) / sizeof(wchar_t))) {
                std::wcout << L"Device UID: " << str << std::endl;
            }
#endif /* defined(_WIN32) */
        }
    }

    if (SUCCEEDED(hr)) {
        char name[64];
        std::size_t cnt = sizeof(name);
        if (SUCCEEDED(hr = ::benchlab_get_device_name(name, &cnt,
                handle.get()))) {
            std::cout << "Device name: " << name << std::endl;;
        }
    }

    if (SUCCEEDED(hr)) {
        uint8_t version = 0;
        if (SUCCEEDED(hr = ::benchlab_get_firmware(&version, handle.get()))) {
            std::_tcout << _T("Firmware version: ") << version << std::endl;
        }
    }

    // Stream data to 'on_sample'.
    if (SUCCEEDED(hr)) {
        hr = benchlab_start_streaming(handle.get(), 10, &on_sample, &sensors);
    }

    if (SUCCEEDED(hr)) {
#if defined(_WIN32)
        std::_tcout << _T("Press any key to end measurement.") << std::endl;
        ::_getch();
#else /* defined(_WIN32) */
        ::sleep(10);
#endif /* defined(_WIN32) */
    }

    return 0;
}
