#ifndef __COMMONFUN_H__
#define __COMMONFUN_H__

std::string getAbsoluteDir();
std::string getFilePath();
std::string getCurRunningExeName();
std::string getFileAbsolutePath(const std::string &file);
std::string getPirorDir(const std::string &file);
std::string getRootDir(const std::string &file);

std::string int2str(int nNum);
std::string float2str(float fValue);
std::string gbk2utf8(const char *gbk);
std::string utf82gbk(const char *utf8);
std::string gbk2utf8(const std::string &gbk);
std::string utf82gbk(const std::string &utf8);
int str2int(const std::string &str);
float str2float(const std::string &str);
CString s2cs(const std::string &str);
std::string cs2s(const CString &str);

bool isContailSpace(CString str);
void initlog();
void uninitLog();

typedef enum eTagLogType
{
	LogType_NULL = -1,
	LogType_SDK_Error,
	LogType_API_Operator,
	LogType_CallBack,
	LogType_SDK_LOG,

	LogType_Unknown = 0xff,
}AG_SIGNAL_LOGTYPE;

void LOG_MSG(const std::string &msgInfo,AG_SIGNAL_LOGTYPE eLogType =  LogType_SDK_Error);


#endif