#include "stdafx.h"
#include "toolkit.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>

#if defined _WIN32 || WIN32 || _WINDOWS
    #include <direct.h>
    #define MKDIR(X) _mkdir((X));
#else
    #include <sys/stat.h>
    #define MKDIR(X) mkdir((X),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #define _vsnprintf vsnprintf
#endif // defined

void WriteLog(const char *fmt, ...)
{
	char buff[1024];
	va_list vl;
	va_start(vl, fmt);
	_vsnprintf(buff, 1024, fmt, vl);
	//fflush(stdout);
	va_end(vl);
	//char temp[2048] = "XSpeed:";
	//OutputDebugStringA(strcat(temp, buff));
#if defined _WIN32 || WIN32 || _WINDOWS
	OutputDebugStringA(buff);
#endif
}

void makedirs(const char* dir)
{
	if (nullptr == dir)
		return;

	size_t len = strlen(dir);

	char * p = new char[len+1];
	strcpy(p,dir);
	for (size_t i = 0; i<len; ++i)
	{
		char ch = p[i];
		if ('\\' == ch ||'/' == ch)
		{
			p[i] = '\0';
			MKDIR(p);
			p[i] = ch;
		}
	}
	delete[] p;
}

double my_round(float val, int x)
{
	double i = ((long)(val * 10000.0 + 0.5)) / 10000.0;
	return i;
}

void GetUpdateTime_HH_mm_ss(char* UpdateTime,int* _HH,int* _mm,int* _ss)
{
	*_HH = atoi(&UpdateTime[0]);
	*_mm = atoi(&UpdateTime[3]);
	*_ss = atoi(&UpdateTime[6]);
}

void GetUpdateTime_HHmmss(char* UpdateTime, int* _HH, int* _mm, int* _ss)
{
	int HHmmss = atoi(UpdateTime);
	*_HH = HHmmss / 10000;
	*_mm = HHmmss % 10000 / 100;
	*_ss = HHmmss % 100;
}

int GetDate(char* TradingDay)
{
	int yyyyMMdd = atoi(TradingDay);
	return yyyyMMdd;
}

int GetTime(char* UpdateTime)
{
	int HH = 0;// atoi(&UpdateTime[0]);
	int mm = 0;//atoi(&UpdateTime[3]);
	int ss = 0;//atoi(&UpdateTime[6]);
	if (strlen(UpdateTime) <= 6)
	{
		GetUpdateTime_HHmmss(UpdateTime, &HH, &mm, &ss);
	}
	else
	{
		GetUpdateTime_HH_mm_ss(UpdateTime, &HH, &mm, &ss);
	}
	return HH * 10000 + mm * 100 + ss;
}

int GetUpdateTime(char* UpdateTime, int* _UpdateTime, int* UpdateMillisec)
{
	// UpdateTime处理
	int HH = 0;// atoi(&UpdateTime[0]);
	int mm = 0;//atoi(&UpdateTime[3]);
	int ss = 0;//atoi(&UpdateTime[6]);
	if (strlen(UpdateTime) <= 6)
	{
		GetUpdateTime_HHmmss(UpdateTime, &HH, &mm, &ss);
	}
	else
	{
		GetUpdateTime_HH_mm_ss(UpdateTime, &HH, &mm, &ss);
	}

	if (UpdateMillisec)
	{
		if (strlen(UpdateTime)>9)
		{
			*UpdateMillisec = atoi(&UpdateTime[9]);
		}
		else
		{
			*UpdateMillisec = 0;
		}
	}

	if (_UpdateTime)
	{
		*_UpdateTime = HH * 10000 + mm * 100 + ss;
		if (*_UpdateTime == 0)
		{
			time_t now = time(0);
			struct tm *ptmNow = localtime(&now);
			int datetime = ptmNow->tm_hour * 10000 + ptmNow->tm_min * 100 + ptmNow->tm_sec;
			if (datetime > 1500 && datetime < 234500)
				*_UpdateTime = datetime;
		}
	}

	return HH;
}

void GetExchangeTime_DCE(char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec)
{
//DCE
//TradingDay : 交易日
//ActionDay : 交易日
	// UpdateTime处理
	int HH = GetUpdateTime(UpdateTime, _UpdateTime, UpdateMillisec);

	// TradingDay处理
	if (_TradingDay)
	{
		int tradingday = atoi(TradingDay);
		if (tradingday == 0)
		{
			time_t now = time(0);
			struct tm *ptmNow = localtime(&now);
			tradingday = (ptmNow->tm_year + 1900) * 10000 + (ptmNow->tm_mon + 1) * 100 + ptmNow->tm_mday;
		}
		*_TradingDay = tradingday;
	}

	if (_ActionDay == nullptr)
		return;

	// ActionDay处理
	if ((HH>6 && HH<18) && (ActionDay != nullptr) && (strlen(ActionDay) == 8))
	{
		*_ActionDay = atoi(ActionDay);
	}
	else
	{
		time_t now = time(0);
		struct tm *ptmNow = localtime(&now);
		if (HH >= 23)
		{
			if (ptmNow->tm_hour<1)
			{
				now -= 86400;
				ptmNow = localtime(&now);
			}
		}
		else if (HH<1)
		{
			if (ptmNow->tm_hour >= 23)
			{
				now += 86400;
				ptmNow = localtime(&now);
			}
		}

		*_ActionDay = (1900 + ptmNow->tm_year) * 10000 + (1 + ptmNow->tm_mon) * 100 + ptmNow->tm_mday;
	}
}

void GetExchangeTime_CZCE(int iTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec)
{
//CZC
//TradingDay : 行情日
//ActionDay : 行情日

	// UpdateTime处理
	int HH = GetUpdateTime(UpdateTime, _UpdateTime, UpdateMillisec);

	// TradingDay处理
	if (_TradingDay)
	{
		int tradingday = 0;
		if ((HH > 18 || HH < 6))
		{
			tradingday = iTradingDay;
		}
		else
		{
			tradingday = atoi(TradingDay);
		}
		if (tradingday == 0)
		{
			time_t now = time(0);
			struct tm *ptmNow = localtime(&now);
			tradingday = (ptmNow->tm_year + 1900) * 10000 + (ptmNow->tm_mon + 1) * 100 + ptmNow->tm_mday;
		}
		*_TradingDay = tradingday;
	}

	if (_ActionDay == nullptr)
		return;

	// ActionDay处理
	if ((ActionDay != nullptr) && (strlen(ActionDay) == 8))
	{
		*_ActionDay = atoi(ActionDay);
	}
	else
	{
		time_t now = time(0);
		struct tm *ptmNow = localtime(&now);
		if (HH >= 23)
		{
			if (ptmNow->tm_hour<1)
			{
				now -= 86400;
				ptmNow = localtime(&now);
			}
		}
		else if (HH<1)
		{
			if (ptmNow->tm_hour >= 23)
			{
				now += 86400;
				ptmNow = localtime(&now);
			}
		}

		*_ActionDay = (1900 + ptmNow->tm_year) * 10000 + (1 + ptmNow->tm_mon) * 100 + ptmNow->tm_mday;
	}
}

void GetExchangeTime_Undefined(int iTradingDay, char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec)
{
	// 由于CTP期货中行情没有提供交易所，所以一开始就无法按交易所来分类

	// UpdateTime处理
	int HH = GetUpdateTime(UpdateTime, _UpdateTime, UpdateMillisec);

	// TradingDay处理
	if (_TradingDay)
	{
		int tradingday = 0;
		if ((HH > 18 || HH < 6))
		{
			tradingday = iTradingDay;
		}
		else
		{
			tradingday = atoi(TradingDay);
		}
		if (tradingday == 0)
		{
			time_t now = time(0);
			struct tm *ptmNow = localtime(&now);
			tradingday = (ptmNow->tm_year + 1900) * 10000 + (ptmNow->tm_mon + 1) * 100 + ptmNow->tm_mday;
		}
		*_TradingDay = tradingday;
	}

	if (_ActionDay == nullptr)
		return;

	// ActionDay处理
	if ((HH>6 && HH<18) && (ActionDay != nullptr) && (strlen(ActionDay) == 8))
	{
		*_ActionDay = atoi(ActionDay);
	}
	else
	{
		time_t now = time(0);
		struct tm *ptmNow = localtime(&now);
		if (HH >= 23)
		{
			if (ptmNow->tm_hour<1)
			{
				now -= 86400;
				ptmNow = localtime(&now);
			}
		}
		else if (HH<1)
		{
			if (ptmNow->tm_hour >= 23)
			{
				now += 86400;
				ptmNow = localtime(&now);
			}
		}

		*_ActionDay = (1900 + ptmNow->tm_year) * 10000 + (1 + ptmNow->tm_mon) * 100 + ptmNow->tm_mday;
	}
}

void GetExchangeTime(char* TradingDay, char* ActionDay, char* UpdateTime, int* _TradingDay, int* _ActionDay, int* _UpdateTime, int* UpdateMillisec)
{
	// TradingDay处理
	if (_TradingDay)
	{
		int tradingday = atoi(TradingDay);
		if (tradingday == 0)
		{
			time_t now = time(0);
			struct tm *ptmNow = localtime(&now);
			tradingday = (ptmNow->tm_year + 1900) * 10000 + (ptmNow->tm_mon + 1) * 100 + ptmNow->tm_mday;
		}
		*_TradingDay = tradingday;
	}

	// UpdateTime处理
	int HH = GetUpdateTime(UpdateTime, _UpdateTime, UpdateMillisec);

	if (_ActionDay == nullptr)
		return;

	// ActionDay处理
	if ((ActionDay != nullptr) && (strlen(ActionDay) == 8))
	{
		*_ActionDay = atoi(ActionDay);
	}
	else
	{
		time_t now = time(0);
		struct tm *ptmNow = localtime(&now);
		if (HH >= 23)
		{
			if (ptmNow->tm_hour<1)
			{
				now -= 86400;
				ptmNow = localtime(&now);
			}
		}
		else if (HH<1)
		{
			if (ptmNow->tm_hour >= 23)
			{
				now += 86400;
				ptmNow = localtime(&now);
			}
		}

		*_ActionDay = (1900 + ptmNow->tm_year) * 10000 + (1 + ptmNow->tm_mon) * 100 + ptmNow->tm_mday;
	}
}

void GetExchangeTime(time_t Time, int* _TradingDay, int* _ActionDay, int* _UpdateTime)
{
	struct tm *ptmNow = localtime(&Time);

	int date = (ptmNow->tm_year + 1900) * 10000 + (ptmNow->tm_mon + 1) * 100 + ptmNow->tm_mday;
	int time = (ptmNow->tm_hour) * 10000 + (ptmNow->tm_min) * 100 + ptmNow->tm_sec;

	if (_TradingDay)
	{
		*_TradingDay = date;
	}

	if (_ActionDay)
	{
		*_ActionDay = date;
	}

	if (_UpdateTime)
	{
		*_UpdateTime = time;
	}
}

void GetOnFrontDisconnectedMsg(int ErrorId, char* ErrorMsg)
{
	switch (ErrorId)
	{
	case 0x1001:
		strcpy(ErrorMsg, "0x1001 4097 网络读失败");
		break;
	case 0x1002:
		strcpy(ErrorMsg, "0x1002 4098 网络写失败");
		break;
	case 0x2001:
		strcpy(ErrorMsg, "0x2001 8193 接收心跳超时");
		break;
	case 0x2002:
		strcpy(ErrorMsg, "0x2002 8194 发送心跳失败");
		break;
	case 0x2003:
		strcpy(ErrorMsg, "0x2003 8195 收到错误报文");
		break;
	case 0x2004:
		strcpy(ErrorMsg, "0x2004 8196 服务器主动断开");
		break;
	default:
		sprintf(ErrorMsg, "%x %d 未知错误", ErrorId, ErrorId);
		break;
	}
}

#if defined _WIN32 || WIN32 || _WINDOWS
void GetDllPathByFunctionName(const char* szFunctionName, char* szPath)
{
	HMODULE hModule = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, szFunctionName, &hModule);

	GetModuleFileNameA(hModule, szPath, MAX_PATH);
}

void GetExePath(char* szPath)
{
	HMODULE hModule = GetModuleHandleA(nullptr);
	//char szPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(hModule, szPath, MAX_PATH);
}

void GetDirectoryByPath(const char* szPath, char* szDirectory)
{
	//char szPath[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath(szPath, drive, dir, fname, ext);
	_makepath(szDirectory, drive, dir, nullptr, nullptr);
}

void GetNewPathInSameDirectory(const char* szPath, const char* szFname, const char* szExt, char* szFileName)
{
	//char szPath[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath(szPath, drive, dir, fname, ext);
	_makepath(szFileName, drive, dir, szFname, szExt);
}
#else
void GetDllPathByFunctionName(const char* szFunctionName, char* szPath)
{}
void GetExePath(char* szPath)
{}
void GetDirectoryByPath(const char* szPath, char* szDirectory)
{}
void GetNewPathInSameDirectory(const char* szPath, const char* szFname, const char* szExt, char* szFileName)
{}

#endif
