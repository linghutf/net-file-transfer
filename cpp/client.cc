/**
 *
 * send file to server
 * 查找服务器->连接服务器->读取文件信息->发送文件名->读文件，发送文件数据
 */
#include "common.h"
#include "fileinfo.h"

typedef boost::asio::ip::tcp BTcp;

class AsyncTcpClient{
    public:
        AsyncTcpClient(boost::asio::io_service& service,const std::string& server_host,const std::string& path):
            filesize_(0),resolver_(service),socket_(service)
    {
        filename_ = path;
        fp_= fopen(filename_.c_str(),"rb");
        if(fp_==NULL){
            fprintf(stderr,"cannot find file:%s\n",filename_.c_str());
            return;
        }

        size_t pos = server_host.find(':');
        if(pos == std::string::npos){
            fprintf(stderr,"server host parsed ERROR!\n");
            return;
        }
        port_ = server_host.substr(pos+1);
        server_host_ = server_host.substr(0,pos);

        BTcp::resolver::query query(server_host_,port_);
        resolver_.async_resolve(query,
                boost::bind(&AsyncTcpClient::handle_resolve,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
        //boost::asio::ip::address::from_string(host)
    }
        virtual ~AsyncTcpClient()
        {
            socket_.close();

        }

    private:
        void handle_resolve(const boost::system::error_code& err,
                BTcp::resolver::iterator endpoint_iterator)
        {
            if(!err){
                BTcp::endpoint endpoint = *endpoint_iterator;
                socket_.async_connect(endpoint,
                        boost::bind(&AsyncTcpClient::handle_connect,
                            this,
                            boost::asio::placeholders::error,
                            ++endpoint_iterator));
            }else{
                fprintf(stderr,"resolve ERROR:%s\n",err.message().c_str());
            }
        }

        void handle_connect(const boost::system::error_code& err,
                BTcp::resolver::iterator endpoint_iterator)
        {
            if(!err){
                //send filename
                std::array<char,MAX_FILENAME_LEN> buf;
                encode_fileinfo(buf,basename(const_cast<char*>(filename_.c_str())));
                //send fileinfo
                boost::asio::async_write(socket_,boost::asio::buffer(buf),
                        boost::bind(&AsyncTcpClient::handle_read_file,
                            this,
                            boost::asio::placeholders::error));

            }else if(endpoint_iterator != BTcp::resolver::iterator()){
                socket_.close();
                BTcp::endpoint endpoint = *endpoint_iterator;
                //connect another
                socket_.async_connect(endpoint,
                        boost::bind(&AsyncTcpClient::handle_connect,
                            this,
                            boost::asio::placeholders::error,++endpoint_iterator));

            }else{
                fprintf(stderr,"connect ERROR:%s\n",err.message().c_str());
            }
        }

        void handle_read_file(const boost::system::error_code& err)
        {
            if(!err){
                //send file
                size_t n = 0;
                if((n=fread(buf_,1,BUFSIZ,fp_))>0){
                    filesize_+=n;
                    BOOST_LOG_TRIVIAL(debug)<<"send size:"<<n;
                    /*
                    for(int i=0;i<n;++i){
                        printf("%x",buf_[i]);
                    }
                    printf("\n");
                    */
                    boost::asio::async_write(socket_,
                            boost::asio::buffer(buf_,n),
                            boost::bind(&AsyncTcpClient::handle_read_file,
                                this,
                                boost::asio::placeholders::error));
                }else if(n==0){//eof
                    fclose(fp_);
                    BOOST_LOG_TRIVIAL(debug)<<"total size:"<<filesize_;
                    //主动关闭写端
                    socket_.shutdown(boost::asio::socket_base::shutdown_send);
                }
            }else{
                //关闭连接
                if(err == boost::asio::error::eof){
                    socket_.close();
                }else{
                    fprintf(stderr,"send file ERROR:%s\n",err.message().c_str());
                }
            }
        }

    private:
        std::string server_host_;
        std::string port_;
        std::string filename_;
        size_t filesize_;
        FILE *fp_;
        char buf_[BUFSIZ];

        //boost::asio::streambuf request_;
        BTcp::resolver resolver_;
        BTcp::socket socket_;
};

int main(int argc, char *argv[])
{
    if(argc!=3){
        fprintf(stderr,"usage:%s host:port filename.\n",argv[0]);
        return 1;
    }

    try{
        boost::asio::io_service service;
        std::string host = argv[1];
        std::string filename = argv[2];
        AsyncTcpClient client(service,host,filename);
        service.run();
    }catch(std::exception& e){
        fprintf(stderr,"ERROR:%s\n",e.what());
    }

    return 0;
}
