#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

int main()
{
    // 在#include <netinet/in.h>定义的数据结构，此数据结构用做bind、connect、recvfrom、sendto等函数的参数
    struct sockaddr_in serveraAddr , clientAddr;  // 服务端和客户端的sockaddr结构
    socklen_t clientAddrLen;   // 客户端地址长度
    int nFd = 0,linkFd = 0;
    int nRet = 0;
    int nReadLen = 0;
    char szBuff[BUFSIZ] = {0};

    /* 功能：创建socket，也就是创建一个socket描述符 
       原型：int socket(int family, int type, int protocol); /
       返回: socket描述字
       用法：family：协议族。常用的协议族有，AF_INET(IPV4)、AF_INET6(IPV6)、
                   AF_LOCAL（或称AF_UNIX，Unix域socket）、AF_ROUTE等。
            type：指定socket类型。常用的socket类型有，SOCK_STREAM、
                  SOCK_DGRAM、SOCK_RAW、SOCK_PACKET、SOCK_SEQPACKET等。
            protocol：指定协议。常用的协议有，IPPROTO_TCP、IPPTOTO_UDP、
                      IPPROTO_SCTP、IPPROTO_TIPC等，它们分别对应TCP传输协议、UDP传输协议、STCP传输协议、TIPC传输协议。
   */
    nFd = socket(AF_INET, SOCK_STREAM, 0);
    // 创建错误
    if(-1 == nFd) 
    {
        perror("socket:");
        exit(1);
    }
    printf("socket is created\n");

    /* 给本地的socket地址赋值 */
    memset(&serveraAddr, 0, sizeof(struct sockaddr_in));
    serveraAddr.sin_family = AF_INET; //以ipv4协议进行连接
    serveraAddr.sin_addr.s_addr = htonl(INADDR_ANY); //接收所有客户端ip的连接
    serveraAddr.sin_port = htons(8080);//绑定8080

    /* 当TCP的连接的状态是TCP_WAIT状态的时候， 可以通过设置SO_REUSEADDR
        选项来强制使用属于TIME_WAIT状态的连接的socket， 也就是即使在close后，
        而该socket缓冲区仍然有数据，也能让其他socket绑定
    */
    int isReuse = 1;
    setsockopt(nFd,SOL_SOCKET,SO_REUSEADDR,(const char*)&isReuse,sizeof(isReuse));

    /* 功能：把产生的socket套接字和本地的sock地址绑定起来 
       原型：int bind(int sock_fd, const struct sockaddr *addr, socklen_t addr_len);
       返回: 失败返回-1
       用法：  sockfd：socket函数产生的socket描述符 
              addr：本地的socket地址，下面会仔细讲解该结构体。 
              addrlen：addr结构体的长度，一般strlen(addr); 
   */
    nRet = bind(nFd, (struct sockaddr*)&serveraAddr, sizeof(serveraAddr));
    if(-1 == nRet)
    {
        perror("bind:");
        exit(1);
    }
    printf("bind socket is OK\n");

   /*  功能：当socket创建之后还不能立即被客户端连接，必须设置一个监听队列来等待客户端的连接 
       原型：int listen(int sock_fd, int conn_num);
       返回: 成功返回0，失败返回-1
       用法：  sockfd：socket函数产生的socket描述符 
              backlog: 设置内核监听的最大长度，一般设为1就可以了 
   */
    listen(nFd,1);

    /* 等待客户端发来的tcp连接 ,当客户端连接进来之后，返回两个之间的唯一的socket连接，存放在linkFd之中*/
    clientAddrLen = sizeof(struct sockaddr_in);

    /* 功能：接收客户端的连接，当没有客户端来连接的时候就阻塞等待 
       原型：int accept(int sock_fd, struct sockaddr *addr, socklen_t *addr_len); 
       返回: 返回连接描述字
       用法： sockfd 创建的的socket套接字 
             addr: 用来获取连接的客户端的socket地址 
             addrlen : 获得addr的长度 
   */
    linkFd = accept(nFd, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if(-1 == linkFd)
    {
        perror("accept:");
        exit(1);
    }
    printf("Server is accepting \n");


    /* 把连接进来的客户端地址和端口打印出来 */
    printf("commect %s %d successful\n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
     
    /* 循环的读取客户端发来的数据 */
    while(1)
    {
        memset(szBuff,0,BUFSIZ);
        nReadLen = read(linkFd, szBuff, BUFSIZ);
        if(nReadLen > 0)
        {
            printf("read data: %s\n",szBuff);
        }
    }

    return 0;
}


