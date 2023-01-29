#include"public.h"

MY_MYSQL::MY_MYSQL()
{
}

MY_MYSQL::~MY_MYSQL()
{
	mysql_close(&mysql);		//关闭数据库
	mysql_library_end();	//释放sql环境,终止使用mysql库

}

bool MY_MYSQL::init_mysql(const char *ip, const char *name, const char *passwd,const char *mysql_name)	//初始化并连接
{
	//初始化mysql句柄
	if((p_mysql = mysql_init(&mysql)) == false)
	{
		std::cout << "mysql_init error" << std::endl;
		return false;
	}

	//初始化mysql库
	if(mysql_library_init(0,NULL,NULL) != 0)
	{
		std::cout << "mysql_library error" << std::endl;
		return false;
	}

	//连接数据库
	if((p_sock = mysql_real_connect(p_mysql,ip,name,passwd,NULL,0,NULL,0)) == NULL)
	{
		std::cout << "mysql_real_connect error:" << mysql_error(p_mysql) << std::endl;
		return false;
	}
	
	//选择数据库
	if(mysql_select_db(p_sock,mysql_name) != 0)
	{
		std::cout << "mysql_select_db error:" << mysql_error(p_mysql) << std::endl;
		return false;
	}

	//设置编码格式
	mysql_query(&mysql,"SET NAMES utf8");
	std::cout << "mysql connect success" << std::endl;

	return true;
}

bool MY_MYSQL::sql_mysql(const char *sql)	//执行sql语句
{
	if(mysql_query(p_sock,sql) != 0)	//执行sql语句
	{
		std::cout << "sql_mysql mysql_query error\n";
		return false;
	}
	return true;
	
}

void MY_MYSQL::select_mysql(const char *sql,MYSQL_ROW *row)	//获取查询结果
{
	if(mysql_query(p_sock,sql) != 0)	//执行sql语句
	{
		std::cout << "mysql_query error:" << mysql_error(p_sock) << std::endl;
		return;
	}

	MYSQL_RES *res;		//结果集
	int columns = 0;	//记录结果集的列数
	int rows = 0;		//记录结果集的行数

	if((res = mysql_store_result(p_sock)) == false)	//获取查询结果数据
	{
		std::cout << "mysql_store_result error:" << mysql_error(p_sock) << std::endl;
		return;
	}
/*
	if((rows = mysql_num_rows(res)) < 0)	//获取结果集的行数
	{
		std::cout << "mysql_num_rows error:" << mysql_error(p_sock) << std::endl;
		return;
	}

	if(rows == 0)
	{
		std::cout << "NO data\n";
		return;
	}
*/
	if((columns = mysql_num_fields(res)) < 0)	//获取结果集中字段的数量
	{
		std::cout << "mysql_num_fields error:" << mysql_error(p_sock) << std::endl;
		return;
	}

	*row = mysql_fetch_row(res);	//获取结果集中行信息
	mysql_free_result(res);	//释放结果集
	return;
}

bool MY_MYSQL::print_mysql(const char *sql)	//打印查询结果
{
	if(mysql_query(p_sock,sql) != 0)	//执行sql语句
	{
		std::cout << "mysql_query error:" << mysql_error(p_sock) << std::endl;
		return false;
	}

	MYSQL_RES *res;		//结果集
	MYSQL_ROW row;		//数据行
	int columns = 0;	//记录结果集的列数
	int rows = 0;		//记录结果集的行数

	if((res = mysql_store_result(p_sock)) == false)	//获取查询结果数据
	{
		std::cout << "mysql_store_result error:" << mysql_error(p_sock) << std::endl;
		return false;
	}

	if((rows = mysql_num_rows(res)) < 0)	//获取结果集的行数
	{
		std::cout << "mysql_num_rows error:" << mysql_error(p_sock) << std::endl;
		return false;
	}

	if(rows == 0)
	{
		std::cout << "NO data\n";
		return false;
	}

	if((columns = mysql_num_fields(res)) < 0)	//获取结果集中字段的数量
	{
		std::cout << "mysql_num_fields error:" << mysql_error(p_sock) << std::endl;
		return false;
	}

	while((row = mysql_fetch_row(res)) != NULL)	//获取结果集中行信息
	{
		for(int i=0; i<columns; i++)	{std::cout << row[i] << '\t';}
		std::cout << std::endl;
	}

	mysql_free_result(res);	//释放结果集
	return true;
}
