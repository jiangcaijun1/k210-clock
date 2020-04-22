#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "w25qxx.h"
#include "lcd.h"
#include "gbk.h"
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size==16)
	{
			qh=*code;
		ql=*(++code);
		if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
		{   		    
	 	   for(i=0;i<csize;i++)*mat++=0x00;//�������
	   		 return; //��������
		}          
		if(ql<0x7f)ql-=0x40;//ע��!
			else ql-=0x41;
			qh-=0x81;   
	
	foffset=((unsigned long)190*qh+ql)*csize;	//�õ��ֿ��е��ֽ�ƫ����  	

			w25qxx_read_data(GBK+foffset, mat, csize, W25QXX_QUAD_FAST);	
  
	}
	else
	{
		qh=*code;
		ql=*(++code);
		ql -= 0xa1;
		qh -= 0xa1;
        foffset = ((unsigned long)94*qh + ql)*csize;
	//w25qxx_read_data(GB32+foffset, mat, csize, W25QXX_QUAD_FAST);	 
	w25qxx_read_data(GB32+foffset, mat, csize, W25QXX_QUAD_FAST);
	
	}
	
   												    
}  
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0����,1,����	   gb32ֻ֧�ֿ���
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[128];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 

	if(size!=16&&size!=24)return;	//��֧�ֵ�size
	
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	if(mode)
	{
		if(size==16)
		lcd_draw_ico(x,y, 16, 16,dzk, BLACK);
		else
		{
		
			lcd_draw_ico(x,y, 24, 24,dzk, BLACK);
		}
		
	}
	else
	{
			uint8_t i = 0;
		    uint8_t j = 0;
		    uint8_t z = 0;
		    uint8_t data = 0;
		    for (z = 0; z < 16; z++){
		         for (i = 0; i < 2; i++)
		    {
		        data = dzk[z*2+i];
		        for (j = 0; j < 8; j++)
		        {
		            if (data & 0x80)
		                lcd_draw_point(x + i*8+j, y+z, BLACK);
		            data <<= 1;
		        }

		    }
		    }

	// 	for(t=0;t<csize;t++)
	// {   												   
	// 	temp=dzk[t];			//�õ���������                          
	// 	for(t1=0;t1<8;t1++)
	// 	{
	// 		if(temp&0x80)lcd_draw_point(x, y,BLACK);
	// 		else 
	// 		temp<<=1;
	// 		y++;
	// 		if((y-y0)==size)
	// 		{
	// 			y=y0;
	// 			x++;
	// 			break;
	// 		}
	// 	}  	 
	// }  
	}
	
}
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//width,height:����
//str  :�ַ���
//size :�����С
//mode:0,����;1,����   	   		   
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //�ַ��������� 1���� 0Ӣ�� 	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)//Ӣ��
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(x0+width-size/2))//����
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else 
		   
		        	
		      lcd_draw_char(x, y, *str, BLACK) ;
		      	
		        
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(x0+width-size))//����
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//Խ�緵��  				     
	        Show_Font(x,y,str,size,mode); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  			 		 
//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��			  
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,320,240,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,320,240,str,size,1);
	}
}   
//ָ����ɫ
void Show_Str_Mid_CL(u16 x,u16 y,u8*str,u8 size,u8 len,uint16_t color)
{
	u16 strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,320,240,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str_CL(strlenth+x,y,320,240,str,size,color);
	}
}   
void Show_Str_CL(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,uint16_t color)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //�ַ��������� 1���� 0Ӣ�� 	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)//Ӣ��
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(x0+width-size/2))//����
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else 
		   
		        	
		      lcd_draw_char(x, y, *str, BLACK) ;
		      	
		        
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(x0+width-size))//����
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//Խ�緵��  				     
	        Show_Font_CL(x,y,str,size,color); //��ʾ�������,������ʾ 
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}  		
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
void Show_Font_CL(u16 x,u16 y,u8 *font,u8 size,uint16_t color)
{
	
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[128];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 

	if(size!=16&&size!=24)return;	//��֧�ֵ�size
	
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 

	
		if(size==16)
		lcd_draw_ico(x,y, 16, 16,dzk, color);
		else
		{
		
			lcd_draw_ico(x,y, 24, 24,dzk, color);
		}
		
	

	
}























		  






