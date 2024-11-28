// <copyright file="serial.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2024 Visualisierungsinstitut der Universität Stuttgart.
// Licensed under the MIT licence. See LICENCE file for details.
// </copyright>
// <author>Christoph Müller</author>

#include "libbenchlab/serial.h"


/*
 * ::benchlab_initialise_serial_configuration
 */
HRESULT LIBBENCHLAB_API benchlab_initialise_serial_configuration(
        _In_ benchlab_serial_configuration *config) {
    if (config == nullptr) {
        return E_POINTER;
    }

    switch (config->version) {
        case 1:
            config->baud_rate = 115200;
            config->command_sleep = 10;
            config->data_bits = 8;
            config->dtr_enable = true;
            config->handshake = benchlab_handshake::none;
            config->parity = benchlab_parity::none;
            config->read_timeout = 500;
            config->rts_enable = true;
            config->stop_bits = benchlab_stop_bits::one;
            config->write_timeout = 500;
            return S_OK;

        default:
            return E_INVALIDARG;
    }
}
