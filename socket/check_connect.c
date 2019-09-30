/*客户端设置非阻塞，然后判断链接是否成功*/
int SocketConnectWithTimeout
(
    int                 mySocket,          
    struct mySocketaddr     *adrs,        
    int                 adrsLen,        
    struct timeval      *timeVal        
)
{
    int     flag;
    fd_set  writeFds;
    int     remotPeerAdressLen;
    struct  mySocketaddr remotPeerAdress;
    
    if(timeVal == NULL)
    {
        return (connect(mymySocket, adrs, adrsLen));
    }
        
    flag = fcntl(mySocket, F_GETFL, 0); 
    fcntl(mySocket, F_SETFL, flag | O_NONBLOCK);//修改当前的flag标志为给阻塞
    
    //对于非阻塞式套接字，如果调用connect函数会之间返回-1（表示出错），且错误为EINPROGRESS，表示连接建立
    if(connect(mySocket, adrs, adrsLen) < 0)
    {
       //当使用非阻塞模式的时候，如果链接没有被立马建立，则connect()返回EINPROGRESS
        if(errno == EINPROGRESS)
        {
        //select是一种IO多路复用机制，它允许进程指示内核等待多个事件的任何一个发生，并且在有一个或者多个事件发生或者经历一段指定的时间后才唤醒它。
        //connect本身并不具有设置超时功能，如果想对套接字的IO操作设置超时，可使用select函数。此时我们使用不断的检测writeFds来判断链接的建立？
            FD_ZERO(&writeFds);
            FD_SET((unsigned int)mySocket, &writeFds); 
    
            if(select(FD_SETSIZE, (fd_set *)NULL, &writeFds, (fd_set *)NULL, timeVal) > 0)
            {
                //select()成功了，查看mySocketet是否可写（关键）
                if (FD_ISSET ((unsigned int)mySocket, &writeFds))
                {
                //已经可写了，此时我们要通过使用getpeername()判断是否真正的链接成功，如果返回值不是-1；
                //说明connect()成功了。
                    remotPeerAdressLen = sizeof (remotPeerAdress);
                    if(getpeername (mySocket, &remotPeerAdress, &remotPeerAdressLen) != ERROR)
                    {
                        return OK；
                    }
                    else
                    {
                        return ERROR；
                    }
                }
            }
        }
        else
        {
            return ERROR；
        }
    }
 
    fcntl(mySocket, F_SETFL, flag);//恢复标志位为阻塞
}
/*
1.将打开的socket设为非阻塞的,可以用fcntl(socket, F_SETFL, O_NDELAY)完成(有的系统用FNEDLAY也可).
2.发connect调用,这时返回-1,但是errno被设为EINPROGRESS,意即connect仍旧行还没有完成. 
3.将打开的socket设进被监视的可写(注意不是可读)文件集合用select进行监视,如果可写用getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, sizeof(int)); 来得到error的值,如果为零,则connect成功.
*/