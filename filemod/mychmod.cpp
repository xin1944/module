#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc,char* argv[])
{
if(argc != 2)
{
printf("Usage:%s filename\n",argv[0]);
return -1;
}
if(chmod(argv[1],0666) != 0)
{
std::cout<<argv[1]<<" chmod failed\n";
return -1;
}
//perror("chmod"),exit(-1);

printf("chmod %s success.\n",argv[1]);

return 0;
}

