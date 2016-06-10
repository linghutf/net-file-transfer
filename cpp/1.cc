#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <boost/enable_shared_from_this.hpp>

#include <stdio.h>

//using namespace std;

static bool debugmode = true;
static boost::mutex debug_mutex;

class async_tcp_client:boost::noncopyable{
    public:
        async_tcp_client(boost::asio::io_service& io_service,const std::string& server,const std::string& path ):
            resolver_(io_service),socket_(io_service){
                size_t pos = server.find(':');
                if(pos == std::string::npos){return ;}
            }
}
class Resource{
    private:
        Resource(const Resource &);
        Resource& operator=(const Resource &);

    public:
        Resource(int fd);
};
int main(void)
{

    return 0;
}
0