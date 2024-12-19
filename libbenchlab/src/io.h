// <copyright file="io.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cerrno>
#include <iterator>
#include <stack>
#include <string>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#include <dirent.h>
#endif /* defined(_WIN32) */

#include "libbenchlab/types.h"

#include "io.inl"


/// <summary>
/// Gets (recursively) all files in the given directory.
/// </summary>
/// <param name="path">The path of the directory to retrieve the files for.
/// </param>
/// <param name="is_recursive">Instructs the function to recursively retrieve all
/// files in <paramref name="path" /> instead of only the immediate children.</param>
/// <returns></returns>
std::vector<std::basic_string<benchlab_char>> get_files(
    _In_ const std::basic_string<benchlab_char>& path,
    _In_ const bool is_recursive);
