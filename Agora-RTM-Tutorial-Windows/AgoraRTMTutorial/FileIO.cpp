#include "stdafx.h"
#include "FileIO.h"
#include <assert.h>
#include "commonFun.h"

CFileIO::CFileIO() :
fileHandle_(nullptr)
, filelimitLine_(0)
, isLog_(false)
{
}

CFileIO::~CFileIO()
{
	fileHandle_ = nullptr;
	filelimitLine_ = 0;
	isLog_ = false;
}

void CFileIO::openLog(const std::string &filepath, int fileFlage /*= OPEN_ALWAYS */)
{
	assert(nullptr == fileHandle_);
	fileHandle_ = CreateFile(
		CString(filepath.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		/*CREATE_NEW | OPEN_ALWAYS | TRUNCATE_EXISTING*/fileFlage,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (INVALID_HANDLE_VALUE == fileHandle_)
	{
		fileHandle_ = nullptr;
		printf("文件创建失败!!!!!!\n");
		int retCode = GetLastError();
		if (ERROR_ALREADY_EXISTS == retCode)
		{
			printf("文件已经存在,创建文件失败!!!!\n");
		}
	}

	isLog_ = true;
	SetFilePointer(fileHandle_, 0, nullptr, FILE_END);
	writeHeadBlock();
}

void CFileIO::openMedia(const std::string &filepath, int fileFlage /*= CREATE_ALWAYS*/)
{
	assert(nullptr == fileHandle_);
	fileHandle_ = CreateFile(CString(filepath.c_str()),
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		nullptr,
		fileFlage,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (INVALID_HANDLE_VALUE == fileHandle_)
	{
		fileHandle_ = nullptr;
		printf("文件创建失败!!!!!!\n");
		int retCode = GetLastError();
		if (ERROR_ALREADY_EXISTS == retCode)
		{
			printf("文件已经存在,创建文件失败!!!!\n");
		}
	}
}

void CFileIO::openReadFile(const std::string &filePath)
{
	openLog(filePath);
	FlushFileBuffers(fileHandle_);
	SetFilePointer(fileHandle_, 0, nullptr, FILE_BEGIN);
}

void CFileIO::close()
{
	CloseHandle(fileHandle_);
	fileHandle_ = nullptr;
}

int CFileIO::write(char *bufferIn, int bufferLen)
{
	if (fileHandle_)
	{
		DWORD writenLen = 0;
		int res = WriteFile(fileHandle_, bufferIn, (DWORD)bufferLen, &writenLen, nullptr);
		if (0 == res)
		{
			printf("write buffer failed..retCode: %d!!!!!\n", GetLastError());
		}
		return int(writenLen);
	}

	return 0;
}

int CFileIO::write(std::string bufferStr)
{
	filelimitLine_++;
	if (isLog_)
	{
		bufferStr += "\r\n";
	}
	if (isLog_ && 100 == filelimitLine_)
	{//清空操作
		SetFilePointer(fileHandle_, 0, nullptr, FILE_BEGIN);
		SetEndOfFile(fileHandle_);
		filelimitLine_ = 0;
	}

	return write((char*)bufferStr.c_str(), bufferStr.length());
}

int CFileIO::read(char *bufferOut, int bufferLen)
{
	DWORD readLen = 0;
	int res = ReadFile(fileHandle_, bufferOut, bufferLen, &readLen, nullptr);
	if (0 == res)
	{
		printf("read buffer from file failed!!!,retCode: %d\n", GetLastError());
		return int(readLen);
	}
	return readLen;
}

std::string CFileIO::readLine()
{
	std::string strLine;
	char readTxt[2] = { 0 };
	DWORD readLen = 0; DWORD dwValue = 0;
	BOOL res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
	std::string chKey = "\r";
	while (true)
	{
		if (res && 1 == readLen && chKey == readTxt)
		{
			res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
			chKey = "\n";
			if (res && 1 == readLen && chKey == readTxt)
			{
				break;
			}
			else
			{
				printf("read error ,cloud read '\r\n'\n");
				return "";
			}
		}
		else if (res && 1 == readLen &&  chKey != readTxt)
		{
			strLine += readTxt;
			//dwValue = SetFilePointer(fileHandle_, readLen, nullptr, FILE_CURRENT);
			readLen = 0; ZeroMemory(readTxt, 2); dwValue = 0;
			res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
		}
		if (res && 0 == readLen)
		{
			break;
		}
	}
	return strLine;
}

std::string CFileIO::read()
{
	return "";
}

bool CFileIO::generatorFile(const std::string &path)
{
	HANDLE fileHandle = CreateFile(CString(path.c_str()),
		GENERIC_READ | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (INVALID_HANDLE_VALUE == fileHandle)
	{
		return false;
	}
	return true;
}

void CFileIO::writeHeadBlock()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char logDesc[MAXPATHLEN] = { '\0' };
	sprintf_s(logDesc, "\r\n Local Date: %d %02d %2d Local Time: %02d:%02d:%02d \r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	write(logDesc);
}

CFileIni::CFileIni() :isValid_(false)
{

}

CFileIni::CFileIni(const std::string &filePath)
{
	iniFile_ = filePath;
	CFileIO::generatorFile(filePath);
}

CFileIni::~CFileIni()
{
	isValid_ = false;
}

bool CFileIni::openFile(const std::string &IniFile)
{
	iniFile_ = IniFile;
	return isValid_ = CFileIO::generatorFile(IniFile);
}

bool CFileIni::write(const std::string &section, const std::string &key, const std::string &Value)
{
	assert(isValid_);
	return (bool)(WritePrivateProfileString(s2cs(section), s2cs(key), s2cs(Value), s2cs(iniFile_)));
}

std::string CFileIni::read(const std::string &section, const std::string &key)
{
	assert(isValid_);
	std::string Value;
	TCHAR returnStr[MAXPATHLEN] = { 0 };
	GetPrivateProfileString(s2cs(section), s2cs(key), _T(""), returnStr, MAXPATHLEN, s2cs(iniFile_));
	Value = cs2s(returnStr);
	return Value;
}

bool CFileIni::getSectionName(std::vector<std::string> &vecSection)
{
	assert(isValid_);
	TCHAR returnStr[MAXPATHLEN] = { 0 }; std::string sectionItem;
	DWORD retNum = GetPrivateProfileSectionNames(returnStr, MAXPATHLEN, s2cs(iniFile_));
	if (0 < retNum)
	{
		int strLen = retNum;
		int nIndex = 0; TCHAR tchTemp = '\0';
		while (nIndex < strLen)
		{
			if ('\0' != (tchTemp = returnStr[nIndex]))
			{
				sectionItem += (tchTemp);
			}
			else
			{
				vecSection.push_back(sectionItem);
				sectionItem.clear();
			}
			nIndex++;
		}
	}
	return retNum > 0;
}

bool CFileIni::getSection(const std::string &section, std::map<std::string, std::string> &mapKeyValue)
{
	assert(isValid_);
	TCHAR returnStr[MAXPATHLEN] = { 0 }; std::string key; std::string value; bool isKey = true;
	DWORD retNum = GetPrivateProfileSection(s2cs(section), returnStr, MAXPATHLEN, s2cs(iniFile_));
	if (0 < retNum)
	{
		int strLen = retNum;
		int nIndex = 0; TCHAR tchTemp = '\0';
		while (nIndex < strLen)
		{
			if ('\0' != (tchTemp = returnStr[nIndex]))
			{
				if (L'=' == tchTemp)
				{
					isKey = false;
					nIndex++;
					continue;
				}
				if (isKey)
				{
					key += (tchTemp);
				}
				else
				{
					value += (tchTemp);
				}
			}
			else
			{
				mapKeyValue.insert(make_pair(key, value));
				key.clear(); value.clear();
				isKey = true;
			}
			nIndex++;
		}
	}
	return retNum > 0;
}

CIniBase::CIniBase(const std::string &filePath) :pIniInstance_(nullptr)
{
	pIniInstance_ = new CFileIni(filePath);
	assert(pIniInstance_);
}

CIniBase::~CIniBase()
{
	if (pIniInstance_)
	{
		delete pIniInstance_;
		pIniInstance_ = nullptr;
	}
}

CConfigSignal::CConfigSignal() :CIniBase("")
{
	std::string path = getAbsoluteDir() + "AgoraRTM.ini";
	pIniInstance_->openFile(path);
}

CConfigSignal::~CConfigSignal()
{

}

CConfigSignal::CConfigSignal(const std::string &path) :
CIniBase(path)
{

}


__IMPLEMENT_INICONFIG_FUN(CConfigSignal, AppID,INI_BASEINFO, INI_BASEINFO_APPID)
__IMPLEMENT_INICONFIG_FUN(CConfigSignal, AppCertificatedId, INI_BASEINFO, INI_BASEINFO_CERTID)