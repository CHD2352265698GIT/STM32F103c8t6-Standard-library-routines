#include "ili9488_tft.h"
#include "stdlib.h"
#include "tft_font.h" 
#include "delay.h"	   
#include "font.h"	   

//	LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR	=	0x0000;		//	������ɫ
u16 BACK_COLOR	=	0xFFFF;  	//	����ɫ 
  
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//*	���ƣ�void LCD_WR_REG(u16 regval)
//*	���ܣ�д�Ĵ���
//*	���룺regval:�Ĵ���ֵ
//*	���أ���
void LCD_WR_REG(u16 regval)
{   
	LCD_RS_CLR; 
 	LCD_CS_CLR; 
	DATAOUT(regval); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;	
}

//*	���ƣ�void LCD_WR_DATA(u16 data)
//*	���ܣ�дLCD����
//*	���룺data:Ҫд���ֵ
//*	���أ���
void LCD_WR_DATA(u16 data)
{	 
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;	
}

//*	���ƣ�u16 LCD_RD_DATA(void)
//*	���ܣ���LCD����
//*	���룺��
//*	���أ�����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{
	u16 t;
 	GPIOB->CRL=0X88888888; //PB0-7  ��������
	GPIOB->CRH=0X88888888; //PB8-15 ��������
	GPIOB->ODR=0X0000;     //ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	LCD_RD_CLR;
					   
	t=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333; //PB0-7  �������
	GPIOB->CRH=0X33333333; //PB8-15 �������
	GPIOB->ODR=0XFFFF;    //ȫ�������
	return t;  
}					   

//*	���ƣ�void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
//*	���ܣ�д�Ĵ���
//*	���룺LCD_Reg:�Ĵ�����ַ��LCD_RegValue:Ҫд�������
//*	���أ���
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    
}	   

//*	���ƣ�u16 LCD_ReadReg(u16 LCD_Reg)
//*	���ܣ����Ĵ���
//*	���룺LCD_Reg:�Ĵ�����ַ
//*	���أ�����������
u16 LCD_ReadReg(u16 LCD_Reg)
{										   	
	LCD_WR_REG(LCD_Reg);  //д��Ҫ���ļĴ�����  
	Delay_us(15);
	return LCD_RD_DATA(); 
}   

//*	���ƣ�void LCD_WriteRAM_Prepare(void)
//*	���ܣ���ʼдGRAM
//*	���룺��
//*	���أ���
void LCD_WriteRAM_Prepare(void)
{	  
	LCD_WR_REG(lcddev.wramcmd);
}	 

//*	���ƣ�void LCD_WriteRAM(u16 RGB_Code)
//*	���ܣ�LCDдGRAM
//*	���룺RGB_Code:��ɫֵ
//*	���أ���
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code);//дʮ��λGRAM
}

//*	���ƣ�u16 LCD_BGR2RGB(u16 c)
//*	���ܣ���ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//*	���룺c:GBR��ʽ����ɫֵ
//*	���أ�RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 

//*	���ƣ�void opt_delay(u8 i)
//*	���ܣ���mdk -O1ʱ���Ż�ʱ��Ҫ����
//*	���룺��ʱi
//*	���أ���
void opt_delay(u8 i)
{
	while(i--);
}

//*	���ƣ�u16 LCD_ReadPoint(u16 x,u16 y)
//*	���ܣ���ȡ��ĳ�����ɫֵ	 
//*	���룺x,y:����
//*	���أ��˵����ɫ
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u16 color;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//�����˷�Χ,ֱ�ӷ���		   	  
	LCD_WR_REG(0X2E);//9486 ���Ͷ�GRAMָ��
	color=LCD_RD_DATA();								//������ɫֵ	 
	opt_delay(2);	  
	color=LCD_RD_DATA();  		  						//ʵ��������ɫ
	opt_delay(2);	   
	return 	color;		
}		 

//*	���ƣ�void LCD_DisplayOn(void)
//*	���ܣ�LCD������ʾ 
//*	���룺��
//*	���أ���
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29);	//������ʾ
}	 

//*	���ƣ�void LCD_DisplayOff(void)
//*	���ܣ�LCD�ر���ʾ 
//*	���룺��
//*	���أ���
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);	//�ر���ʾ
}   

//*	���ƣ�void LCD_Scan_Dir(u8 dir)
//*	���ܣ�����LCD���Զ�ɨ�跽��
//*	���룺��
//*	���أ���
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//����ʱ����6804��1963���ı�ɨ�跽������ʱ1963�ı䷽��
	{			   
		switch(dir)//����ת��
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
	
	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36;
	regval|=0X08; 
	LCD_WriteReg(dirreg,regval);
	
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
}     

//*	���ƣ�void LCD_Display_Dir(u8 dir)
//*	���ܣ�����LCD��ʾ����
//*	���룺dir:0,������1,����
//*	���أ���
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			
	{
		lcddev.dir=0;
	
		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 
		
		lcddev.width=320;
		lcddev.height=480;
	}
	else
	{	  				
		lcddev.dir=1;
		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 
		lcddev.width=480;
		lcddev.height=320; 			
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}	 

//*	���ƣ�void LCD_Init(void)
//*	���ܣ���ʼ��lcd
//*	���룺��
//*	���أ���
void LCD_Init(void)
{ 		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����SWD��ʧ��JTAG

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 

	GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);
	GPIO_ResetBits(GPIOC,GPIO_Pin_10);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_All);

	LCD_WR_REG(0XD3);				   
	lcddev.id=LCD_RD_DATA();
	lcddev.id=LCD_RD_DATA();	
	lcddev.id=LCD_RD_DATA();   							   
	lcddev.id<<=8;
	lcddev.id|=LCD_RD_DATA();  	

	LCD_WR_REG( 0XF7 );
	LCD_WR_DATA( 0xA9 );
	LCD_WR_DATA( 0x51 );
	LCD_WR_DATA( 0x2C );
	LCD_WR_DATA( 0x82 ); 

	/* Power Control 1 (C0h)  */
	LCD_WR_REG( 0xC0 );
	LCD_WR_DATA( 0x11 );
	LCD_WR_DATA( 0x09 );

	/* Power Control 2 (C1h) */
	LCD_WR_REG( 0xC1 );
	LCD_WR_DATA( 0x41 );

	/* VCOM Control (C5h)  */
	LCD_WR_REG( 0XC5 );
	LCD_WR_DATA( 0x00 );
	LCD_WR_DATA( 0x0A );
	LCD_WR_DATA( 0x80 );

	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	LCD_WR_REG( 0xB1 );
	LCD_WR_DATA( 0xB0 );
	LCD_WR_DATA( 0x11 );

	/* Display Inversion Control (B4h) */
	LCD_WR_REG( 0xB4 );
	LCD_WR_DATA( 0x02 );

	/* Display Function Control (B6h)  */
	LCD_WR_REG( 0xB6 );
	LCD_WR_DATA( 0x02 );
	LCD_WR_DATA( 0x22 );

	/* Entry Mode Set (B7h)  */
	LCD_WR_REG( 0xB7 );
	LCD_WR_DATA( 0xc6 );

	/* HS Lanes Control (BEh) */
	LCD_WR_REG( 0xBE );
	LCD_WR_DATA( 0x00 );
	LCD_WR_DATA( 0x04 );

	/* Set Image Function (E9h)  */
	LCD_WR_REG( 0xE9 );
	LCD_WR_DATA( 0x00 );

	LCD_WR_REG( 0x36 );
	LCD_WR_DATA( 0x08 );

	/* Interface Pixel Format (3Ah) */
	LCD_WR_REG( 0x3A );
	LCD_WR_DATA( 0x55 ); /* 0x55 : 16 bits/pixel  */

	/* PGAMCTRL (Positive Gamma Control) (E0h) */
	LCD_WR_REG( 0xE0 );
	LCD_WR_DATA( 0x00 );
	LCD_WR_DATA( 0x07 );
	LCD_WR_DATA( 0x10 );
	LCD_WR_DATA( 0x09 );
	LCD_WR_DATA( 0x17 );
	LCD_WR_DATA( 0x0B );
	LCD_WR_DATA( 0x41 );
	LCD_WR_DATA( 0x89 );
	LCD_WR_DATA( 0x4B );
	LCD_WR_DATA( 0x0A );
	LCD_WR_DATA( 0x0C );
	LCD_WR_DATA( 0x0E );
	LCD_WR_DATA( 0x18 );
	LCD_WR_DATA( 0x1B );
	LCD_WR_DATA( 0x0F );

	/* NGAMCTRL (Negative Gamma Control) (E1h)  */
	LCD_WR_REG( 0XE1 );
	LCD_WR_DATA( 0x00 );
	LCD_WR_DATA( 0x17 );
	LCD_WR_DATA( 0x1A );
	LCD_WR_DATA( 0x04 );
	LCD_WR_DATA( 0x0E );
	LCD_WR_DATA( 0x06 );
	LCD_WR_DATA( 0x2F );
	LCD_WR_DATA( 0x45 );
	LCD_WR_DATA( 0x43 );
	LCD_WR_DATA( 0x02 );
	LCD_WR_DATA( 0x0A );
	LCD_WR_DATA( 0x09 );
	LCD_WR_DATA( 0x32 );
	LCD_WR_DATA( 0x36 );
	LCD_WR_DATA( 0x0F );

	/* Sleep Out11h */
	LCD_WR_REG( 0x11 );
	Delay_ms( 120 );
	LCD_WR_REG( 0x29 );

	LCD_Display_Dir(0);
	LCD_LED=1;		
	LCD_Clear(WHITE);
} 

//*	���ƣ�void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
//*	���ܣ����ٻ���
//*	���룺x,y:����,color:��ɫ
//*	���أ���
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y,lcddev.width,lcddev.height);
    LCD_WR_REG(lcddev.wramcmd);
	LCD_WR_DATA(color);	  
}

//*	���ƣ�void LCD_Clear(u16 color)
//*	���ܣ���������
//*	���룺color:��ɫ
//*	���أ���
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//�õ��ܵ�	

	LCD_SetCursor(0,0,lcddev.width,lcddev.height);
	
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD_WR_DATA(color);
	}
	
} 

//*	���ƣ�void LCD_SetCursor(uint16_t statr_x,uint16_t start_y,uint16_t end_x,uint16_t end_y)
//*	���ܣ����ù��λ��
//*	���룺����
//*	���أ���
void LCD_SetCursor(uint16_t statr_x,uint16_t start_y,uint16_t end_x,uint16_t end_y)
{	 
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(statr_x>>8);
	LCD_WR_DATA(statr_x&0XFF); 		
	LCD_WR_DATA(end_x>>8);
	LCD_WR_DATA(end_x&0XFF); 				 
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(start_y>>8);
	LCD_WR_DATA(start_y&0XFF);
	LCD_WR_DATA(end_y>>8);
	LCD_WR_DATA(end_y&0XFF);
} 

//*	���ƣ�void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
//*	���ܣ���ָ����������䵥����ɫ
//*	���룺sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)  color:Ҫ������ɫ 
//*	���أ���
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	
	LCD_SetCursor(sx,sy,ex,ey);
	
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	
	for(j=sy;j<ey-sy+1;j++)
	{
		for(i=sx;i<ex-sx+1;i++)
		{
			LCD_WR_DATA(color);			 
		}
	}	 
}  

//*	���ƣ�void LCD_DrawPoint(u16 x,u16 y)
//*	���ܣ�����
//*	���룺x,y:����
//*	���أ���
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y,lcddev.width,lcddev.height);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();								//��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR); 
} 

//*	���ƣ�void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
//*	���ܣ�����
//*	���룺x1,y1:������꣬x2,y2:�յ�����  
//*	���أ���
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    

//*	���ƣ�void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
//*	���ܣ�������	
//*	���룺(x1,y1),(x2,y2):���εĶԽ�����
//*	���أ���
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

//*	���ƣ�void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
//*	���ܣ���ָ��λ�û�һ��ָ����С��Բ
//*	���룺(x,y):���ĵ㣬r:�뾶
//*	���أ���
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  

//*	���ƣ�void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
//*	���ܣ���ָ��λ����ʾһ���ַ�
//*	���룺x,y:��ʼ���꣬num:Ҫ��ʾ���ַ�:" "--->"~"��size:�����С 12/16/24��mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
//*	���أ���
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)
			temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)
			temp=asc2_1608[num][t];	//����1608����
		else if(size==24)
			temp=asc2_1608[num][t];	//����2412����
		else 
			return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)
				LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)
				return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)
					return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}   

//*	���ƣ�u32 LCD_Pow(u8 m,u8 n)
//*	���ܣ�m^n����
//*	���룺m^n�η�
//*	���أ���
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 

//*	���ƣ�void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
//*	���ܣ���ʾ����,��λΪ0,����ʾ
//*	���룺x,y :������꣬len :���ֵ�λ����size:�����С��color:��ɫ ��num:��ֵ(0~4294967295);
//*	���أ���
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}
			else 
				enshow=1; 
		}
		LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//* ��ʾ����,��λΪ0,������ʾ
//*	x,y:�������
//*	num:��ֵ(0~999999999);	 
//* len:����(��Ҫ��ʾ��λ��)
//* size:�����С
//* mode:
//*	[7]:0,�����;1,���0.
//* [6:1]:����
//* [0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//* ��ʾ�ַ���
//* x,y:�������
//* width,height:�����С  
//* size:�����С
//* *p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//*	���ƣ�void LCD_test(void)
//*	���ܣ�LCD���ܺ�������
//*	���룺��
//*	���أ���
void LCD_test(void)
{
	uint16_t i ,j;		
	LCD_Fill(0,0,150,150,YELLOW);
	for( j=150;j<300;j++)
	{
		for( i=150;i<300;i++)
		{
		  LCD_Fast_DrawPoint(j,i,CYAN);			 
		}
	}
	LCD_ShowNum(10,150,88888,5,24);
	LCD_ShowChar(10,170,'8',24,0);
	LCD_ShowxNum(10,210,112233,6,24,0);
	LCD_ShowString(10,230,60,10,24,(uint8_t*)"AABBCC");
	LCD_DrawLine(0,0, 300, 300);
	LCD_Draw_Circle(150,150,100);
}

