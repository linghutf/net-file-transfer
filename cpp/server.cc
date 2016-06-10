/**
 *
 * recv file from client as response
 * 传输文件名: 长度数字+filename
 * 传输文件内容
 */
#include "common.h"
#include "fileinfo.h"

typedef boost::asio::ip::tcp BTcp;

#define PORT 50013


class AsyncTcpConnection:public std::enable_shared_from_this<AsyncTcpConnection>{
    public:
        AsyncTcpConnection(boost::asio::io_service& service):
            socket_(service),bufsize_(BUFSIZ),filesize_(0)
    {

    }
        void start()
        {
            printf("%s...\n",__FUNCTION__);
            //recv fileninfo
            boost::asio::async_read(socket_,boost::asio::buffer(info_),
                    boost::bind(&AsyncTcpConnection::handle_read_request,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

        BTcp::socket& socket()
        {
            return socket_;
        }

    private:
        //request filename
        void handle_read_request(const boost::system::error_code& err,size_t bytes_transferred)
        {
            if(err){
                return handle_error(__FUNCTION__,err);
            }
            //read filename

            decode_fileinfo(filename_,info_);
            BOOST_LOG_TRIVIAL(debug) << "filename:"<<filename_;
            fp_ = fopen(filename_.c_str(),"wb");
            if(fp_ == NULL){
                fprintf(stderr,"open file ERROR.\n");
                return;
            }
            //recv file
            //能保证按序到达
            boost::asio::async_read(socket_,boost::asio::buffer(buf_,bufsize_),
                    boost::bind(&AsyncTcpConnection::handle_write_file,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

        }

        void handle_write_file(const boost::system::error_code& err,size_t bytes_transferred)
        {

            BOOST_LOG_TRIVIAL(debug) << "recv:"<<bytes_transferred;//收到了数据

            if(err){
                if(err == boost::asio::error::eof){
                    //一次发送就读完的数据单独处理
                    size_t n=fwrite(buf_,1,bytes_transferred,fp_);
                    filesize_+=n;//bytes_transferred;
                    BOOST_LOG_TRIVIAL(debug) << "filesize:"<<filesize_;
                    //关闭连接
                    socket_.close();
                }else{
                    handle_error(__FUNCTION__,err);
                }
                fclose(fp_);
                //fprintf(stderr,"recv ERROR:%s\n",err.message().c_str());
            }else{
                if(bytes_transferred>0){
                    //保证全部写入
                    //BOOST_LOG_TRIVIAL(debug)<<"recv size:"<<bytes_transferred;
                    size_t n=fwrite(buf_,1,bytes_transferred,fp_);
                    filesize_+=n;//bytes_transferred;
                }
                boost::asio::async_read(socket_,boost::asio::buffer(buf_,bufsize_),
                        boost::bind(&AsyncTcpConnection::handle_write_file,shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            }
        }

        void handle_error(const std::string& func_name,const boost::system::error_code& err)
        {
            if(err){
                fprintf(stderr,"ERROR:[%s]%s\n",func_name.c_str(),err.message().c_str());
            }
        }

    private:
        std::array<char,MAX_FILENAME_LEN> info_;

        std::string filename_;
        FILE *fp_;

        BTcp::socket socket_;
        char buf_[BUFSIZ];

        //std::array<char,BUFSIZ> buf_;
        size_t bufsize_;
        size_t filesize_;
};

class AsyncTcpServer:private boost::noncopyable{
    public:
        typedef std::shared_ptr<AsyncTcpConnection> AsyncTcpConnectionPtr;

        AsyncTcpServer(unsigned short port)
            :acceptor_(service_,BTcp::endpoint(BTcp::v4(),port),true){
                AsyncTcpConnectionPtr new_conn(new AsyncTcpConnection(service_));

                acceptor_.async_accept(new_conn->socket(),
                        boost::bind(&AsyncTcpServer::handle_accept,this,
                            new_conn,
                            boost::asio::placeholders::error));
                service_.run();

            }

        virtual ~AsyncTcpServer()
        {
            service_.stop();
        }

    private:
        void handle_accept(AsyncTcpConnectionPtr cur_conn,const boost::system::error_code& err)
        {
            if(!err){
                cur_conn->start();
            }
        }

    private:
        boost::asio::io_service service_;//has been initialized
        BTcp::acceptor acceptor_;
};

int main(int argc, char *argv[])
{
    try{
        if(argc!=2){
            fprintf(stderr,"usage:%s port.\n",argv[0]);
            return 1;
        }
        unsigned short port = atoi(argv[1]);
        printf("listening on port:%u...\n",port);
        std::shared_ptr<AsyncTcpServer> server(new AsyncTcpServer(port));
    }catch(std::exception& e)
    {
        fprintf(stderr,"ERROR:%s\n",e.what());
    }
    return 0;
}
