#ifndef COMN_FUNC_H
#define COMN_FUNC_H

//// Conversion Functions //
// switch a string to integer
inline int ParseInt(char* str)
{
	int i = 0;
	int e = strlen(str);
	int num = 0;;
	char c;
	if (str[0] == '-')
	{
		for (i = 0; i < e; i++)
		{
			c = str[i];
			if (c >= '0' && c <= '9')
			{
				num = num*10 - (c-'0');
			}
		}
	}
	else
	{
		for (i = 0; i < e; i++)
		{
			c = str[i];
			if (c >= '0' && c <= '9')
			{
				num = num*10 + (c-'0');
			}
		}
	}
	return num;
}

// switch a string to double
inline double ParseDouble(char* str)
{
	int phase = 1; // 1 is positive, -1 is negative
	int e = strlen(str);
	bool floatStart = false;
	int intPart = 0;
	double floatPart = 0.0;
	int floatBit = 1;
	double dNum = 0.0;

	char c;

	if (str[0] == '-')
	{
		phase = -1;
	}
	else
	{
		phase = 1;
	}

	for (int i = 0; i < e; i++)
	{
		c = str[i];
		if (!floatStart)
		{
			if (c >= '0' && c <= '9')
				intPart = intPart*10 + (c-'0');
			else if (c == '.')
				floatStart = true;
		}
		else
		{
			if (c >= '0' && c <= '9')
			{
				floatBit *= 10;
				floatPart = floatPart*10 + (c-'0');
			}
		}
	}

	dNum = phase * (intPart + floatPart / floatBit);

	return dNum;
}

// switch integer to a string
inline void ParseStr(char* str, int num)
{
	int i = 0, len = 0;
	char c;
	bool isnegative = false;
	if (num < 0)
		isnegative = true;
	if (num == 0)
	{
		str[0] = '0';
		i = 1;
	}
	while(num != 0)
	{
		str[i] = '0'+num%10;
		num = num/10;
		i++;
	}
	if (isnegative)
	{
		str[i] = '-';
		i++;
	}
	str[i] = '\0';

	len = strlen(str);
	for (i = 0; i < (len>>1); i++)
	{
		c = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = c;
	}
	str[len] = '\0';
}

// switch integer to a string
inline char* ParseStr(int num)
{
	char* str = new char[32];

	int i = 0, len = 0;
	char c;
	bool isnegative = false;
	if (num < 0)
		isnegative = true;
	if (num == 0)
	{
		str[0] = '0';
		i = 1;
	}
	while(num != 0)
	{
		str[i] = '0'+num%10;
		num = num/10;
		i++;
	}
	if (isnegative)
	{
		str[i] = '-';
		i++;
	}
	str[i] = '\0';

	len = strlen(str);
	for (i = 0; i < (len>>1); i++)
	{
		c = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = c;
	}
	str[len] = '\0';

	return str;
}

// overload switch double to a string
inline char* ParseStr(double dNum, int precision)
{
	int intPart = (int)dNum;
	double floatPart = dNum - intPart;
	unsigned int scale = 1;
	int zeropack = 0; // pack the 0 for those number like 0.00001
	for (int i = 0; i < precision; i++)
	{
		floatPart *= 10;
		if (floatPart < 1)
			zeropack++;
	}

	floatPart *= scale;

	char* str = new char[32];

	str = ParseStr(intPart);
	strcat_s(str, 32, ".");
	for (int i = 0; i < zeropack; i++)
		strcat_s(str, 32, "0");
	strcat_s(str, 32, ParseStr((int)floatPart));

	return str;
}

//// Memory Operations //

inline void myMemset(void *dest, int count, unsigned int data)
{
	// this function fills or sets unsigned 32-bit aligned memory
	// count is number of quads

	_asm
	{
		mov edi, dest;	// edi(变址寄存器) points to destination memory
		mov ecx, count;	// number of 32-bit words to move (ecx通常用作循环计数器，rep指令使用exc循环递减)
		mov eax, data;	// 32-bit data
		rep stosd;		// move data (rep指令是重复执行stosd指令，stosd指令是将EAX中的内容存储到由ES:EDI寻址的内存地址.)
	} // end asm
} // end Mem_Set_QUAD

#endif