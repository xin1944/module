#include<iostream>
#include<sys/statvfs.h>
#include<sys/statfs.h>
#include<sys/stat.h>

int main()
{
        struct statvfs vfs_info;
        double percent = 0.0;
        int nper = -1;
        int nret = statvfs("/home/www/webroot/fdrfile", &vfs_info);
        if(nret == 0)
        {
                percent = (double)(vfs_info.f_blocks-vfs_info.f_bfree)/(double)(vfs_info.f_blocks);
                nper = percent * 100;
                std::cout<<"total:"<<vfs_info.f_blocks<<"\tfree:"<<vfs_info.f_bfree<<"\tused:"<<nper<<std::endl;
        }
        else
        {
                std::cout<<"exe statvfs err,ret:"<<nret<<std::endl;
        }
		struct statfs fs_info;
		nret = statfs("/home/www/webroot/fdrfile", &fs_info);
		if(0 == nret)
		{		
			long free_size = (fs_info.f_bfree * fs_info.f_bsize)>>30;
			std::cout<<"free_size:"<<free_size<<std::endl;
		}
		else
		{
                std::cout<<"exe statfs err,ret:"<<nret<<std::endl;
        }
        return 0;
}
