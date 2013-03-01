#include "examples_common.h"

int libssh(ipinfo iplist){
    int rc;
    int nbytes;
    char buffer[1024];
    // ssh连接
    if(!(iplist->session=connect_ssh(iplist->ip,iplist->user,iplist->port,iplist->pawd))){
        fprintf(stderr, "ssh连接失败\n");
        ssh_finalize();
        return -1;
    }    
    // ssh通道建立
    if(!(iplist->channel=ssh_channel_new(iplist->session))){
        fprintf(stderr, "ssh通道建立失败\n");
        ssh_disconnect(iplist->session);
        ssh_free(iplist->session);
        ssh_finalize();
        return -1;
    }
    // 打开会话通道
    if ((rc=ssh_channel_open_session(iplist->channel))<0)
    {
        fprintf(stderr, "打开会话通道失败\n");
        goto failed;
    }
    // 执行命令
    if ((rc=ssh_channel_request_exec(iplist->channel,iplist->cmd))<0)
    {
        fprintf(stderr, "执行命令失败\n");
        goto failed;
    }
    while((nbytes=ssh_channel_read(iplist->channel,buffer,sizeof(buffer),0))>0){
        if (fwrite(buffer,1,nbytes,stdout) != (unsigned int)nbytes)
        {
            fprintf(stderr, "标准输出失败\n");
            goto failed;
        }
        nbytes=ssh_channel_read(iplist->channel,buffer,sizeof(buffer),0);
    }
    if(nbytes<0)
        goto failed;
    ssh_channel_send_eof(iplist->channel);
    ssh_channel_close(iplist->channel);
    ssh_channel_free(iplist->channel);
    ssh_disconnect(iplist->session);
    ssh_free(iplist->session);
    ssh_finalize();    
    return 0;
failed:
    ssh_channel_close(iplist->channel);
    ssh_channel_free(iplist->channel);
    ssh_disconnect(iplist->session);
    ssh_free(iplist->session);
    ssh_finalize();
    return 1;    
}
