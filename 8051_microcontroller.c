#include<reg51.h>
#define port P1
#define key P0		  // Port for keypad
 void memory();

 sfr lcd_data_pin=0xA0;
 sbit rs = P1^3;
 sbit rw = P1^4;
 sbit en = P1^5;

 sbit col1=key^4;
 sbit col2=key^5;
 sbit col3=key^6;
 sbit row1=key^0;
 sbit row2=key^1;
 sbit row3=key^2;
 sbit row4=key^3;
 unsigned int number=0,pointer=1;
 unsigned char card_mem[4][12];
 unsigned char card_id[12],index=0,key1=0,lcd,print=0,new_user=0,recharge=0,user_index,t,tool=0;
 unsigned int amount[4];
 unsigned char current_byte = 0,money[6];
 unsigned char pos=0,set=0;

void delay(unsigned char count)	//Function to provide time delay
{
 	unsigned int j;
	unsigned char i;
 	for(i=0;i<count;i++)
 	for(j=0;j<1275;j++);
}

void lcd_command(unsigned char comm)   //LCD command funtion 
{
 	lcd_data_pin=comm;
 	en=1;
 	rs=0;
 	rw=0;
 	delay(1);
 	en=0;
}

void lcd_data(unsigned char disp)  //LCD data function 
{
 	lcd_data_pin=disp;
 	en=1;
 	rs=1;
 	rw=0;
 	delay(1);
 	en=0;
}

lcd_string(unsigned char *disp)	 //LCD string function 
{
 	char x;
 	for(x=0;disp[x]!=0;x++)
	{
 		lcd_data(disp[x]);
	}
}

void lcd_ini()				  //Function to initialize the LCD
{
	lcd_command(0x38);		   
	delay(5);
	lcd_command(0x0F);        
	delay(5);
	
}

lcd_display(unsigned int val)
{
	unsigned char flg=0;
	lcd_command(0xCC);
	if(val==0)
	lcd_data('0');
	while(val>0)
	{ 
		lcd=val%10;
		val=val/10;
		lcd_command(0xCC-flg);
		lcd_data(lcd+'0');
		flg++;
	}
}


void recieve() interrupt 4	 //Function to recieve data serialy from RS232 
{
	card_id[current_byte]=SBUF;
	RI=0;				// Reset the serial interrupt after recieving the byte
	current_byte++;	
}

void display(unsigned char value)
{
	lcd_command(0x80+pos);
	if(value==10)
	{
		print=1; 
	}
	if(value==12)
	{
		set=1; 
	}
	if(value<=9)
	{
		lcd_data('0'+value);
		money[pos]=value;
		pos++;
	}
}

check_col1()
{
	row1=row2=row3=row4=1;
	row1=0;
	if(col1==0)
	display(1);
	row1=1;
	row2=0;
	if(col1==0)
	display(4);
	row2=1;
	row3=0;
	if(col1==0)
	display(7);
	row3=1;
	row4=0;
	if(col1==0)
	display(10);
	row4=1;
}

check_col2()
{
	row1=row2=row3=row4=1;
	row1=0;
	if(col2==0)
	display(2);
	row1=1;
	row2=0;
	if(col2==0)
	display(5);
	row2=1;
	row3=0;
	if(col2==0)
	display(8);
	row3=1;
	row4=0;
	if(col2==0)
	display(0);
	row4=1;
}

check_col3()
{
	row1=row2=row3=row4=1;
	row1=0;
	if(col3==0)
	display(3);
	row1=1;
	row2=0;
	if(col3==0)
	display(6);
	row2=1;
	row3=0;
	if(col3==0)
	display(9);
	row3=1;
	row4=0;
	if(col3==0)
	display(12);
	row4=1;
}

keypad()
{
	row1=row2=row3=row4=0;
	if(col1==0)
	check_col1();
	if(col2==0)
	check_col2();
	if(col3==0)
	check_col3();
}
  
void show()		
{
	unsigned char count,i,key,val,flag=0,in;
	unsigned int balance,j;
	TMOD=0x00;							//Enable Timer 1
	TH1=0x00;
	SCON=0x00;
	TR1=0;
	IE=0x00; 
	lcd_command(0x01);		
	 val=index;
	for(i=0;i<index;i++)
	{ 
		key=0;
		for(count=0;count<12;count++)
		{ 
			if(card_id[count]==card_mem[i][count])
			{
				key++;
			}
		}
		if(key==12)
		{
			flag=1;
			if(amount[i]<=10)
			{
				user_index=i;
				lcd_string("Insuff. balance");
				lcd_command(0xC0);
				lcd_string("Pls recharge");
				delay(250);
				lcd_command(0x01);
				lcd_string("To recharge");
				lcd_command(0xC0);
				lcd_string("press *");
				for(in=0;in<250;in++)
				{
					for(j=0;j<1275;j++)
					{
						row1=row2=row3=row4=0;
						if(col1==0)
						{
							tool=1;
							recharge=1;
							t=1;
							keypad();
							break;
						}
					}
					if(t==1)
					break;
				}
				lcd_command(0x01);
				lcd_string("Pls scan your ID");
				break;  
			}
			if(tool==0)
			{
				lcd_string("Thank you User ");
				lcd_command(0x8F);
				lcd_data(49+i);
				delay(250);
				lcd_command(0x01);
				lcd_string("Charges 10.00");
				amount[i]=amount[i]-10;
				lcd_command(0xC0);
				lcd_string("Amt left  ");
				balance=amount[i];
				lcd_display(balance);
				lcd_command(0xCD);
				lcd_string(".00");
				delay(250);
				lcd_command(0x01);
				if(amount[i]<=30)
				{
					user_index=i;
					lcd_string("Pls recharge");
					lcd_command(0xC0);
					lcd_string("your account");
					delay(250);
					lcd_command(0x01);
					lcd_string("To recharge");
					lcd_command(0xC0);
					lcd_string("press *");
					for(in=0;in<250;in++)
					{
						for(j=0;j<1275;j++)
						{
							row1=row2=row3=row4=0;
							if(col1==0)
							{
								recharge=1;
								t=1;
								keypad();
								break;
							}
						}
						if(t==1)
						break;
					} 
				}
			}
		t=0;
		lcd_command(0x01);
		lcd_string("Pls scan your ID"); 
		break;
		}
	}
	if(flag==0)
	{
		lcd_string("Wrong ID");
		delay(100);
	}
	if(flag==0)
	{
		lcd_command(0x01);
		lcd_string("New user press *");
		new_user=1;
	}
	current_byte=0;
	tool=0;
	TMOD=0x20;							//Enable Timer 1
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	IE=0x94;
}

recharge_acc()
{
	unsigned char pin;
	lcd_command(0x01);
	lcd_string("Pls enter the");
	lcd_command(0xC0);
	lcd_string("Balance");
	delay(250);
	lcd_command(0x01);
	while(set!=1)
	{
		keypad();
		delay(40); 
	}
	set=0;
	lcd_command(0x01);
	lcd_string("Your balance is");
	for(pin=0;pin<=pos;pin++)
	{
		number=number+money[pos-pin-1]*pointer;
		pointer=pointer*10;
	}
	amount[user_index]=amount[user_index]+number;
	lcd_command(0xC0);
	lcd_display(amount[user_index]);
	lcd_command(0xCD);
	lcd_string(".00");
	delay(250);
	lcd_command(0x01);
	number=0;
	pointer=1;
	pos=0;
	lcd_string("Pls scan your ID");
}

void memory()
{
	unsigned char i,key=0,count,pin,try=0;
	lcd_command(0x01);
	lcd_string("New user scan ID");
	current_byte=0;
	while(current_byte!=12);
	for(i=0;i<4;i++)
	{ 
		key=0;
		for(count=0;count<12;count++)
		{ 
			if(card_id[count]==card_mem[i][count])
			{
				key++;
			}
		}
		if(key==12)
		{
			lcd_command(0x01);
			lcd_string("Sorry ! You are");
			lcd_command(0xC0);
			lcd_string("already an user"); 
			delay(200);
			lcd_command(0x01);
			lcd_string("Pls scan your ID");
			key=0;
			try=1;
			current_byte=0;
			break;
		}
	}
	if(key<12 && try==0)
	{
		key=0;	 
		for(i=0;i<12;i++)
		{
			card_mem[index][i]=card_id[i];
		}
		current_byte=0;
		lcd_command(0x01);
		lcd_string("Pls scan again");
		while(current_byte!=12);
		for(i=0;i<12;i++)
		{
			if(card_mem[index][i]==card_id[i])
			{
				key++;
			}
		}
		current_byte=0;
		TMOD=0x00;							
		TH1=0x00;
		SCON=0x00;
		TR1=0;
		IE=0x00; 
		if(key==12)
		{
			lcd_command(0x01);
			lcd_string("Congratulation !");
			lcd_command(0xC0);
			lcd_string("You are User");
			lcd_command(0xCC);
			lcd_data(index+49);
			delay(250);
			lcd_command(0x01);
			lcd_string("Pls enter the");
			lcd_command(0xC0);
			lcd_string("Balance");
			delay(150);
			lcd_command(0x01);
			while(set!=1)
			{
				keypad();
				delay(40); 
			}
			set=0;
			lcd_command(0x01);
			lcd_string("Your balance is");
			for(pin=0;pin<=pos;pin++)
			{
				number=number+money[pos-pin-1]*pointer;
				pointer=pointer*10;
			}
			lcd_command(0xC0);
			lcd_display(number);
			lcd_command(0xCD);
			lcd_string(".00");
			amount[index]=number;
			delay(250);
			lcd_command(0x01);
			number=0;
			pointer=1;
			pos=0;
			lcd_string("Pls scan your ID"); 
		}
		else	
		{
			lcd_command(0x01);
			lcd_string("ERROR");
			delay(200);
			for(i=0;i<12;i++)
			{
				card_mem[index][i]=0;
			}
			lcd_command(0x01);
			lcd_string("Pls scan your ID");
		}
		if(key==12)
		index++;
	}
	TMOD=0x20;							//Enable Timer 1
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	IE=0x94;
}

void main()
{
	col1=col2=col3=1;
	TMOD=0x20;							
	TH1=0xFD;
	SCON=0x50;
	TR1=1;
	IE=0x94;
	lcd_ini();					//Place cursor to second position of first line 
	lcd_string("Pls scan your ID");
	while(1)
	{
		row1=row2=row3=row4=0;
		if(col1==0)
		{
			keypad();
		}
		if(print==1 && new_user==1)
		{
			memory();
			print=0;
			new_user=0;
		}
		if(print==1 && recharge==1)
		{
			recharge_acc();
			print=0;
			recharge=0;
		}
		if(current_byte==12)
		{
			show();
		}
	}
}
