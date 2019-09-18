#include "stdafx.h"
#include "commonFun.h"
#include "FileIO.h"

#define MAXPATHLEN 1000
std::string getAbsoluteDir()
{
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(nullptr, path, MAXPATHLEN);

	std::string filePath = CStringA(path).GetBuffer();
	return filePath.substr(0, filePath.rfind("\\") + 1);
}

std::string getFilePath()
{
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(nullptr, path, MAXPATHLEN);
	return CStringA(path).GetBuffer();
}

std::string getCurRunningExeName()
{
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(nullptr, path, MAXPATHLEN);

	std::string filePath = CStringA(path).GetBuffer();
	return filePath.substr(filePath.rfind("\\") + 1, filePath.length() - filePath.rfind("\\"));
}

std::string getFileAbsolutePath(const std::string &file)
{
	HMODULE hModule = GetModuleHandle(CString(file.c_str()));
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(hModule, path, MAXPATHLEN);
	return CStringA(path).GetBuffer();
}

std::string getPirorDir(const std::string &file)
{
	HMODULE hModule = GetModuleHandle(CString(file.c_str()));
	TCHAR path[MAXPATHLEN] = { 0 };
	GetModuleFileName(hModule, path, MAXPATHLEN);
	std::string fullpath = CStringA(path).GetBuffer();
	return fullpath.substr(0, fullpath.rfind("\\") + 1);
}

std::string getRootDir(const std::string &file)
{
	std::string FileDir = getFileAbsolutePath(file);
	return FileDir.substr(0, FileDir.find("\\") + 1);
}

std::string int2str(int nNum)
{
	char str[MAXPATHLEN] = { 0 };
	_itoa_s(nNum, str, 10);
	return str;
}

std::string float2str(float fValue)
{
	char str[MAXPATHLEN] = { 0 };
	sprintf_s(str, "%f", fValue);
	return str;
}

int str2int(const std::string &str)
{
	return atoi(str.c_str());
}

float str2float(const std::string &str)
{
	return (float)atof(str.c_str());
}

CString s2cs(const std::string &str)
{
	return CString(str.c_str());
}

std::string cs2s(const CString &str)
{
	CString sTemp(str);
	return CStringA(sTemp.GetBuffer()).GetBuffer();
}

std::string utf82gbk(const char *utf8)
{
	std::string str;

	if (utf8 != NULL)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		std::wstring strW;

		strW.resize(len);

		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)strW.data(), len);

		len = WideCharToMultiByte(936, 0, strW.data(), len - 1, NULL, 0, NULL, NULL);	

		str.resize(len);

		WideCharToMultiByte(936, 0, strW.data(), -1, (LPSTR)str.data(), len, NULL, NULL);
	}

	return str;
}

std::string gbk2utf8(const char *gbk)
{
	std::string str;

	if (gbk != NULL)
	{
		int len = MultiByteToWideChar(936, 0, gbk, -1, NULL, 0);
		std::wstring strW;

		strW.resize(len);

		MultiByteToWideChar(936, 0, gbk, -1, (LPWSTR)strW.data(), len);

		len = WideCharToMultiByte(CP_UTF8, 0, strW.data(), len - 1, NULL, 0, NULL, NULL);

		str.resize(len);

		WideCharToMultiByte(CP_UTF8, 0, strW.data(), -1, (LPSTR)str.data(), len, NULL, NULL);
	}

	return str;
}

std::string gbk2utf8(const std::string &gbk)
{
	return gbk2utf8(gbk.c_str());
}

std::string utf82gbk(const std::string &utf8)
{
	return utf82gbk(utf8.c_str());
}

bool isContailSpace(CString str)
{
	return str.Find(_T(' ')) > 0;
}

void initlog()
{
	std::string strLogPath = getAbsoluteDir() + "AgoraSignalSDK.log";

	gLogSignal.openLog(strLogPath);
}

void uninitLog()
{
	gLogSignal.close();
}

void LOG_MSG(const std::string &msgInfo, AG_SIGNAL_LOGTYPE eLogType /*= LogType_SDK_Error*/)
{
	char LogDesc[MAXPATHLEN] = { '\0' };
	SYSTEMTIME st;
	GetLocalTime(&st);

	switch (eLogType)
	{
	case AG_SIGNAL_LOGTYPE::LogType_SDK_Error:
	{
		sprintf_s(LogDesc, "[SDK_ERROR] %02d:%02d:%02d:%03d ; %s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msgInfo.data());
	}
	break;
	case AG_SIGNAL_LOGTYPE::LogType_SDK_LOG:
	{
		sprintf_s(LogDesc, "[SDK_LOG] %02d:%02d:%02d:%03d ; %s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msgInfo.data());
	}
	break;;
	case AG_SIGNAL_LOGTYPE::LogType_API_Operator:
	{
		sprintf_s(LogDesc, "[SDK_API] %02d:%02d:%02d:%03d ; %s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msgInfo.data());
	}
	break;
	case AG_SIGNAL_LOGTYPE::LogType_CallBack:
	{
		sprintf_s(LogDesc, "[SDK_CALLBACK] %02d:%02d:%02d:%03d ; %s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msgInfo.data());
	}
	break;;
	default:break;
	}

	if (strlen(LogDesc)){
		gLogSignal.write(LogDesc);
	}
}