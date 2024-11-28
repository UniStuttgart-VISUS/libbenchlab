# libbenchlab
A native C/C++ library for the [ElmorLabs BENCHLAB](https://benchlab.io/) telemetry system. The implementation of this library is directly derived from the [C# implementation](https://github.com/BenchLab-io/BENCHLAB.BENCHLAB_Core) used by the official tool.

## Building the library
The library is self-contained and can be built using CMake on Windows. Support for Linux is under development.

## Using the library
In order to anything else, you first need to obtain a `benchlab_handle` for the Benchlab device. There are two ways of doing this. If you know the serial port the device is connected to, you can open the handle directly:
```c++
#include <libbenchlab/benchlab.h>

benchlab_handle handle = NULL;

{
    auto hr = ::benchlab_open(&handle, L"\\\\.\\COM3", nullptr);
    if (FAILED(hr)) { /* Handle the error. */ }
}
```

You can also probe for devices like so:
```c++
#include <vector>
#include <libbenchlab/benchlab.h>

// Determine the required buffer size.
std::size_t cnt = 0;
{
    auto hr = ::benchlab_probe(nullptr, &cnt);
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) { /* Handle the error. */ }
}

// Open the handles.
std::vector<benchlab_handle> handles(cnt);
{
    auto hr = ::benchlab_probe(handles.data(), &cnt);
    if (FAILED(hr)) { /* Handle the error. */ }
}
```

Handles need to be closed when no longer used in order to avoid leaking resources:
```c++
if (handle != NULL) {
    ::benchlab_close(handle);
}
```

### Reading sensor data
Sensor data can be obtained synchronously as follows:
```c++
benchlab_sensor_readings readings;

{
    auto hr = ::benchlab_read_sensors(&readings, handle);
    if (FAILED(hr)) { /* Handle the error. */ }
}
```

Note that this will give you the raw data as returned by the device. If you want to have a more user-friendly version of the data using Volts, Amperes and Watts, you can convert the `benchlab_sensor_readings` to a `benchlab_sample_` like so:
```c++
benchlab_sensor_readings readings;
benchlab_sample sample;

{
    auto hr = ::benchlab_readings_to_sample(&sample, &readings, nullptr);
    if (FAILED(hr)) { /* Handle the error. */ }
}
```

### Streaming sensor data
The API also allows for asynchronously streaming `benchlab_sample`s to a user-defined callback:
```c++
void on_sample(benchlab_handle src, const benchlab_sample *sample, void *ctx) {
    // Do something with the sample.
}

{
    // The second parameter specifies the period between two samples. If the
    // period is smaller than it takes to obtain a sample, the API will stream
    // as fast as possible.
    // The fourth parameter is a user-defined context that will be passed to
    // 'on_sample'.
    auto hr = ::benchlab_start_streaming(handle, 10, &on_sample, nullptr);
    if (FAILED(hr)) { /* Handle the error. */ }
}
```

Streaming is stopped by:
```c++
{
    auto hr = benchlab_stop_streaming(handle);
    if (FAILED(hr)) { /* Handle the error. */ }
}
```

> [!WARNING]
> You cannot use any synchronous APIs accessing the hardware while the device is streaming.

## Demo programmes
### cclient
This is the simplest possible demo for obtaining samples in C. The programme probes for a Benchlab device attached to the computer and dumps its data to the console if no command line argument was provided. The programme accepts one optional command line argument, which is the path of the COM port to open.
