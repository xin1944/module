#include<stdio.h>
#include<stdlib.h>
#include<string.h>
 
 
/*
* 字符串转成bcd码，这个是正好偶数个数据的时候，如果是奇数个数据则分左靠还是右靠压缩BCD码
*/
intasc_to_bcd(char * dest,const char *src)
{
       unsigned char temp;
       while(*src !='\0')
       {
              temp = *src;
              *dest |=((temp&0xf)<<4);
              src++;
              temp = *src;
              *dest |= (temp&0xf);
              src++;
              dest++;
       }
       return 0;
}
 
intasc_to_bcd_right(char *dest,const char *src,int src_len)
{
       unsigned char temp;
       if((src_len %2) !=0)
       {
              *dest &= 0;
              temp = *src;
              *dest |= (temp&0xf);
              src++;
              dest++;
       }
       asc_to_bcd(dest,src);
       return 0;
}
 
intasc_to_bcd_left(char *dest,const char *src,int src_len)
{
       unsigned char temp;
       if((src_len %2) !=0)
       {
              dest[src_len-1] &=0;
       }
       asc_to_bcd(dest,src);
       return 0;
}
 
voidprint_hex(char * bcd,int len)
{
       int i=0;
       for(i=0;i<len;i++)
       {
              int n = 8;
              while(n--){
                     if((bcd[i] &(0x1<<n))==0)
                            printf("0");
                     else
                            printf("1");
              }
              putchar('\n');
       }
}
 
 
intbcd_to_asc(char *dest,const char *src,int src_len)
{
       unsigned char temp;
       char *t = dest;
       while(src_len--)
       {
              *t |= ((*src&0xf0)>>4);
              *t++ |= 0x30;
              *t |= (*src&0xf);
              *t++ |= 0x30;
              src++;
       }
       return 0;
}
 
intleft_bcd_to_asc(char *dest,const char * src,int src_len)
{
       bcd_to_asc(dest,src,src_len);
       dest[src_len*2 -1] = '\0';
 
       return 0;
}
 
 
intright_bcd_to_asc(char *dest,const char * src,int src_len)
{
       bcd_to_asc(dest,src,src_len);
 
       memmove(dest,dest+1,src_len*2-1);
       dest[src_len*2-1] = '\0';
       return 0;
}
 
 
int main(void)
{
       char str[100];
      
       char *str1 = "1234567";
       int str_len = strlen(str1);
       int bcd_len = (str_len+1)/2;
       char *bcd = (char *)malloc(bcd_len);
       printf("str_len = %d\n",str_len);
       printf("bcd_len = %d\n",bcd_len);
       memset(bcd,0,bcd_len);
       memset(str,0,100);
 
#if 0
       printf("右靠\n");
       asc_to_bcd_right(bcd,str1,str_len);
       print_hex(bcd,bcd_len);
       right_bcd_to_asc(str,bcd,bcd_len);
       printf("str = %s\n",str);
#endif
 
#if 1
       printf("左靠\n");
       asc_to_bcd_left(bcd,str1,str_len);
       print_hex(bcd,bcd_len);
       //memset(str,0,100);
       left_bcd_to_asc(str,bcd,bcd_len);
       printf("str = %s\n",str);
#endif
       return 0;
}

