#include"/root/public_h/public.h"
#include<pthread.h>
#include<stdio.h>

void *pth_main(void *fd);

int main()
{
        SOCKET_KHD sock;
	MY_MYSQL sql;
	if(!sql.init_mysql("192.168.126.122","root","123","chese"))	//连接数据库
	{
		std::cout << "mysql init error\n";
		return -1;
	}

	CIULOG::init_ciulog(true,false);	//日志系统初始化

	char name[50];
	std::cout << "Please enter your name: ";	//输入名字
end:
	memset(name,0,sizeof(name));
	int i=0;

	//字符串输入空格也要保存
	//std::cin.getline(name,sizeof(name));
	while((name[i] = getchar()) != '\n')	i++;
	name[i] = 0;
	//gets(name);

	char nametmp[100];
	memset(nametmp,0,sizeof(nametmp));
	strcpy(nametmp,name);
	insert_str("select (name)from qq_name where name = '",nametmp,"'");

	MYSQL_ROW row;
	sql.select_mysql(nametmp,&row);
	if(row != NULL)
	{
		std::cout << "(already exists) Please re-enter::";	//已存在请重新输入
		goto end;
	}
        //连接成功优先向数据库写name和id
        memset(nametmp,0,sizeof(nametmp));
        strcpy(nametmp,name);
        insert_str("'",nametmp,"'");
        insert_str("insert into qq_name values(1,",nametmp,")");        //组合sql语言,插入id和name,id固定为1
        //如果插入错误一直重新插入,直到插入成功,出现错误表示数据库内文件尚未处理
        while(!sql.sql_mysql(nametmp)); 

	CIULOG::LOG_INFO(name);

	//连接sock
	if(!sock.init_socket(5051,"192.168.126.122"))   return -1;

	pthread_t pthid;	//创建线程
	if(pthread_create(&pthid,0,pth_main,(void*)(long)sock.sock) != 0)
	{
		std::cout << "pthread_create error\n";
		return -1;
	}

	char str[1000];
	while(1)
	{
		memset(str,0,sizeof(str));
		std::cin.getline(str,sizeof(str));
		//gets(str);
		if(str[0] == 0)	continue;
		if(!sock.send_socket(str))	return -1;
	}
	return 0;
}

void *pth_main(void *fd)
{
	int fhd = (long)fd;

	char str[1024];
	while(1)
	{
		memset(str,0,sizeof(str));
		int val = recv(fhd,str,sizeof(str),0);
		if(val == -1)
			continue;
		else if (val > 0)
		{
			std::cout <<  str << std::endl;
			CIULOG::LOG_INFO(str);
		}
	}
}
