﻿// <copyright file="excel_worker.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include <libbenchlab/benchlab.h>

#include <wil/resource.h>

#include "excel_output.h"


/// <summary>
/// Manages the output of Benchlab data into an excel sheet and most
/// importantly decouples writing the data from the sampler thread.
/// </summary>
class excel_worker final {

public:

    /// <summary>
    /// Initialises a new instance writing to the given
    /// <paramref name="output" />.
    /// </summary>
    /// <param name="input">The handle to a Benchlab device that has already
    /// been opened, but is not yet streaming. The object will register its own
    /// callback and starts streaming to it.</param>
    /// <param name="output">The Excel output to write to. Callers must make
    /// sure that this instance lives at least as long as the worker is running.
    /// </param>
    /// <exception cref="wil::ResultException">If the worker could not be
    /// started, which mainly has two reasons: streaming has already been
    /// started on <paramref name="input" /> before or <paramref name="input" />
    /// is <c>nullptr</c>.</exception>
    excel_worker(_Inout_ visus::benchlab::unique_handle&& input,
        _Inout_ excel_output& output);

    /// <summary>
    /// Finalises the instance.
    /// </summary>
    ~excel_worker(void) noexcept;

    /// <summary>
    /// (Re-) Starts streaming.
    /// </summary>
    /// <remarks>
    /// This method may only be called if the worker has been stopped before.
    /// The constructor will call this method as well and start streaming
    /// automatically.
    /// </remarks>
    /// <param name="interval">The desired interval in milliseconds between two
    /// samples.</param>
    /// <exception cref="wil::ResultException">If the operation failed, most
    /// likely because the worker was already running.</exception>
    void start(_In_ const std::chrono::milliseconds interval
        = std::chrono::milliseconds(5));

    /// <summary>
    /// Makes the worker stop the Powenetics device and causes the writer thread
    /// to exit.
    /// </summary>
    /// <remarks>
    /// It is safe to assume that the writer thread has exited once the method
    /// returns. The destructor will call this method, too, if streaming has not
    /// been stopped before.
    /// </remarks>
    void stop(void);

private:

    /// <summary>
    /// The callback that can be registered with a
    /// <see cref="powenetics_handle" /> to write its data to the
    /// configured Excel sheet. Note that a pointer to the
    /// <see cref="excel_worker" /> must be passed as the context.
    /// </summary>
    /// <param name="source">The handle to the device we receive the
    /// data from.</param>
    /// <param name="sample">The sample that the device has just produced.
    /// </param>
    /// <param name="context">A pointer to this worker object.</param>
    static void callback(_In_ benchlab_handle source,
        _In_ const benchlab_sample *sample,
        _In_opt_ void *context);

    /// <summary>
    /// The worker thread that periodically writes the data into the
    /// Excel sheet.
    /// </summary>
    void worker(void);

    wil::unique_event _event;
    visus::benchlab::unique_handle _input;
    std::mutex _lock;
    excel_output& _output;
    std::vector<benchlab_sample> _samples;
    std::thread _thread;

};
