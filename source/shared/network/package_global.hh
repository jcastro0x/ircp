// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include <shared/network/package.hh>

#define PACKAGE IRCP::network::package<512, std::endian::big>

namespace IRCP::network
{
    /**
     * Dump all bytes from buffer into a string
     * @param lineWidth Max width of bytes before create new line
     * @param bUseASCIIExtended When dump, values from 128 to 254 will be printed
     * @return String with all bytes dumped
     */
    std::string dump_package_buffer(const PACKAGE& package, size_t lineWidth = 16, bool bUseASCIIExtended = false);
}
