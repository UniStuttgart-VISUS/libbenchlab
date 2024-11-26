// <copyright file="cclient.c" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include <libbenchlab/serial.h>

#include <assert.h>
#include <stdio.h>

#if defined(_WIN32)
#include <conio.h>
#include <objbase.h>
#include <Windows.h>
#include <tchar.h>
#else /* defined(_WIN32) */
#include <unistd.h>

#define _tmain main
#define _TCHAR char
#endif /* defined(_WIN32) */

#include "libbenchlab/benchlab.h"


#if 0
/// <summary>
/// A callback to receive the samples from the device.
/// </summary>
/// <param name="src"></param>
/// <param name="sample"></param>
/// <param name="ctx"></param>
void on_sample(_In_ powenetics_handle src,
        _In_ const powenetics_sample *sample,
        _In_opt_ void *ctx) {
    printf("ATX 12V: %f V, %f A\r\n",
        sample->atx_12v.voltage,
        sample->atx_12v.current);
    printf("ATX 3.3V: %f V, %f A\r\n",
        sample->atx_3_3v.voltage,
        sample->atx_3_3v.current);
    printf("ATX 5V: %f V, %f A\r\n",
        sample->atx_5v.voltage,
        sample->atx_5v.current);
    printf("ATX Stand-by: %f V, %f A\r\n",
        sample->atx_stb.voltage,
        sample->atx_stb.current);
    printf("EPS #1: %f V, %f A\r\n",
        sample->eps1.voltage,
        sample->eps1.current);
    printf("EPS #2: %f V, %f A\r\n",
        sample->eps2.voltage,
        sample->eps2.current);
    printf("EPS #3: %f V, %f A\r\n",
        sample->eps3.voltage,
        sample->eps3.current);
    printf("PCIe 12V #1: %f V, %f A\r\n",
        sample->pcie_12v1.voltage,
        sample->pcie_12v1.current);
    printf("PCIe 12V #2: %f V, %f A\r\n",
        sample->pcie_12v2.voltage,
        sample->pcie_12v2.current);
    printf("PCIe 12V #3: %f V, %f A\r\n",
        sample->pcie_12v3.voltage,
        sample->pcie_12v3.current);
    printf("PEG 12V: %f V, %f A\r\n",
        sample->peg_12v.voltage,
        sample->peg_12v.current);
    printf("PEG 3.3V: %f V, %f A\r\n",
        sample->peg_3_3v.voltage,
        sample->peg_3_3v.current);
}
#endif

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

    // Initialisation phase: either open the user-defined port or probe for one
    // Powenetics device attached to the machine.
    if (SUCCEEDED(hr)) {
        if (argc < 2) {
            size_t cnt = 1;
            hr = benchlab_probe(&handle, &cnt);

            // For the demo, we can live with having only one device, so if the
            // error indicates that there would be more, we just ignore that.
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                hr = S_OK;
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

    if (SUCCEEDED(hr)) {
        benchlab_sensor_readings readings;
        hr = benchlab_read_sensors(&readings, handle);

        benchlab_sample sample;
        hr = benchlab_readings_to_sample(&sample, &readings, NULL);
        
        benchlab_rgb_config rgb_config;
        hr = benchlab_read_rgb(&rgb_config, handle, 0);

        int x = 5;

        //rgb_config.red = 0x00;
        //rgb_config.green = 0x00;
        //rgb_config.blue = 0xff;
        //rgb_config.mode = benchlab_rgb_mode_single_colour;
        //rgb_config.speed = 128;
        //rgb_config.direction = benchlab_rgb_direction_anti_clockwise;
        //hr = benchlab_write_rgb(handle, &rgb_config);

        //hr = benchlab_read_rgb(&rgb_config, handle);

        //hr = benchlab_button_press(handle, benchlab_button_power, 200);
    }

    {
        size_t s = 0;
        benchlab_get_power_sensors(NULL, &s);
        benchlab_char *ss = malloc(s * sizeof(benchlab_char));
        benchlab_get_power_sensors(ss, &s);
        int x = 0;
    }

#if false
    // Calibrate the device.
    if (SUCCEEDED(hr)) {
        //hr = powenetics_calibrate(handle);
    }

    // Stream data to 'on_sample'.
    if (SUCCEEDED(hr)) {
        hr = powenetics_start_streaming(handle, on_sample, NULL);
    }

    if (SUCCEEDED(hr)) {
#if defined(_WIN32)
        printf("Press any key to end measurement.\n");
        getch();
#else /* defined(_WIN32) */
        sleep(10);
#endif /* defined(_WIN32) */
    }

    // Cleanup phase.
    if (handle != NULL) {
        powenetics_close(handle);
    }
#endif

    return 0;
}
