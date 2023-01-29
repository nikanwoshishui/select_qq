#include"public.h"

/********SOCKET_FWD**********/
SOCKET_FWD::SOCKET_FWD()
{
	sock =0;
	sock_khd = 0;
}

SOCKET_FWD::~SOCKET_FWD()
{
	close(sock);
	close(sock_khd);
}

bool SOCKET_FWD::init_socket(int port_val,const char* ip)
{
        //申请socket
        sock = socket(AF_INET,SOCK_STREAM,0);
        if(sock < 0)
        {
                std::cout << "socket error\n";
                return false;
        }

        struct sockaddr_in sock_in;
        memset(&sock_in,0,sizeof(sock_in));
        sock_in.sin_family = AF_INET;   //协议族
        sock_in.sin_port = htons(port_val);     //端口号,>需要手动开通
        if(ip == "INADDR_ANY")
                sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
        else
                sock_in.sin_addr.s_addr = inet_addr(ip);

        if(bind(sock,(struct sockaddr *)&sock_in,sizeof(sock_in)) != 0)
        {
                std::cout << "bind error\n";
                close(sock);
                return false;
        }
        return true;
}

bool SOCKET_FWD::accept_socket()
{
        if(listen(sock,5) != 0)
        {
                std::cout << "listen error\n";
                return false;
        }

        struct sockaddr_in addr;
        int addr_size = sizeof(&addr);
        sock_khd = accept(sock,(struct sockaddr *)&addr,(socklen_t *)&addr_size);

        if(sock_khd < 0)
        {
                std::cout << "accept error\n";
                return false;
        }

        std::cout << inet_ntoa(addr.sin_addr) << "连接成功\n";
        return true;
}

bool SOCKET_FWD::send_socket(const char* str)
{
        if(!send(sock_khd,str,strlen(str),0))
        {
                std::cout << "send error\n";
                return false;
        }
        return true;
}

bool SOCKET_FWD::recv_socket(char* str)
{
        if(!recv(sock_khd,str,sizeof(str),0))
        {
                std::cout << "recv error\n";
                return false;
        }
        return true;
}


/**************SOCKET_KHD*******/
SOCKET_KHD::SOCKET_KHD()
{
	sock = 0;
}

SOCKET_KHD::~SOCKET_KHD()
{
	close(sock);
}


bool SOCKET_KHD::init_socket(int port_val, const char* ip)
{
        //申请socket
        sock = socket(AF_INET,SOCK_STREAM,0);
        if(sock < 0)
        {
                std::cout << "socket error\n";
                return false;
        }

        struct sockaddr_in sock_in;
        hostent* h;

        if((h = gethostbyname(ip)) == 0)
        {
                std::cout << "gethostbyname error";
                close(sock);    return false;
        }

        memset(&sock_in,0,sizeof(sock_in));

        sock_in.sin_family = AF_INET;   //协议族
        sock_in.sin_port = htons(port_val);     //端口号,>需要手动开通
        memcpy(&sock_in.sin_addr,h->h_addr,h->h_length);

        if(connect(sock,(struct sockaddr *)&sock_in, sizeof(sock_in)) < 0)
        {
                std::cout << "connect error\n";
                close(sock);    return false;
        }
                std::cout << "connect monitor success\n";
        return true;
}

bool SOCKET_KHD::send_socket(const char* str)
{
        if(!send(sock,str,strlen(str),0))
        {
                std::cout << "send error\n";
                return false;
        }
        return true;
}
bool SOCKET_KHD::recv_socket(char* str)
{ 
	if(!recv(sock,str,sizeof(str),0))
        {
                std::cout << "recv error\n";
                return false;
        }
        return true;
}

