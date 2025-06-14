// <copyright file="api.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once


#if (defined(_MSC_VER) && !defined(LIBBENCHLAB_STATIC))

#if defined(LIBBENCHLAB_EXPORTS)
#define LIBBENCHLAB_API __declspec(dllexport)
#else /* defined(LIBBENCHLAB_EXPORTS) */
#define LIBBENCHLAB_API __declspec(dllimport)
#endif /* defined(LIBBENCHLAB_EXPORTS)*/

#else /* (defined(_MSC_VER) && !defined(LIBBENCHLAB_STATIC)) */

#define LIBBENCHLAB_API

#endif /* (defined(_MSC_VER) && !defined(LIBBENCHLAB_STATIC)) */


#if defined(LIBBENCHLAB_EXPOSE_TO_TESTING)
#define LIBBENCHLAB_TEST_API LIBBENCHLAB_API
#else /* defined(LIBBENCHLAB_EXPOSE_TO_TESTING) */
#define LIBBENCHLAB_TEST_API
#endif /* defined(LIBBENCHLAB_EXPOSE_TO_TESTING) */


#if !defined(_WIN32)
#include "libbenchlab/nosal.h"
#endif /* !defined(_WIN32) */
