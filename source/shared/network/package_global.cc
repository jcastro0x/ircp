// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include "package_global.hh"

#include <shared/pch.hh>


std::string IRCP::network::dump_package_buffer(const PACKAGE& package, size_t lineWidth/* = 16*/, bool bUseASCIIExtended /*= false*/)
{
    const auto totalSize = static_cast<size_t>(PACKAGE::header_size_value + package.buffer.header.body_size.get());

    const std::string firstLine = "[" + std::to_string(totalSize) + " bytes]";          // header of output text
    const std::string lineAddress = "\n00000000 | ";                                    // address of 4 bytes in hex

    const auto bytesHex   = lineWidth*3;                                                // space to print bytes in hex format with one space
    const auto bytesChars = lineWidth+1;                                                // space to print bytes as chars with one space between bytes and chars and return char
    const auto line  = lineAddress.size() + bytesHex + bytesChars;                      // space to print one line, including address
    const auto lines = unsigned(totalSize/lineWidth) + (totalSize%lineWidth!=0 ? 1:0);  // total lines to print all bytes in groups of lineWidth bytes

    std::string output(line * lines + firstLine.size(), ' ');
    output.replace(0, firstLine.size(), firstLine);

//    if(package.buffer_.header.body_size > PACKAGE::buffer_max_size_value)
//    {
//        return "Overflow at header::body_size\n";
//    }

    for(unsigned i = 0; i < std::min(PACKAGE::buffer_max_size_value, totalSize); i++)
    {
        const auto posBeginAddress = firstLine.size() + line*(i/lineWidth);
        const auto posBeginBytes   = posBeginAddress  + lineAddress.size();
        const auto posBeginChars   = posBeginBytes    + bytesHex;

        // Print line address
        if(i % lineWidth == 0)
        {
            std::stringstream address;
            address << std::hex << std::setw(8) << std::setfill('0') << i;

            std::string thisAddress = lineAddress;
            thisAddress.replace(1, address.str().size(), address.str());

            output.replace(posBeginAddress, thisAddress.size(), thisAddress);
        }

        // Print bytes
        std::stringstream byte;
        byte << std::hex << std::setw(2) << std::setfill('0') << +package.buffer.raw_bytes[i] << " ";
        output.replace(posBeginBytes + 3*(i%lineWidth), byte.str().size(), byte.str());

        // Print byte's character
        if(package.buffer.raw_bytes[i] >= 32 && package.buffer.raw_bytes[i] < (bUseASCIIExtended?255:128) )
        {
            std::stringstream character;
            character << package.buffer.raw_bytes[i];
            output.replace(posBeginChars + i%lineWidth, 1, character.str());
        }
        else output.replace(posBeginChars + i%lineWidth, 1, ".");
    }

    return output;
}
