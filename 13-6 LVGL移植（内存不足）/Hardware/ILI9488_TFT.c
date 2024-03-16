#include "ili9488_tft.h"
#include "stdlib.h"
#include "tft_font.h" 
#include "delay.h"	   
#include "font.h"	   

//	LCD的画笔颜色和背景色	   
u16 POINT_COLOR	=	0x0000;		//	画笔颜色
u16 BACK_COLOR	=	0xFFFF;  	//	背景色 
  
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//*	名称：void LCD_WR_REG(u16 regval)
//*	功能：写寄存器
//*	输入：regval:寄存器值
//*	返回：无
void LCD_WR_REG(u16 regval)
{   
	LCD_RS_CLR; 
 	LCD_CS_CLR; 
	DATAOUT(regval); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;	
}

//*	名称：void LCD_WR_DATA(u16 data)
//*	功能：写LCD数据
//*	输入：data:要写入的值
//*	返回：无
void LCD_WR_DATA(u16 data)
{	 
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;	
}

//*	名称：u16 LCD_RD_DATA(void)
//*	功能：读LCD数据
//*	输入：无
//*	返回：返回值:读到的值
u16 LCD_RD_DATA(void)
{
	u16 t;
 	GPIOB->CRL=0X88888888; //PB0-7  上拉输入
	GPIOB->CRH=0X88888888; //PB8-15 上拉输入
	GPIOB->ODR=0X0000;     //全部输出0

	LCD_RS_SET;
	LCD_CS_CLR;
	LCD_RD_CLR;
					   
	t=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333; //PB0-7  上拉输出
	GPIOB->CRH=0X33333333; //PB8-15 上拉输出
	GPIOB->ODR=0XFFFF;    //全部输出高
	return t;  
}					   

//*	名称：void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
//*	功能：写寄存器
//*	输入：LCD_Reg:寄存器地址，LCD_RegValue:要写入的数据
//*	返回：无
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    
}	   

//*	名称：u16 LCD_ReadReg(u16 LCD_Reg)
//*	功能：读寄存器
//*	输入：LCD_Reg:寄存器地址
//*	返回：读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{										   	
	LCD_WR_REG(LCD_Reg);  //写入要读的寄存器号  
	Delay_us(15);
	return LCD_RD_DATA(); 
}   

//*	名称：void LCD_WriteRAM_Prepare(void)
//*	功能：开始写GRAM
//*	输入：无
//*	返回：无
void LCD_WriteRAM_Prepare(void)
{	  
	LCD_WR_REG(lcddev.wramcmd);
}	 

//*	名称：void LCD_WriteRAM(u16 RGB_Code)
//*	功能：LCD写GRAM
//*	输入：RGB_Code:颜色值
//*	返回：无
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code);//写十六位GRAM
}

//*	名称：u16 LCD_BGR2RGB(u16 c)
//*	功能：从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//*	输入：c:GBR格式的颜色值
//*	返回：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 

//*	名称：void opt_delay(u8 i)
//*	功能：当mdk -O1时间优化时需要设置
//*	输入：延时i
//*	返回：无
void opt_delay(u8 i)
{
	while(i--);
}

//*	名称：u16 LCD_ReadPoint(u16 x,u16 y)
//*	功能：读取个某点的颜色值	 
//*	输入：x,y:坐标
//*	返回：此点的颜色
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u16 color;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   	  
	LCD_WR_REG(0X2E);//9486 发送读GRAM指令
	color=LCD_RD_DATA();								//读出颜色值	 
	opt_delay(2);	  
	color=LCD_RD_DATA();  		  						//实际坐标颜色
	opt_delay(2);	   
	return 	color;		
}		 

//*	名称：void LCD_DisplayOn(void)
//*	功能：LCD开启显示 
//*	输入：无
//*	返回：无
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29);	//开启显示
}	 

//*	名称：void LCD_DisplayOff(void)
//*	功能：LCD关闭显示 
//*	输入：无
//*	返回：无
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);	//关闭显示
}   

//*	名称：void LCD_Scan_Dir(u8 dir)
//*	功能：设置LCD的自动扫描方向
//*	输入：无
//*	返回：无
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
	{			   
		switch(dir)//方向转换
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
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
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

//*	名称：void LCD_Display_Dir(u8 dir)
//*	功能：设置LCD显示方向
//*	输入：dir:0,竖屏；1,横屏
//*	返回：无
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
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 

//*	名称：void LCD_Init(void)
//*	功能：初始化lcd
//*	输入：无
//*	返回：无
void LCD_Init(void)
{ 		
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//开启SWD，失能JTAG

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

//*	名称：void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
//*	功能：快速画点
//*	输入：x,y:坐标,color:颜色
//*	返回：无
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y,lcddev.width,lcddev.height);
    LCD_WR_REG(lcddev.wramcmd);
	LCD_WR_DATA(color);	  
}

//*	名称：void LCD_Clear(u16 color)
//*	功能：清屏函数
//*	输入：color:颜色
//*	返回：无
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点	

	LCD_SetCursor(0,0,lcddev.width,lcddev.height);
	
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD_WR_DATA(color);
	}
	
} 

//*	名称：void LCD_SetCursor(uint16_t statr_x,uint16_t start_y,uint16_t end_x,uint16_t end_y)
//*	功能：设置光标位置
//*	输入：坐标
//*	返回：无
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

//*	名称：void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
//*	功能：在指定区域内填充单个颜色
//*	输入：sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)  color:要填充的颜色 
//*	返回：无
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	
	LCD_SetCursor(sx,sy,ex,ey);
	
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	
	for(j=sy;j<ey-sy+1;j++)
	{
		for(i=sx;i<ex-sx+1;i++)
		{
			LCD_WR_DATA(color);			 
		}
	}	 
}  

//*	名称：void LCD_DrawPoint(u16 x,u16 y)
//*	功能：画点
//*	输入：x,y:坐标
//*	返回：无
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y,lcddev.width,lcddev.height);		//设置光标位置 
	LCD_WriteRAM_Prepare();								//开始写入GRAM
	LCD_WR_DATA(POINT_COLOR); 
} 

//*	名称：void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
//*	功能：画线
//*	输入：x1,y1:起点坐标，x2,y2:终点坐标  
//*	返回：无
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
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

//*	名称：void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
//*	功能：画矩形	
//*	输入：(x1,y1),(x2,y2):矩形的对角坐标
//*	返回：无
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

//*	名称：void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
//*	功能：在指定位置画一个指定大小的圆
//*	输入：(x,y):中心点，r:半径
//*	返回：无
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
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
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  

//*	名称：void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
//*	功能：在指定位置显示一个字符
//*	输入：x,y:起始坐标，num:要显示的字符:" "--->"~"，size:字体大小 12/16/24，mode:叠加方式(1)还是非叠加方式(0)
//*	返回：无
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)
			temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)
			temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)
			temp=asc2_1608[num][t];	//调用2412字体
		else 
			return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)
				LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)
				return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)
					return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   

//*	名称：u32 LCD_Pow(u8 m,u8 n)
//*	功能：m^n函数
//*	输入：m^n次方
//*	返回：无
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 

//*	名称：void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
//*	功能：显示数字,高位为0,则不显示
//*	输入：x,y :起点坐标，len :数字的位数，size:字体大小，color:颜色 ，num:数值(0~4294967295);
//*	返回：无
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
//* 显示数字,高位为0,还是显示
//*	x,y:起点坐标
//*	num:数值(0~999999999);	 
//* len:长度(即要显示的位数)
//* size:字体大小
//* mode:
//*	[7]:0,不填充;1,填充0.
//* [6:1]:保留
//* [0]:0,非叠加显示;1,叠加显示.
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
//* 显示字符串
//* x,y:起点坐标
//* width,height:区域大小  
//* size:字体大小
//* *p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//*	名称：void LCD_test(void)
//*	功能：LCD功能函数测试
//*	输入：无
//*	返回：无
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

