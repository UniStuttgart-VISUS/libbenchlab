// <copyright file="debug.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#if !defined(_BENCHLAB_DEBUG_H)
#define _BENCHLAB_DEBUG_H
#pragma once

#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#include <cstdio>
#endif /* defined(_WIN32) */


#if (defined(DEBUG) || defined(_DEBUG))

#if defined(_WIN32)
#define _benchlab_debug(msg) ::OutputDebugStringW(L##msg)
#else /* defined(_WIN32) */
#define _benchlab_debug(msg) ::printf(L##msg)

#endif /* defined(_WIN32) */

#else /* (defined(DEBUG) || defined(_DEBUG)) */
#define _benchlab_debug(msg)
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

#endif /* !defined(_BENCHLAB_DEBUG_H) */
