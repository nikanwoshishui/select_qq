#include<iostream>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<fstream>
#include"/root/public_h/public.h"

/*select工作流程::申请一个可以容纳1024个socket描述符的fd集合;
每当增加socket时会把该描述符置为1表示该文件描述符需要检测;
当调用select函数时,该fd集合会从用户态拷贝到内核态,由内核来检测描述符中谁有事件发生;
有事件发生的描述符会被内核在fd集合中标志为1,无事件发生的描述符会在fd集合中置为0;
这就是为什么需要fd集合副本的原因;
*/


int main()
{
	CIULOG::init_ciulog(true,false);	//日志系统初始化

	fd_set sock, sock_copy;	//sock集合
	FD_ZERO(&sock_copy);	//初始化sock
	FD_ZERO(&sock);	//初始化sock

	SOCKET_FWD fwd;
	if(!fwd.init_socket(5051,"192.168.126.122"))
	{
		std::cout << "init_sock error\n";
		return -1;
	}

	if(listen(fwd.sock,5) != 0)
	{
		std::cout << "listen error\n";
		return -1;
	}

	int max = fwd.sock;		//更新集合中的最大描述符
	FD_SET(fwd.sock,&sock);	//把监听的socket加入到集合中
	char str[1024];	//str1用来存储sql语句
	MYSQL_ROW row;			//用来存储select的结果

	MY_MYSQL sql;	//连接数据库
	if(!sql.init_mysql("192.168.126.122","root","123","chese"))	return -1;
	if(!sql.sql_mysql("delete from qq_name"))	return -1;

	while(1)
	{
		memcpy(&sock_copy,&sock,sizeof(&sock));		//复制socket集合的副本
		//val返回的是socket发生变化的个数,不知道具体是哪一个socket发生变化
		int val = select(max+1,&sock_copy,NULL,NULL,NULL);

		if(val == -1)
		{
			std::cout << "select error\n";
			return -1;
		}
		else if(val == 0)	continue;

		//判断监听的socket是否在临时的sock_copy集合中,如果为真说明有新连接
		if(FD_ISSET(fwd.sock,&sock_copy))
		{
			struct sockaddr_in addr;
			int addrlen = sizeof(&addr);
			int tmp = accept(fwd.sock,(struct sockaddr*)&addr, (socklen_t*)&addrlen);

			if(tmp > max)	max = tmp;
			FD_SET(tmp,&sock);	//加入集合

			//加入群聊成功,发送"你加入群聊成功"
			memset(str,0,sizeof(str));
			strcpy(str,"you Join the group chat successfully");
			CIULOG::LOG_INFO(str);		//写入日志
			if(val = send(tmp,str,sizeof(str),0) <= 0)
			{
				std::cout << tmp << ":: disconnect\n";
				continue;
			}
			//优先修改数据库内新连接sock的值
			memset(str,0,sizeof(str));
			sprintf(str,"%d",tmp);
			insert_str("update qq_name set id=", str ," where id=1");
			if(!sql.sql_mysql(str))	return -1;

			//获取新连接的名字并发送至各个群成员
			memset(str,0,sizeof(str));
			sprintf(str,"%d",tmp);
			insert_str("select (name)from qq_name where id = ", str);	//获取该sock的名字
			row = 0;
			sql.select_mysql(str,&row);		//获取该连接的名字
			memset(str,0,sizeof(str));
			strcpy(str,row[0]);
			std::cout << str << " success\n";
			strcat(str," Joined group chat");	//"已加入群聊"
			CIULOG::LOG_INFO(str);		//写入日志

			//给所有群聊成员发送新成员加入成功
			for(int i=fwd.sock+1; i<max; i++)
			{
				if((FD_ISSET(i,&sock)>0) && (i != tmp))
				{
					if(val = send(i,str,strlen(str),0) < 0)
					{
						std::cout << i << ":: disconnect\n";
						continue;
					}
				}
			}

			continue;
		}

		for(int i = fwd.sock+1; i<=max; i++)	//遍历所有小于max以内描述符
		{
			if(FD_ISSET(i,&sock_copy))	//判断当前socket描述符是否在集合中
			{
				memset(str,0,sizeof(str));

				int tmp = recv(i,str,sizeof(str),0);
				if(tmp < 0)
				{
					std::cout << "recv error\n";
					return -1;
				}
				else if(tmp == 0)
				{
					std::cout << "client " <<  i << " closed...\n";
					FD_CLR(i,&sock);
					close(i);
					sprintf(str,"%d",i);
					insert_str("delete from qq_name where id in(",str,")");
					if(!sql.sql_mysql(str))	return -1;
					continue;
				}

				char name[100];
				memset(name,0,sizeof(name));
				sprintf(name,"%d",i);
				insert_str("select (name)from qq_name where id = ", name);	//获取该sock的名字
				row = 0;
				sql.select_mysql(name,&row);		//获取该连接的名字
				memset(name,0,sizeof(name));
				insert_str(row[0],name," send: ");
				insert_str(name,str);
				std::cout << str << std::endl;

				CIULOG::LOG_INFO(str);		//写入日志
				//给所有描述符集合内的sock发送有sock事件的文本内容
				for(int j = fwd.sock+1; j<=max; j++)
				{
					//判断当前socket描述符是否在集合中
					if((FD_ISSET(j,&sock_copy)==0) && (FD_ISSET(j,&sock)>0))
					{
						if(send(j,str,strlen(str),0) <= 0)
						{
							std::cout << j << " send error\n";	continue;
						}
					}
				}
			}
		}
	}
}
