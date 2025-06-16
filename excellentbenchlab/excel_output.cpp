// <copyright file="excel_output.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 - 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "excel_output.h"

#include <sstream>


/*
 * excel_output::excel_output
 */
excel_output::excel_output(void) {
    this->open();
}


/*
 * excel_output::excel_output
 */
excel_output::~excel_output(void) { }


/*
 * excel_output::close
 */
void excel_output::close(void) {
    if (this->_excel != nullptr) {
        //// Set .Saved property of workbook to TRUE so we aren't prompted
        //// to save when we tell Excel to quit...
        //{
        //    VARIANT x;
        //    x.vt = VT_I4;
        //    x.lVal = 1;
        //    AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlBook, L"Saved", 1, x);
        //}

        invoke(NULL, DISPATCH_METHOD, this->_excel, L"Quit");

        this->_book.reset();
        this->_books.reset();
        this->_excel.reset();
        this->_sheet.reset();
    }
}


/*
 * excel_output::open
 */
void excel_output::open(void) {
    if (this->_excel == nullptr) {
        CLSID clsid;                // CLSID of Excel application.

        // Get CLSID of Excel.
        THROW_IF_FAILED(::CLSIDFromProgID(L"Excel.Application", &clsid));

        // Start Excel server.
        THROW_IF_FAILED(::CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER,
            IID_IDispatch, this->_excel.put_void()));

        // Get Workbooks collection.
        {
            wil::unique_variant output;
            invoke(&output, DISPATCH_PROPERTYGET, this->_excel, L"Workbooks");
            this->_books = output.pdispVal;
        }

        // Add a new workbook for the results.
        {
            wil::unique_variant output;
            invoke(&output, DISPATCH_PROPERTYGET, this->_books, L"Add");
            this->_book = output.pdispVal;
        }

        // Get the active sheet.
        {
            wil::unique_variant output;
            invoke(&output, DISPATCH_PROPERTYGET, this->_excel, L"ActiveSheet");
            this->_sheet = output.pdispVal;
        }

        // Cache the sensor names and write the header row.
        this->_sensors = visus::benchlab::get_power_sensors();

        {
            const long row = 0;
            long col = 0;
            this->write_value(L"Timestamp", row, col++);

            for (auto& s : this->_sensors) {
                this->write_value(s + L" [V]", row, col++);
                this->write_value(s + L" [A]", row, col++);
                this->write_value(s + L" [W]", row, col++);
                this->write_value(s + L" [S]", row, col++);
            }
        }
    }
}


/*
 * excel_output::save
 */
void excel_output::save(_In_ const std::wstring& path) {
    if (path.empty()) {
        invoke(nullptr, DISPATCH_METHOD, this->_book, L"Save");

    } else {
        wil::unique_variant input;
        input.vt = VT_BSTR;
        input.bstrVal = ::SysAllocString(path.c_str());
        invoke(nullptr, DISPATCH_METHOD, this->_book, L"SaveAs", input);
    }
}


/*
 * excel_output::visible
 */
excel_output& excel_output::visible(_In_ const bool visible) {
    wil::unique_variant input;
    input.vt = VT_I4;
    input.lVal = visible ? 1 : 0;
    invoke(nullptr, DISPATCH_PROPERTYPUT, this->_excel, L"Visible", input);
    return *this;
}


/*
 * excel_output::operator <<
 */
excel_output& excel_output::operator <<(_In_ const benchlab_sample& rhs) {
    if (this->_sheet == nullptr) {
        THROW_IF_FAILED(E_NOT_VALID_STATE);
    }

    auto row = this->last_row();
    long col = 0, col_i, col_u;

    this->write_value(rhs.timestamp, row, col++);

    for (std::size_t s = 0; s < this->_sensors.size(); ++s) {
        col_u = col++;
        col_i = col++;
        this->write_value(rhs.voltages[s], row, col_u);
        this->write_value(rhs.currents[s], row, col_i);
        this->write_value(rhs.power[s], row, col++);
        this->write_formula(cell_name(row, col_u) + L"*"
            + cell_name(row, col_u), row, col++);
    }

    return *this;
}


/*
 * excel_output::cell_name
 */
std::wstring excel_output::cell_name(_In_ const long row, _In_ const long col) {
    return column_name(col) + std::to_wstring(row + 1);
}


/*
 * excel_output::column_name
 */
std::wstring excel_output::column_name(_In_ const long col) {
    const long RANGE = L'Z' - L'A' + 1;
    long d = col + 1;
    long r = 0;
    std::wstring retval;

    while (d > 0) {
        r = (d - 1) % RANGE;
        retval = std::wstring(1, (L'A' + static_cast<wchar_t>(r))) + retval;
        d = (d - r) / RANGE;
    }

    return retval;
}


/*
 * excel_output::get_range
 */
wil::com_ptr<IDispatch> excel_output::get_range(_In_ const long first_row,
        _In_ const long first_col, _In_ const long last_row,
        _In_ const long last_col) {
    wil::unique_variant input;      // Input of automation call.
    wil::unique_variant output;     // Output of automation call.
    std::wstringstream range_spec;  // String representation of the retval.
    wil::com_ptr<IDispatch> retval; // Range of new row.

    // Prepare input.
    range_spec << excel_output::column_name(first_col) << (first_row + 1)
        << L":" << excel_output::column_name(last_col) << (last_row + 1)
        << std::ends;
    input.vt = VT_BSTR;
    input.bstrVal = ::SysAllocString(range_spec.str().c_str());

    // Make the call.
    assert(this->_sheet != nullptr);
    invoke(&output, DISPATCH_PROPERTYGET, this->_sheet, L"Range", input);
    retval = output.pdispVal;

    return retval;
}


/*
 * excel_output::get_used_range
 */
wil::com_ptr<IDispatch> excel_output::get_used_range(void) {
    wil::unique_variant output;
    assert(this->_sheet != nullptr);
    invoke(&output, DISPATCH_PROPERTYGET, this->_sheet, L"UsedRange");
    wil::com_ptr<IDispatch> retval(output.pdispVal);
    return retval;
}


/*
 * excel_output::last_row
 */
long excel_output::last_row(void) {
    // Get the used retval.
    auto range = this->get_used_range();

    // Retrieve the used rows.
    wil::unique_variant rows;
    invoke(&rows, DISPATCH_PROPERTYGET, range, L"Rows");
    range = rows.pdispVal;

    // Retrieve number of rows.
    wil::unique_variant count;
    invoke(&count, DISPATCH_PROPERTYGET, range, L"Count");
    auto retval = count.lVal - 1;

    return retval;
}


/*
 * excel_output::read_value
 */
void excel_output::read_value(_Out_ VARIANT& outValue,
        _In_ const long row, _In_ const long col) {
    // Get the retval to read from.
    auto range = this->get_range(row, col);
    assert(range != nullptr);

    // Get the value.
    ::VariantInit(&outValue);
    invoke(&outValue, DISPATCH_PROPERTYGET, range, L"Value");
}


/*
 * excel_output::write_formula
 */
void excel_output::write_formula(_In_ const std::wstring& formula,
        _In_ const long row, _In_ const long col) {

    // Prepare input.
    wil::unique_variant cell;
    cell.vt = VT_BSTR;
    cell.bstrVal = ::SysAllocString(formula.c_str());

    // Get the retval we write to.
    auto range = this->get_range(row, col);
    assert(range != nullptr);

    // Write the formula.
    invoke(nullptr, DISPATCH_PROPERTYPUT, range, L"Formula", cell);
}


/*
 * excel_output::write_value
 */
void excel_output::write_value(_In_ const wil::unique_variant& value,
        _In_ const long row, _In_ const long col) {
    // Get the cell we want to write to.
    auto range = this->get_range(row, col);
    assert(range != nullptr);

    // Write the property.
    invoke(nullptr, DISPATCH_PROPERTYPUT, range, L"Value", value);
}


/*
 * excel_output::write_value
 */
void excel_output::write_value(_In_ const float value, _In_ const long row,
        _In_ const long col) {
    wil::unique_variant v;
    v.vt = VT_R4;
    v.fltVal = value;
    this->write_value(v, row, col);
}


/*
 * excel_output::write_value
 */
void excel_output::write_value(_In_ const std::uint16_t value,
        _In_ const long row, _In_ const long col) {
    wil::unique_variant v;
    v.vt = VT_UI2;
    v.uiVal = value;
    this->write_value(v, row, col);
}


/*
 * excel_output::write_value
 */
void excel_output::write_value(_In_ const std::int64_t value,
        _In_ const long row, _In_ const long col) {
    wil::unique_variant v;
    v.vt = VT_I8;
    v.llVal = value;
    this->write_value(v, row, col);
}


/*
 * excel_output::write_value
 */
void excel_output::write_value(_In_ const std::wstring& value,
        _In_ const long row, _In_ const long col) {
    wil::unique_variant v;
    v.vt = VT_BSTR;
    v.bstrVal = ::SysAllocString(value.c_str());
    this->write_value(v, row, col);
}
