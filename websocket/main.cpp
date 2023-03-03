#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/post.hpp>

#include <semaphore.h>

std::string timestamp()
{
    //const auto t = std::chrono::steady_clock::now();
    const auto t = std::chrono::system_clock::now();
    std::stringstream ss;
    ss << "[";
    ss << std::fixed << std::setprecision(6) << (double)(std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count()) / 1000000;
    ss << "] ";
    return ss.str();
}

class Connection
{
public:
    Connection(const std::string& port)
        : m_ioc(BOOST_ASIO_CONCURRENCY_HINT_SAFE)
        , m_work(boost::asio::make_work_guard(m_ioc))
        , m_strand(m_ioc)
        , m_resolver(m_ioc)
        , m_socket(m_ioc)
        , m_ws(m_socket)
        , m_response()
        , m_port(port)
    {
        if (sem_init(&m_sem, 0, 0) == -1)
        {
            std::cout << timestamp() << "sem_init failed" << std::endl;
        }
    }

    ~Connection()
    {
        sem_destroy(&m_sem);
    }

    void run()
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        m_ioc.run();
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    void onHandshake(const boost::system::error_code& ec)
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (ec)
        {
            std::cout << timestamp() << "onHandshake failed" << std::endl;
        }
        sem_post(&m_sem);
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    void onConnect(const boost::system::error_code& ec)
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (ec)
        {
            std::cout << timestamp() << "onConnect failed" << std::endl;
            sem_post(&m_sem);
            return;
        }

        int keepintvl = 5;
        int keepcnt = 5;
        int keepidle = 1;
        int keepalive = 1;
        if (setsockopt(m_socket.native_handle(), SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int)) ||
            setsockopt(m_socket.native_handle(), SOL_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(int)) ||
            setsockopt(m_socket.native_handle(), SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(int)) ||
            setsockopt(m_socket.native_handle(), SOL_TCP, SO_KEEPALIVE, &keepalive, sizeof(int)))
        {
            std::cout << timestamp() << "failed to configure socket" << std::endl;
        }

        m_ws.async_handshake_ex(std::string("127.0.0.1:") + m_port,
                                "/",
                                [this](boost::beast::websocket::request_type& req)
                                {
                                    req.set(boost::beast::http::field::user_agent, "tsd-rsi-client/1.0");
                                    req.version(11);
                                },
                                boost::asio::bind_executor(
                                    this->m_strand,
                                    std::bind(&Connection::onHandshake, this, std::placeholders::_1)));
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    void onResolve(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results)
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (ec)
        {
            std::cout << timestamp() << "onResolve failed" << std::endl;
            sem_post(&m_sem);
            return;
        }

        boost::asio::async_connect(
            m_ws.next_layer(),
            results.begin(),
            results.end(),
            boost::asio::bind_executor(m_strand, std::bind(&Connection::onConnect, this, std::placeholders::_1)));
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    int connect()
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        boost::asio::post(m_ioc, [this]()
        {
            this->m_resolver.async_resolve(
                "127.0.0.1",
                m_port,
                boost::asio::bind_executor(
                    this->m_strand,
                    std::bind(&Connection::onResolve, this, std::placeholders::_1, std::placeholders::_2)));
        });
        sem_wait(&m_sem);
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
        return 0;
    }

    void onClose(const boost::system::error_code& ec)
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (ec)
        {
            std::cout << timestamp() << "onClose failed" << std::endl;
        }
        sem_post(&m_sem);
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    void onRead(const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (ec == boost::asio::error::operation_aborted)
        {
            std::cout << timestamp() << "onRead failed" << std::endl;
            return;
        }

        if ((ec) || (bytes_transferred == 0))
        {
            std::cout << timestamp() << "onRead error: " << ec.value() << std::endl;
            std::cout << timestamp() << "onRead bytes: " << bytes_transferred << std::endl;
            return;
        }

        std::string data(boost::beast::buffers_to_string(m_response.data()));
        std::cout << timestamp() << "onRead data: " << data << std::endl;
        m_response.consume(m_response.size());

        read();

        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    int read()
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (!m_strand.running_in_this_thread())
        {
            boost::asio::post(m_ioc, [this]() {
                this->m_ws.async_read(
                    this->m_response,
                    boost::asio::bind_executor(
                        this->m_strand,
                        std::bind(&Connection::onRead, this, std::placeholders::_1, std::placeholders::_2)));
            });
        }
        else
        {
            m_ws.async_read(
                m_response,
                boost::asio::bind_executor(
                    m_strand,
                    std::bind(&Connection::onRead, this, std::placeholders::_1, std::placeholders::_2)));
        }
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
        return 0;
    }

    int disconnect()
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (!m_ws.is_open())
        {
            std::cout << timestamp() << "already closed" << std::endl;
            return 1;
        }

        boost::asio::post(m_ioc, [this]() {
            this->m_ws.async_close(
                boost::beast::websocket::close_code::normal,
                boost::asio::bind_executor(
                    this->m_strand,
                    std::bind(&Connection::onClose, this, std::placeholders::_1)));
        });

        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 30;

        int ret = sem_timedwait(&m_sem, &ts);
        if (ret == -1 && errno == ETIMEDOUT)
        {
            std::cout << timestamp() << __FUNCTION__ << " sem_wait timeout" << std::endl;
        }

        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
        return 0;
    }

    void stopConnection(const boost::system::error_code& ec)
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        if (ec != boost::system::errc::make_error_code(boost::system::errc::success))
        {
            boost::system::error_code _ec;
            m_ws.next_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, _ec);
            std::cout << timestamp() << "stopConnection shutdown" << std::endl;
            m_ws.next_layer().close(_ec);
            std::cout << timestamp() << "stopConnection close" << std::endl;
        }
        else
        {
            disconnect();
        }
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

    void shutdown()
    {
        std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
        stopConnection(boost::system::errc::make_error_code(boost::system::errc::success));

        m_work.reset();
        std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    }

private:
    boost::asio::io_context m_ioc;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work;

    boost::asio::io_context::strand m_strand;
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket m_socket;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket&> m_ws;
    boost::beast::multi_buffer m_response;

    const std::string m_port;
    sem_t m_sem;
};

int main(int argc, char* argv[])
{
    std::cout << timestamp() << __FUNCTION__ << " begin" << std::endl;
    std::string port("1337");
    if (argc > 1)
    {
        port = argv[1];
    }
    std::cout << "connection port: " << port << std::endl;

    Connection conn(port);
    std::thread t([&conn](){
        conn.run();
    });

    conn.connect();
    conn.read();

    std::this_thread::sleep_for(std::chrono::seconds(5));
    conn.shutdown();
    t.join();

    std::cout << timestamp() << __FUNCTION__ << " end" << std::endl;
    return 0;
}
