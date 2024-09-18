#include <iostream>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void session(tcp::socket sock)
{
    try
    {
        while (true)
        {
            char data[1024];

            // 클라이언트로부터 데이터 수신
            boost::system::error_code error;
            size_t length = sock.read_some(boost::asio::buffer(data), error);

            if (error == boost::asio::error::eof)
            {
                // 연결이 정상적으로 종료됨
                break;
            }
            else if (error)
            {
                // 기타 오류 발생
                throw boost::system::system_error(error);
            }

            // 받은 데이터 출력
            std::cout << "Received: " << std::string(data, length) << std::endl;

            // 클라이언트에게 동일한 데이터 전송
            boost::asio::write(sock, boost::asio::buffer(data, length));
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

int main()
{
    try
    {
        // Boost.Asio I/O 컨텍스트 생성
        boost::asio::io_context io_context;

        // 서버가 리스닝할 포트 번호
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        while (true)
        {
            // 클라이언트의 연결 요청 수락
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            // 클라이언트와의 세션을 처리하는 스레드 실행
            std::thread(session, std::move(socket)).detach();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
