// <copyright file="timestamp.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "stdint.h"

#include "libbenchlab/api.h"


#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

    /// <summary>
    /// The type used to represent a timestamp, which is the time elapsed since
    /// 1st January 1601 (UTC) in units of 100ns.
    /// </summary>
    typedef int64_t benchlab_timestamp;

#if defined(LIBBENCHLAB_EXPOSE_TO_TESTING)
    /// <summary>
    /// Creates a timestamp from the current system time using the STL clock.
    /// </summary>
    /// <remarks>
    /// This is the implementation that is used on Linux where
    /// <see cref="GetSystemTimePreciseAsFileTime" /> is not available. We have the
    /// implementation on Windows, too, for testing purposes. This API should not
    /// be called by any client code and is not exported in release builds
    /// </remarks>
    /// <returns>The timestamp in 100ns units.</returns>
    benchlab_timestamp LIBBENCHLAB_TEST_API _benchlab_make_timestamp(void);
#endif /* defined(LIBBENCHLAB_EXPOSE_TO_TESTING) */

    /// <summary>
    /// Creates a timestamp from the current system time.
    /// </summary>
    /// <returns>The timestamp in 100ns units.</returns>
    benchlab_timestamp LIBBENCHLAB_API benchlab_make_timestamp(void);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* defined(__cplusplus) */
