#include "examples_common.h"

int sshcmd(MYSQL_ROW rows,char *argv1){
    ipinfo iplist;
    // iplist动态分配内存
    if ((iplist=malloc(sizeof(struct iplist))) == NULL)
    {
        fprintf(stderr, "iplist结构动态分配内存错误\n");
        exit(1);
    }
    // ip信息分别赋值
    iplist->ip=rows[1];
    iplist->user=rows[2];
    iplist->pawd=rows[3];
    iplist->port=rows[4];
    iplist->cmd=argv1;
    // 执行命令
    if (libssh(iplist)<0)
    {
        fprintf(stderr, "执行失败,请检查\n");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    MYSQL_RES *res;
    MYSQL_ROW rows;
    
    // 命令行
    if (argc != 2)
    {
        fprintf(stderr, "请使用: ./ssh 命令\n");
        return -1;
    }
    // 查询数据库
    char *str="select * from iplist";
    if ((res=query_mysql(str)) == NULL)
    {   
        fprintf(stderr, "数据库结果集返回错误\n");
        return -1; 
    }   
    // 获取结果集内容
    while((rows=mysql_fetch_row(res))){
        if (sshcmd(rows,argv[1]) < 0)
        {   
            fprintf(stderr, "执行命令前的分配信息失败,请检查\n%s 执行失败\n",rows[1]);
            return -1; 
        }   
    }  
    return 0;
}
