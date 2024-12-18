// <copyright file="io.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/// <summary>
/// Combines two or more <paramref name="paths" /> to a single path string.
/// </summary>
/// <typeparam name="TPaths"></typeparam>
/// <param name="path"></param>
/// <param name="paths"></param>
/// <returns></returns>
template<class... TPaths>
std::basic_string<benchlab_char> combine_path(
        _In_ std::basic_string<benchlab_char> path,
        _In_ TPaths&&... paths) {
#if defined(_WIN32)
    static const std::basic_string<benchlab_char> sep(L"\\");
#else /* defined(_WIN32) */
    static const std::basic_string<benchlab_char> sep("/");
#endif /* defined(_WIN32) */
    int unpack[] { 0, (path += sep + combine_path(paths), 0)... };
    static_cast<void>(unpack);
    return path;
}


/// <summary>
/// Enumerates all child entries of <paramref name="path" /> that fulfill the
/// given <paramref name="predicate" />.
/// </summary>
/// <typeparam name="Titerator">The type of an output iterator, which can be used
/// to write the resulting entries.</typeparam>
/// <typeparam name="TPredicate">The type of a predicate that the entries being
/// returned must fulfill.</typeparam>
/// <param name="oit"></param>
/// <param name="path"></param>
/// <param name="is_recursive"></param>
/// <param name="predicate"></param>
/// <exception cref="std::system_error"></exception>
template<class TIterator, class TPredicate>
void get_file_system_entries(_In_ TIterator oit,
        _In_ const std::basic_string<benchlab_char>& path,
        _In_ const bool is_recursive,
        _In_ TPredicate predicate) {
    typedef std::basic_string<benchlab_char> string_type;
    typedef std::stack<string_type> stack_type;

    stack_type stack;
    stack.push(path);

#if defined(_WIN32)
    static const string_type current_directory_name(L".");
    static const string_type parent_directory_name(L"..");
    WIN32_FIND_DATAW fd;

    while (!stack.empty()) {
        auto cur = stack.top();
        stack.pop();

        auto query = combine_path(cur, L"*");
        auto hFind = ::FindFirstFileW(query.c_str(), &fd);
        if (hFind == INVALID_HANDLE_VALUE) {
            std::error_code ec(::GetLastError(), std::system_category());
            throw std::system_error(ec, "FindFirstFile failed.");
        }

        do {
            if ((current_directory_name != fd.cFileName)
                    && (parent_directory_name != fd.cFileName)) {
                auto fn = combine_path(cur, fd.cFileName);
                if (is_recursive && ((fd.dwFileAttributes
                        & FILE_ATTRIBUTE_DIRECTORY) != 0)) {
                    stack.push(fn);
                }
                if (predicate(fd)) {
                    *oit++ = fn;
                }
            }
        } while (::FindNextFileW(hFind, &fd) != 0);

        {
            auto e = ::GetLastError();
            if (e != ERROR_NO_MORE_FILES) {
                std::error_code ec(e, std::system_category());
                throw std::system_error(ec, "FindNextFile failed.");
            }
        }
    }

#else /* defined(_WIN32) */
    static const string_type current_directory_name(".");
    static const string_type parent_directory_name("..");
    struct dirent *fd;

    while (!stack.empty()) {
        auto cur = stack.top();
        stack.pop();

        auto dir = ::opendir(cur.c_str());
        if (dir == nullptr) {
            std::error_code ec(errno, std::system_category());
            throw std::system_error(ec, "opendir failed.");
        }

        while ((fd = ::readdir(dir)) != nullptr) {
            if ((current_directory_name != fd->d_name)
                    && (parent_directory_name != fd->d_name)) {
                auto fn = combine_path(cur, fd->d_name);
                if (is_recursive && (fd->d_type == DT_DIR)) {
                    stack.push(fn);
                }
                if (predicate(*fd)) {
                    *oit++ = fn;
                }
            }
        }

        ::closedir(dir);
    }
#endif /* defined(_WIN32) */
}


/// <summary>
/// Gets all file system entries below <paramref name="path" />.
/// </summary>
/// <typeparam name="TIterator"></typeparam>
/// <param name="oit"></param>
/// <param name="path"></param>
/// <param name="is_recursive"></param>
template<class TIterator>
inline void get_file_system_entries(_In_ TIterator oit,
        _In_ const std::basic_string<benchlab_char>& path,
        _In_ const bool is_recursive = false) {
#if defined(_WIN32)
    typedef WIN32_FIND_DATAW file_system_entry;
#else defined(_WIN32)
    typedef struct dirent file_system_entry;
#endif defined(_WIN32)

    ::get_file_system_entries(oit, path, is_recursive,
        [](const file_system_entry&) { return true; });
}
