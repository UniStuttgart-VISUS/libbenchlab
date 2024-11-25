// <copyright file="deivce.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>


/*
 * benchlab_device::ports
 */
template<class TIterator>
HRESULT benchlab_device::ports(_In_ TIterator oit) {
#if defined(_WIN32)
    wil::unique_hkey hkey;
    auto hr = wil::reg::open_unique_key_nothrow(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Enum\\USB\\VID_0483&PID_5740",
        hkey);
    if (FAILED(hr)) {
        return hr;
    }

    for (auto& c : wil::make_range(wil::reg::key_iterator(hkey.get()),
            wil::reg::key_iterator())) {
        wil::unique_hkey k, l;
        wil::unique_bstr s;

        if (FAILED(wil::reg::open_unique_key_nothrow(hkey.get(),
                c.name.c_str(),
                k))) {
            continue;
        }

        if (FAILED(wil::reg::open_unique_key_nothrow(k.get(),
                L"Device Parameters",
                l))) {
            continue;
        }

        if (FAILED(wil::reg::get_value_expanded_string_nothrow(l.get(),
                L"PortName",
                s))) {
            continue;
        }

        *oit++ = std::wstring(s.get());
    }

    return S_OK;

#else /* defined(_WIN32) */
    string[] ttyList = Directory.GetDirectories("/sys/bus/usb-serial/devices/");

    foreach(string portFile in ttyList) {
        string realPortFile = Mono.Unix.UnixPath.GetRealPath(portFile);
        string checkPath = Path.GetFullPath(Path.Combine(realPortFile, "../uevent"));
        string[] lines = File.ReadAllLines(checkPath);
        string productLine = lines.First(l = > l.StartsWith("PRODUCT="));
        string[] productSplit = productLine.Remove(0, 8).Trim().Split('/');

        if (productSplit[0] == "1a86" && productSplit[1] == "7523") {
            string portDevPath = Path.Combine("/dev/", Path.GetFileName(portFile));
            ports.Add(portDevPath);
        }
    }
    return E_NOTIMPL;
#endif /* defined(_WIN32) */
}
