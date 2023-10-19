// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include <shared/pch.hh>

namespace IRCP::network
{
    /**
     * Concept used to arithmetics values less than 32 bits.
     */
    template <typename T>
    concept ArithmeticConcept = std::is_arithmetic<T>::value && (sizeof(T) <= sizeof(uint32_t));

    /**
     * Hold any type of data in endianness agnostic.
     * When set and get, perform a std::byteswap (C++23) onto the value to always be
     * aligned with platform endianness.
     */
    template <typename T, auto endianness = std::endian::big>
    struct platform_agnostic_value
    {
        using value_type = T;

        platform_agnostic_value() noexcept = default;

        platform_agnostic_value(platform_agnostic_value&&) = delete;
        platform_agnostic_value& operator=(platform_agnostic_value&&) = delete;

        explicit platform_agnostic_value(const value_type& t) noexcept
        {
            using enum std::endian;
            if constexpr(native != endianness) value = std::byteswap(t);
            else                               value = t;
        }

        platform_agnostic_value& operator=(const value_type& t) noexcept
        {
            using enum std::endian;
            if constexpr(native != endianness) value = std::byteswap(t);
            else                               value = t;

            return *this;
        }

        value_type get() const noexcept
        {
            using enum std::endian;
            if constexpr(native != endianness) return std::byteswap(value);
            else                               return value;
        }

        template<ArithmeticConcept V>
        platform_agnostic_value& operator-=(V decrement) noexcept
        {
            value -= decrement;
            return *this;
        }

        template<ArithmeticConcept V>
        platform_agnostic_value& operator+=(V decrement) noexcept
        {
            value += decrement;
            return *this;
        }

        explicit operator value_type() const noexcept
        {
            return get();
        }

    private:
        T value;
    };

    /**
     * @note According to RFC-1700 Section 2.1, the network byte order is defined as big-endian. But package struct
     * allow custom endianness. We recommend to use the same as your server machine and delegate in clients the
     * conversion.
     *
     * Refer to the following URL for detailed information:
     * https://datatracker.ietf.org/doc/html/rfc1700
     *
     * @see http://number-none.com/product/Scalar%20Quantization/
     * @see ail2ban, iptables o firewalld en Linux. (ping flood y DoS). Muchas ISP y routers controlar el "ping flood"
     **/
    template<size_t buffer_max_size = 512, auto endianness = std::endian::big>
    struct package final
    {
        template<typename data_t>
        class package_overflow_exception final : public std::runtime_error
        {
            public: explicit package_overflow_exception() : std::runtime_error("package overflow exception") {}
        };

        using opcode_t   = platform_agnostic_value<uint16_t, endianness>;
        using bodysize_t = platform_agnostic_value<uint16_t, endianness>;
        using checksum_t = platform_agnostic_value<uint32_t, endianness>;

        union
        {
            struct header
            {
                bodysize_t body_size;
                opcode_t op_code;
                checksum_t checksum;
            };

            mutable header header {};
            std::array<uint8_t, buffer_max_size> raw_bytes;

        } buffer;

        static constexpr size_t header_size_value     = sizeof(buffer.header);
        static constexpr size_t buffer_max_size_value = buffer_max_size - header_size_value;


        static_assert(buffer_max_size_value <= (2 << (sizeof(buffer.header.body_size)*8))
        , "buffer_max_size must have a size that header.bodySize is able to point");

    public:
        //~=============================================================================================================
        // Constructors
        //~=============================================================================================================
        explicit package() noexcept
        {
            std::ranges::fill(
                buffer.raw_bytes.begin(),
                buffer.raw_bytes.end(),
                typename decltype(buffer.raw_bytes)::value_type{}
            );
        }

        explicit package(opcode_t::value_type opcode) noexcept : package()
        {
            buffer.header.op_code = opcode;
        }

        package(package&& package) noexcept
        {
            buffer.raw_bytes.swap(package.buffer_.raw_bytes);
        }

        package& operator=(package&& package) noexcept
        {
            buffer.raw_bytes.swap(package.buffer_.raw_bytes);
            return *this;
        }

        // No copy/assigment allowed
        package(const package&) = delete;
        package& operator=(const package&) = delete;

        // Destructor
        ~package() = default;

    public:
        //~=============================================================================================================
        // Push operations
        //~=============================================================================================================

        /**
         * Generic push arithmetic values into the package.
         * @param in data to be added into the package
         */
        template<ArithmeticConcept data_t>
        package& operator<<(const data_t& in)
        {
            constexpr auto in_size = sizeof(data_t); // not ODR-used
            if(buffer.header.body_size.get() + in_size > buffer_max_size) throw package_overflow_exception<data_t>();

            using enum std::endian;
            if constexpr(sizeof(data_t) != 1 && native != endianness)
            {
                data_t copy = std::byteswap(in);
                copyToBuffer(&copy, in_size);
            }
            else
            {
                copyToBuffer(&in, in_size);
            }

            return *this;
        }


    public:
        //~=============================================================================================================
        // Pop operations
        //~=============================================================================================================

        /**
         * Generic pop Arithmetic-value from the package.
         * @param in Reference to be populated from the package
         */
        template<ArithmeticConcept data_t>
        const package& operator>>(data_t& out) const
        {
            constexpr auto in_size = sizeof(data_t); // not ODR-used
            if(in_size > static_cast<size_t>(buffer.header.getBodySize())) throw package_overflow_exception<data_t>();

            buffer.header.bodySize -= in_size;
            std::memcpy(&out, buffer.data() + (buffer.header.body_size.get() + sizeof(buffer.header), in_size));

            using enum std::endian;
            if constexpr(sizeof(data_t) != 1 && native != endianness)
            {
                out = std::byteswap(out);
            }

            return *this;
        }

    public:
        uint8_t* data()
        {
            return buffer.raw_bytes.data();
        }

        const uint8_t* data() const
        {
            return buffer.raw_bytes.data();
        }

    private:
        //~=============================================================================================================
        // HANDLE BUFFER
        //~=============================================================================================================
        inline consteval void copyToBuffer(const void* source, const size_t size) noexcept
        {
            std::memcpy(buffer.raw_bytes.data() + buffer.header.bodySize + package::header_size_value, source, size);
            buffer.header.bodySize += static_cast<decltype(buffer.header.bodySize)::value_type>(size);
        }
    };
};
