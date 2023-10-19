// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include <shared/pch.hh>

#include <shared/utilities/constructor_policies.hh>
#include <shared/network/package_global.hh>


namespace IRCP::network
{
    /**
     * @bief Base class of all sockets.
     */
    class socket : private immovable
    {

    public:
        explicit socket(boost::asio::io_context& context);
        virtual ~socket();

    public:
        //~=============================================================================================================
        //
        //~=============================================================================================================

        void open();
        void close();

        bool is_closed() const;

    protected:
        virtual void process_incoming_package(PACKAGE&& buffer) = 0;

    private:
        void begin_read();
        void on_read_header(const boost::system::error_code& error);
        void on_read_body(const boost::system::error_code& error);

    public:
        //~=============================================================================================================
        // CASTING OPERATORS
        //~=============================================================================================================
        explicit operator boost::asio::ip::tcp::socket&()
        {
            return socket_;
        }

        explicit operator const boost::asio::ip::tcp::socket&() const
        {
            return socket_;
        }


    private:
        enum EStatus
        {
            Closed,         //< Closed
            idle,           //< Idle status. No write nor reading.
            Reading,        //< Reading from buffer.

            reading_header,
            reading_body,

            Writing,        //< Writing to buffer.
            Receiving,      //< Receiving from native socket
            Sending         //< Sending the buffer
        };

        EStatus socket_status_ = EStatus::idle;

    private:
        PACKAGE out_buffer_ {};
        PACKAGE in_buffer_  {};

        size_t requests_received_ {};

        mutable std::mutex unhandled_exception_mutex;
        std::exception_ptr exception_ptr;

        boost::asio::ip::tcp::socket socket_;
        boost::asio::deadline_timer read_body_timer_;

        bool bChecksum = false;
    };
};
