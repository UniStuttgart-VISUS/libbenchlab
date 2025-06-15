// <copyright file="io.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "io.h"


/*
 * ::get_files
 */
std::vector<std::basic_string<benchlab_char>> get_files(
        _In_ const std::basic_string<benchlab_char>& path,
        _In_ const bool is_recursive) {
    std::vector<std::basic_string<benchlab_char>> retval;

    ::get_file_system_entries(std::back_inserter(retval),
        path,
        is_recursive,
#if defined(_WIN32)
        [](const WIN32_FIND_DATAW& e) {
            return ((e.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
        });
#else /* defined(_WIN32) */
        [](const  struct dirent& e) {
            return (e.d_type == DT_REG);
        });
#endif /* defined(_WIN32) */

    return retval;
}
