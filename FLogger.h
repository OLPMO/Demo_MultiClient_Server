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

	// ��һ����־�ļ�
	// Parm : name ����ı�ʶ,ͨ���˱�ʶ����־�ļ����ж�д����
	// Parm : filepath ��־�ļ�·��
	// Return : �Ƿ�򿪳ɹ�
	static bool addLog(const char *name, const char *filepath)
	{
		if(name == nullptr || filepath == nullptr) 
			return false;

		FLogger *logger = FLogger::sharedInstance();
		return logger->addFilepathIntoList(name, filepath);
	}


	// �ر�һ����־�ļ�
	// Parm : name ��־��ʶ
	// Return : �Ƿ�رճɹ�
	static bool removeLog(const char *name)
	{
		FLogger *logger = FLogger::sharedInstance();
		return logger->removeFilepathFromList(name);
	}


	// ���һ����־�ļ�
	// Parm : name ��־��ʶ
	// Return �Ƿ���ճɹ�
	static bool clearLog(const char *name)
	{
		FLogger *logger = FLogger::sharedInstance();
		return logger->resetLogFileInfo(name);
	}


	// ���һ���ַ�����־
	// Parm : name ��־�ļ���ʶ
	// Parm : str ��������ַ���
	// Return : �Ƿ�����ɹ�
	static bool Log(const char *name, const char *str)
	{
		FLogger *logger = FLogger::sharedInstance();
		return logger->printStrToLogFile(name, str);
	}


	// ���һ������
	// Parm : name ��־�ļ���ʶ
	// Parm : val �����������
	// Return : �Ƿ�����ɹ�
	static bool Log(const char *name, const int val)
	{
		char str[64] = { 0 };
		sprintf_s(str, "%d", val);
		return FLogger::Log(name, str);
	}


	// ���һ��������
	// Parm : name ��־�ļ���ʶ
	// Parm : val ������ĸ�����
	// Return : �Ƿ�����ɹ�
	static bool Log(const char *name, const float val)
	{
		char str[64] = { 0 };
		sprintf_s(str, "%f", val);
		return FLogger::Log(name, str);
	}


	// ���һ������ֵ
	// Parm : name ��־�ļ���ʶ
	// Parm : val ������Ĳ���ֵ
	// Return : �Ƿ�����ɹ�
	static bool Log(const char *name, const bool val)
	{
		char *str[2] = { "false", "true" };
		return FLogger::Log(name, str[val]);
	}


	// ��ʽ�������־
	// Parm : name ��־�ļ���ʶ
	// Parm : format �����ʽ,��printf�еĵڶ�������һ��
	// Parm : argnum ������������,�˴���Ϊ�˺�����ַ�����Log������
	// Parm : ... ��������
	// Return : �Ƿ�����ɹ�
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

	// ��ȡȫ�ֵ���,ֻ���ڲ���̬�����ڻ�ȡ
	// Return : ȫ�ֵ���ָ��
	static FLogger* sharedInstance(void)
	{
		static FLogger instance;
		return &instance;
	}


	/// �жϱ�ʶ���Ƿ���ʹ��
	// Parm : name ��־��ʶ��
	// Return : ��ʶ���Ƿ���ʹ��
	bool isNameUsed(const char *name)
	{
		std::map<std::string, SLOG_FILE>::iterator itor = mapFilePath.find(name);
		return itor != mapFilePath.end();
	}


	// ���һ���ַ������ļ�
	// Parm : name ��־�ļ���ʶ
	// Parm : str ��������ַ���
	// Return : �Ƿ�����ɹ�
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


	// ����һ����־����
	// Parm : name ��־�ļ���ʶ
	// Return : �Ƿ����óɹ�
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


	// ����һ����ֵ�Լ�¼
	// Parm : name ��־��ʶ
	// Parm : filepath ��־�ļ�·��
	// Return : �Ƿ���ӳɹ�
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


	// ɾ��һ����ֵ�Լ�¼
	// Parm : name ��־��ʶ
	// Return : �Ƿ��Ƴ��ɹ�
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