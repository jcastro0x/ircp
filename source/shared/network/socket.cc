// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include "socket.hh"

#include <shared/utilities/exceptions.hh>

#include <boost/crc.hpp>


IRCP::network::socket::socket(boost::asio::io_context& context)
    : socket_(context)
    , read_body_timer_(context)
{
}

IRCP::network::socket::~socket()
{
}

void IRCP::network::socket::open()
{
    IRCP::logging::logging::trace("IRCP::network::socket::open");
    begin_read();
}

void IRCP::network::socket::close()
{
    IRCP::logging::logging::trace("IRCP::network::socket::close");
    assert(!is_closed());

    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    if(ec)
    {
        switch (ec.value())
        {
            default:
                try { throw IRCP::exceptions::socket_unhandled(ec.message(), ec.value()); }
                catch (...)
                {
                    IRCP::logging::logging::fatal("cache socket_unhandled_exception");
                    std::scoped_lock lock(unhandled_exception_mutex);
                    exception_ptr = std::current_exception();
                }
        }
    }

    socket_.close();
    socket_status_ = EStatus::Closed;
}

void IRCP::network::socket::begin_read()
{
    socket_status_ = EStatus::idle;
    if(is_closed()) return;

    IRCP::logging::logging::trace("socket::begin_read");

    socket_status_ = EStatus::reading_header;
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(in_buffer_.data(), PACKAGE::header_size_value),
        [&](const auto& ec, auto /*length*/){ on_read_header(ec); }
    );
}

void IRCP::network::socket::on_read_header(const boost::system::error_code& ec)
{
    IRCP::logging::logging::trace("socket::on_read_header");

    if(ec)
    {
        using enum boost::asio::error::basic_errors;
        using enum boost::asio::error::misc_errors;

        switch (ec.value())
        {
            case eof:
                IRCP::logging::logging::trace("listener::on_read_header eof");
                break;

            case connection_reset:
                IRCP::logging::logging::trace("listener::on_read_header connection_reset");
                break;

            case operation_aborted:
                IRCP::logging::logging::trace("listener::on_read_header operation_aborted");
                break;

            default:
                try { throw IRCP::exceptions::socket_unhandled(ec.message(), ec.value()); }
                catch (...)
                {
                    IRCP::logging::logging::fatal("cache socket_unhandled_exception");
                    std::scoped_lock lock(unhandled_exception_mutex);
                    exception_ptr = std::current_exception();
                }
        }

        close();
        return;
    }

    // Increment the requests received. For each time that this socket read something, we increment the value to can
    // check how much active is this socket.
    if(requests_received_ == std::numeric_limits<decltype(requests_received_)>::max())
    {
        IRCP::logging::logging::trace(std::format("socket {} overflow requests_received"
            , socket_.remote_endpoint().address().to_string()));
    }
    ++requests_received_;

    // Calculate checksum
    if(bChecksum)
    {
        boost::crc_32_type result;
        result.process_bytes(in_buffer_.data(), in_buffer_.buffer.header.body_size.get());
        if(in_buffer_.buffer.header.checksum.get() != result.checksum())
        {
            IRCP::logging::logging::error("checksum not satisfied");
            close();
            return;
        }
    }

    // Check over-sized
    if(in_buffer_.buffer.header.body_size.get() > PACKAGE::buffer_max_size_value)
    {
        auto p = in_buffer_.buffer.header.body_size.get();
        p = std::byteswap(p);

        IRCP::logging::logging::error(
            std::format("socket sent over-sized body size.\n"
                "body_size: {}\n"
                "buffer_max_size_value: {}",
                in_buffer_.buffer.header.body_size.get(), PACKAGE::buffer_max_size_value
            )
        );

        close();
        return;
    }

    // Update socket status
    socket_status_ = EStatus::reading_body;

    // Set up a limit-timer to stop to trying to read body if socket take too time to read the body bytes.
    read_body_timer_.expires_from_now(boost::posix_time::seconds(5));
    read_body_timer_.async_wait([&](const boost::system::error_code& ec)
    {
        if(!ec)
        {
            socket_.cancel();
        }
    });

    boost::asio::async_read(
        socket_,
        boost::asio::buffer(in_buffer_.data() + PACKAGE::header_size_value, in_buffer_.buffer.header.body_size.get()),
        [&](const auto& ec, auto /*length*/)
        {
            if(ec == boost::asio::error::operation_aborted)
            {
                IRCP::logging::logging::error("Too much time waiting for body content");
                close();
            }
            else
            {
                read_body_timer_.cancel();
                on_read_body(ec);
            }
        }
    );
}

void IRCP::network::socket::on_read_body(const boost::system::error_code& ec)
{
    IRCP::logging::logging::trace("socket::on_read_body");

    // Dump package's bytes
    IRCP::logging::logging::debug("dump_package_buffer: ");
    IRCP::logging::logging::debug("\n" + dump_package_buffer(in_buffer_, 16, true));

    // Flush pending bytes to read
    {
        boost::asio::streambuf buf;
        while (socket_.available())
        {
            socket_.read_some(buf.prepare(socket_.available()));
            buf.commit(socket_.available());
        }

//        boost::system::error_code flush_ec;
//        boost::asio::streambuf buf;
//        boost::asio::read(socket_, buf, boost::asio::transfer_all(), flush_ec);
//
//        if(flush_ec != boost::asio::error::misc_errors::eof)
//        {
//            try { throw IRCP::exceptions::socket_unhandled(flush_ec.message(), flush_ec.value()); }
//            catch (...)
//            {
//                BOOST_LOG_TRIVIAL(fatal) << "cache socket_unhandled_exception";
//                std::scoped_lock lock(unhandled_exception_mutex);
//                exception_ptr = std::current_exception();
//            }
//
//            close();
//            return;
//        }
    }

    socket_status_ = EStatus::idle;

    // Process package and re-run reading
    process_incoming_package(std::move(in_buffer_));
    begin_read();
}

bool IRCP::network::socket::is_closed() const
{
    return socket_status_ == EStatus::Closed;
}



/**

    TODO

- Enviar solo el hader, esperar 1 minuto y no enviar nada mas
- Enviar solo el hader, esperar 59 segundos y enviar el resto
- Enviar 0xFFFF como body_size
- Enviar 0x0000 como body_size
- Enviar mas bytes de lo indicados en body_size

*/