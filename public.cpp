#include"public.h"

//在p2前面或则后面添加字符串
//实参与形参的结合是从左至右顺序进行的,因此指定默认值的参数必须放在形参列表这种的最右端,否则出错;
bool insert_str(const char* p1, char* p2, const char* p3)
{
	char tmp[1024];
	memset(tmp,0,sizeof(tmp));
	if(p1 != NULL)	strcpy(tmp,p1);
	strcat(tmp,p2);
	if(p3 != NULL)	strcat(tmp,p3);
	strcpy(p2,tmp);

        return true;
}
