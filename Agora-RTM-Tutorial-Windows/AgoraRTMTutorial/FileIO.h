#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <vector>
#include <map>
class CFileIO
{
public:
	CFileIO();
	~CFileIO();

	void openLog(const std::string &filepath, int fileFlage = OPEN_ALWAYS);
	void openMedia(const std::string &filepath, int fileFlage = CREATE_ALWAYS);
	void openReadFile(const std::string &filePath);
	void close();
	int write(std::string bufferStr);
	int read(char *bufferOut, int bufferLen);
	std::string readLine();
	std::string read();

public:
	static bool generatorFile(const std::string &path);
	void writeHeadBlock();

protected:
	int write(char *bufferIn, int bufferLen);

private:
	HANDLE fileHandle_;
	int filelimitLine_;
	bool isLog_;
};

class CFileIni
{
public:
	CFileIni();
	CFileIni(const std::string &filePath);
	~CFileIni();

	bool openFile(const std::string &IniFile);
	bool write(const std::string &section, const std::string &key, const std::string &Value);
	std::string read(const std::string &section, const std::string &key);
	bool getSectionName(std::vector<std::string> &vecSection);
	bool getSection(const std::string &section, std::map<std::string, std::string> &mapKeyValue);
	std::string getFilePath(){ return iniFile_; }

private:
	std::string iniFile_;
	bool isValid_;
};

class CIniBase
{
public:
	CIniBase(const std::string &filePath);
	virtual ~CIniBase();
	std::string getFilePath(){ return pIniInstance_->getFilePath(); }

#define __DECLARE_INICONFIG_FUN(CMDID)\
		std::string get##CMDID();\
		int set##CMDID(const std::string &v);

#define __IMPLEMENT_INICONFIG_FUN(CLASSNAME,CMDID,FIELD,KEY)\
		std::string CLASSNAME::get##CMDID()	{\
		return pIniInstance_->read(FIELD,KEY);}\
		int CLASSNAME::set##CMDID(const std::string &v)	{\
		return pIniInstance_->write(FIELD,KEY,v);}

#define __DECLARE_INICONFIG_SIMILAR_FUN(CMDID)\
		std::string get##CMDID(const std::string &filed);\
		int set##CMDID(const std::string &filed,const std::string &v);

#define __IMPLEMENT_INICONFIG_SIMILAR_FUN(CLASSNAME,CMDID,KEY)\
		std::string CLASSNAME::get##CMDID(const std::string &filed){\
		return pIniInstance_->read(filed,KEY);}\
		int CLASSNAME::set##CMDID(const std::string &filed,const std::string &v){\
		return pIniInstance_->write(filed,KEY,v);}

protected:
	CFileIni *pIniInstance_;
};

#define INI_BASEINFO					"BaseInfo"
#define INI_BASEINFO_APPID				"AppID"
#define INI_BASEINFO_CERTID			    "AppCertificatedId"
#define INI_BASEINFO_UPLOADMEDIAID		"LastUploadMediaId"
#define INI_BASEINFO_DOWNLOADMEDIAID	"LastDownloadMediaId"
class CConfigSignal:public CIniBase
{
public:
	CConfigSignal();
	CConfigSignal(const std::string &path);
	~CConfigSignal();

public:

	__DECLARE_INICONFIG_FUN(AppID)
	__DECLARE_INICONFIG_FUN(AppCertificatedId)
    __DECLARE_INICONFIG_FUN(LastUploadMediaId)
    __DECLARE_INICONFIG_FUN(LastDownloadMediaId)
private:

};

#endif
