#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
void * thread1(void *arg)
{
    int nReadLen = 0;
    char szBuff[BUFSIZ] = {0};
    int nFd = (int)(*((int*)arg));  // 获取主线程传递的nFd的值

    sleep(1);
    while(1)
    {
        /* 从终端读取数据 */
        memset(szBuff,0,BUFSIZ);
        nReadLen = read(STDIN_FILENO,szBuff,BUFSIZ);
        if (nReadLen > 0)
        {
            write(nFd,szBuff,strlen(szBuff));
        }
    }

}

void * thread2(void *arg)
{
    long t = time(0);
    char* s = ctime(&t);
    size_t size = strlen(s) * sizeof(char);
    int nFd = (int)(*((int*)arg));

    if( write(nFd, s, size) != size)
        perror("write error");
}

int main(int argc,char* argv[])
{
    struct sockaddr_in serverAddr;
    int nRet = 0;

    /* 判断有没有输入要连接的ip地址 */
    if (argc == 1)
    {
        perror("please input the ip address that you want to connect\n");
        exit(1);
    }

    /* 创建套接字描述符 */
    int nFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == nFd)
    {
        perror("socket:");
        exit(1);
    }
    printf("socket is created\n");
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(8080);//默认以8080端口连接

    /* 功能：获取连接服务器端
       原型：int connect(int sock_fd, struct sockaddr *addr,int addr_len);
       返回: 失败返回-1
       用法：  sockfd：socket函数产生的socket描述符 
              addr：连接目标服务器的协议族
              addrlen：addr结构体的长度，一般strlen(addr); 
   */
    nRet = connect(nFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (nRet == -1)
    {
        printf("connect:");
        exit(1);
    }

    int err;
    pthread_t tid1, tid2;
    void *tret;
  
    //  创建线程1， 并将 nFd 传递给线程1
    err = pthread_create(&tid1, NULL, thread1, &nFd);
    if(err != 0)
    {
        printf("create thread 1 is error");
        exit(1);
    }
    //  创建线程2
    err = pthread_create(&tid2, NULL, thread2, &nFd);
    if(err != 0)
    {
        printf("create thread 2 is error");
        exit(1);
    }

    // 主线程等待线程1 结束才结束。不至于导致主线程先结束而导致线程1被迫终止
    pthread_join(tid1, &tret);
    return 0;
}
