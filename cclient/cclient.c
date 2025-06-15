// <copyright file="cclient.c" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <libbenchlab/serial.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <conio.h>
#include <objbase.h>
#include <Windows.h>
#include <tchar.h>

#define STR_FMT "%ls"
#else /* defined(_WIN32) */
#include <unistd.h>

#define _tmain main
#define _TCHAR char

#define STR_FMT "%s"
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
    benchlab_char *sensors = (benchlab_char *) ctx;
    _Analysis_assume_(sensors != NULL);

    for (size_t i = 0; i < BENCHLAB_VIN_SENSORS; ++i) {
        printf("Input voltage #%zu: %f V\r\n", i, sample->input_voltage[i]);
    }

    printf("Supply voltage: %f V\r\n", sample->supply_voltage);
    printf("Reference voltage: %f V\r\n", sample->reference_voltage);
    printf("Chip temperature: %f °C\r\n", sample->chip_temperature);

    for (size_t i = 0; i < BENCHLAB_TEMPERATURE_SENSORS; ++i) {
        printf("Temperature #%zu: %f °C\r\n", i, sample->temperatures[i]);
    }

    printf("Ambient temperature: %f °C\r\n", sample->ambient_temperature);
    printf("Humidity: %f %%\r\n", sample->humidity);
    printf("External fan duty: %hhu\r\n", sample->external_fan_duty);

    benchlab_char *sensor = sensors;
    for (size_t i = 0; i < BENCHLAB_POWER_SENSORS; ++i) {
        printf("Voltage #%zu (" STR_FMT "): %f V\r\n",
            i, sensor, sample->voltages[i]);
        printf("Current #%zu (" STR_FMT "): %f A\r\n",
            i, sensor, sample->currents[i]);
        printf("Power #%zu (" STR_FMT "): %f W\r\n",
            i, sensor, sample->power[i]);
        while (*sensor++ != 0);
    }

    for (size_t i = 0; i < BENCHLAB_FANS; ++i) {
        printf("Fan #%zu speed: %hu\r\n", i, sample->fan_speeds[i]);
        printf("Fan #%zu duty: %hhu\r\n", i, sample->fan_duties[i]);
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
    benchlab_handle handle = NULL;
    HRESULT hr = S_OK;
    benchlab_char *sensors = NULL;

    // Initialisation phase: either open the user-defined port or probe for one
    // Benchlab device attached to the machine.
    if (SUCCEEDED(hr)) {
        if (argc < 2) {
            size_t cnt = 1;
            hr = benchlab_probe(&handle, &cnt);

            // For the demo, we can live with having only one device, but we
            // only get anything if we provide a sufficient buffer for *all* the
            // devices. To do that, we need to allocate temporary memory and
            // immediately close all handles but the first one.
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                benchlab_handle *h = malloc(cnt * sizeof(benchlab_handle));
                if (h != NULL) {
                    hr = E_OUTOFMEMORY;
                }

                if (SUCCEEDED(hr)) {
                    hr = benchlab_probe(h, &cnt);
                }

                if (SUCCEEDED(hr)) {
                    assert(cnt > 0);
                    handle = h[0];

                    // Close the handles we do not need.
                    for (size_t i = 1; i < cnt; ++i) {
                    benchlab_close(h[i]);
                    }
                }

                if (h != NULL) {
                    free(h);
                }
            }

        } else {
            hr = benchlab_open(&handle, argv[1], NULL);
        }
    }
    assert((handle != NULL) || FAILED(hr));

    if (SUCCEEDED(hr)) {
        benchlab_device_uid_type uid;
        if (SUCCEEDED(hr = benchlab_get_device_uid(&uid, handle))) {
#if defined(_WIN32)
            wchar_t str[128];
            if (StringFromGUID2(&uid, str, sizeof(str) / sizeof(wchar_t))) {
                printf("Device UID: %ls\r\n", str);
            }
#endif /* defined(_WIN32) */
        }
    }

    if (SUCCEEDED(hr)) {
        char name[64];
        size_t cnt = sizeof(name);
        if (SUCCEEDED(hr = benchlab_get_device_name(name, &cnt, handle))) {
            printf("Device name: %s\r\n", name);
        }
    }

    if (SUCCEEDED(hr)) {
        uint8_t version = 0;
        if (SUCCEEDED(hr = benchlab_get_firmware(&version, handle))) {
            printf("Firmware version: %hhu\r\n", version);
        }
    }

    {
        size_t size = 0;
        benchlab_get_power_sensors(NULL, &size);
        if ((sensors = malloc(size * sizeof(benchlab_char))) == NULL) {
            hr = E_OUTOFMEMORY;
        } else {
            benchlab_get_power_sensors(sensors, &size);
        }
    }

    // Stream data to 'on_sample'.
    if (SUCCEEDED(hr)) {
        hr = benchlab_start_streaming(handle, 10, &on_sample, sensors);
    }

    if (SUCCEEDED(hr)) {
#if defined(_WIN32)
        printf("Press any key to end measurement.\n");
        _getch();
#else /* defined(_WIN32) */
        sleep(10);
#endif /* defined(_WIN32) */
    }

    // Cleanup phase.
    if (handle != NULL) {
        benchlab_close(handle);
    }

    if (sensors != NULL) {
        free(sensors);
    }

    return 0;
}
