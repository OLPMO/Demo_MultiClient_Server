#ifndef _F_LOGGER_H_
#define _F_LOGGER_H_

#include <map>
#include <string>

#include <stdio.h>
#include <stdarg.h>


typedef struct SLOG_FILE
{
	unsigned int logCount;
	std::string filepath;
} *SLOG_FILE_PTR;


class FLogger
{
private:

	FLogger()
	{
		// Do Nothing
	}

	~FLogger()
	{
		// Do Nothing
	}


public:

	// 打开一个日志文件
	// Parm : name 分配的标识,通过此标识对日志文件进行读写操作
	// Parm : filepath 日志文件路径
	// Return : 是否打开成功
	static bool addLog(const char *name, const char *filepath)
	{
		if(name == nullptr || filepath == nullptr) 
			return false;

		FLogger *logger = FLogger::sharedInstance();
		return logger->addFilepathIntoList(name, filepath);
	}


	// 关闭一个日志文件
	// Parm : name 日志标识
	// Return : 是否关闭成功
	static bool removeLog(const char *name)
	{
		FLogger *logger = FLogger::sharedInstance();
		return logger->removeFilepathFromList(name);
	}


	// 清空一个日志文件
	// Parm : name 日志标识
	// Return 是否清空成功
	static bool clearLog(const char *name)
	{
		FLogger *logger = FLogger::sharedInstance();
		return logger->resetLogFileInfo(name);
	}


	// 输出一行字符串日志
	// Parm : name 日志文件标识
	// Parm : str 待输出的字符串
	// Return : 是否输出成功
	static bool Log(const char *name, const char *str)
	{
		FLogger *logger = FLogger::sharedInstance();
		return logger->printStrToLogFile(name, str);
	}


	// 输出一个整数
	// Parm : name 日志文件标识
	// Parm : val 待输出的整数
	// Return : 是否输出成功
	static bool Log(const char *name, const int val)
	{
		char str[64] = { 0 };
		sprintf_s(str, "%d", val);
		return FLogger::Log(name, str);
	}


	// 输出一个浮点数
	// Parm : name 日志文件标识
	// Parm : val 待输出的浮点数
	// Return : 是否输出成功
	static bool Log(const char *name, const float val)
	{
		char str[64] = { 0 };
		sprintf_s(str, "%f", val);
		return FLogger::Log(name, str);
	}


	// 输出一个布尔值
	// Parm : name 日志文件标识
	// Parm : val 待输出的布尔值
	// Return : 是否输出成功
	static bool Log(const char *name, const bool val)
	{
		char *str[2] = { "false", "true" };
		return FLogger::Log(name, str[val]);
	}


	// 格式化输出日志
	// Parm : name 日志文件标识
	// Parm : format 输出格式,和printf中的第二个参数一致
	// Parm : argnum 不定参数个数,此处是为了和输出字符串的Log做区分
	// Parm : ... 不定参数
	// Return : 是否输出成功
	static bool Log(const char *name, const char *format, int argnum, ...)
	{
		va_list arglist;
		va_start(arglist, argnum);

		char strFormat[128] = { 0 };
		strcpy_s(strFormat, format);

		char str[512] = { 0 };
		const char *ptrStrStart = strFormat;
		for(char *ptr = strFormat; *ptr != '\0'; ptr++)
		{
			if(*ptr != '%') continue;

			switch(*(ptr + 1))
			{
			case 'c':
				*ptr = '\0';
				sprintf_s(str, "%s%s%c", str, ptrStrStart, va_arg(arglist, char));
				ptrStrStart = ptr + 2;
				break;

			case 'd': 
				*ptr = '\0';
				sprintf_s(str, "%s%s%d", str, ptrStrStart, va_arg(arglist, int));
				ptrStrStart = ptr + 2;
				break;

			case 'u': 
				*ptr = '\0';
				sprintf_s(str, "%s%s%u", str, ptrStrStart, va_arg(arglist, unsigned));
				ptrStrStart = ptr + 2;
				break;

			case 'f': 
				*ptr = '\0';
				sprintf_s(str, "%s%s%.4f", str, ptrStrStart, va_arg(arglist, double));
				ptrStrStart = ptr + 2;
				break;

			case 's': 
				*ptr = '\0';
				sprintf_s(str, "%s%s%s", str, ptrStrStart, va_arg(arglist, char*));
				ptrStrStart = ptr + 2;
				break;

			default: 
				break;
			}

		} // end for ptr

		sprintf_s(str, "%s%s", str, ptrStrStart);

		va_end(arglist);

		return FLogger::Log(name, str);
	}


private:

	// 获取全局单例,只在内部静态方法内获取
	// Return : 全局单例指针
	static FLogger* sharedInstance(void)
	{
		static FLogger instance;
		return &instance;
	}


	/// 判断标识名是否已使用
	// Parm : name 日志标识名
	// Return : 标识名是否已使用
	bool isNameUsed(const char *name)
	{
		std::map<std::string, SLOG_FILE>::iterator itor = mapFilePath.find(name);
		return itor != mapFilePath.end();
	}


	// 输出一条字符串到文件
	// Parm : name 日志文件标识
	// Parm : str 待输出的字符串
	// Return : 是否输出成功
	bool printStrToLogFile(const char *name, const char *str)
	{
		std::map<std::string, SLOG_FILE>::iterator itor = mapFilePath.find(name);
		if(itor != mapFilePath.end())
		{
			SLOG_FILE &logFile = itor->second;

			FILE *fp = nullptr;
			fopen_s(&fp, logFile.filepath.c_str(), "a+");
			if(fp)
			{
				fprintf_s(fp, "[%u] %s \n\n", logFile.logCount++, str);
				fclose(fp);
				return true;
			}
		}

		return false;
	}


	// 重置一条日志内容
	// Parm : name 日志文件标识
	// Return : 是否重置成功
	bool resetLogFileInfo(const char *name)
	{
		std::map<std::string, SLOG_FILE>::iterator itor = mapFilePath.find(name);
		if(itor != mapFilePath.end())
		{
			SLOG_FILE &logFile = itor->second;
			logFile.logCount = 1;

			FILE *fp = nullptr;
			fopen_s(&fp, logFile.filepath.c_str(), "w+");
			if(fp)
			{
				fclose(fp);
				return true;
			}
		}

		return false;
	}


	// 插入一条键值对记录
	// Parm : name 日志标识
	// Parm : filepath 日志文件路径
	// Return : 是否添加成功
	bool addFilepathIntoList(const char *name, const char *filepath)
	{
		if(!isNameUsed(name))
		{
			SLOG_FILE logFileInfo;
			logFileInfo.logCount = 1;
			logFileInfo.filepath = filepath;
			mapFilePath.insert(std::pair<std::string, SLOG_FILE>(name, logFileInfo));

			return true;
		}

		return false;
	}


	// 删除一条键值对记录
	// Parm : name 日志标识
	// Return : 是否移除成功
	bool removeFilepathFromList(const char *name)
	{
		std::map<std::string, SLOG_FILE>::iterator itor = mapFilePath.find(name);
		if(itor == mapFilePath.end())
			return false;

		mapFilePath.erase(itor);

		return true;
	}


private:

	std::map<std::string, SLOG_FILE> mapFilePath;

};



#endif