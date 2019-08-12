#include <windows.h>
#include <shlobj.h>
#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

#include <glog/logging.h>

enum LogLevel
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

class GLogger
{
public:
	GLogger();
	~GLogger();

	void Write(const char* function, std::string log, LogLevel level = LOG_INFO);

private:
	inline void Write_Info(const std::string&, const std::string&, const std::string&);
	inline void Write_Warning(const std::string&, const std::string&, const std::string&);
	inline void Write_Error(const std::string&, const std::string&, const std::string&);

	inline std::string GetModuleNameByStack(unsigned short layer);
	inline std::string GetLogDatePath();
	inline std::string GetLogDateINFOPath(std::string);
};

// Init glog
GLogger::GLogger()
{
	std::string logDatePath = GetLogDatePath();
	FLAGS_log_dir = logDatePath;
	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, GetLogDateINFOPath(logDatePath).c_str());
	google::SetStderrLogging(google::GLOG_INFO);
	google::SetLogFilenameExtension("log_");
	FLAGS_colorlogtostderr = true;              // Set log color
	FLAGS_logbufsecs = 0;                       // Set log output speed(s)
	FLAGS_max_log_size = 1024;                  // Set max log file size
	FLAGS_stop_logging_if_full_disk = true;     // If disk is full
}

GLogger::~GLogger()
{
	google::ShutdownGoogleLogging();
}

// Write log
void GLogger::Write(const char* function, std::string log, LogLevel level)
{
	if (log.empty())
		return;

	std::string fileName(GetModuleNameByStack(1));
	if (fileName.empty())
		fileName = "etc";

	std::string functionName(function);

	switch (level)
	{
	case LOG_WARNING:
		Write_Warning(fileName, functionName, log);
		break;
	case LOG_ERROR:
		Write_Error(fileName, functionName, log);
		break;
	default:
		Write_Info(fileName, functionName, log);
		break;
	}
}

inline void GLogger::Write_Info(const std::string& fileName, const std::string& functionName, const std::string& logContent)
{
	LOG(INFO) << "[" << fileName << " " << functionName << "] " << logContent;
}

inline void GLogger::Write_Warning(const std::string& fileName, const std::string& functionName, const std::string& logContent)
{
	LOG(WARNING) << "[" << fileName << " " << functionName << "] " << logContent;
}

inline void GLogger::Write_Error(const std::string& fileName, const std::string& functionName, const std::string& logContent)
{
	LOG(ERROR) << "[" << fileName << " " << functionName << "] " << logContent;
}

// Get module name (exe/dll)
inline std::string GLogger::GetModuleNameByStack(unsigned short layer)
{
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);

	void* stack[100];
	unsigned short frames = CaptureStackBackTrace(0, 100, stack, NULL);
	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = MAX_SYM_NAME;

	if (frames >= ++layer)
	{
		SymFromAddr(process, (DWORD64)(stack[2]), 0, symbol);

		HMODULE hModule = NULL;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCTSTR)(stack[layer]), &hModule);

		if (hModule != NULL)
		{
			char moduleName[256];
			lstrcpyA(moduleName, "");
			GetModuleFileNameA(hModule, moduleName, 256);

			std::string name(moduleName);
			if (!name.empty())
			{
				size_t nPos = name.rfind("\\");
				name = name.substr(nPos + 1, name.size() - 1);
				nPos = name.rfind(".");
				name = name.substr(0, nPos);
				return name;
			}
		}
	}
	return "";
}

// Get Log Date path: %appdata%\GLogger\Log\yyyy_mm_dd
inline std::string GLogger::GetLogDatePath()
{
	// Get appdata
	char path[MAX_PATH];
	if (::SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path) != S_OK)
	{
		return "";
	}

	// date
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	std::string date(std::to_string(sys.wYear) + '_'
		+ std::to_string(sys.wMonth) + '_'
		+ std::to_string(sys.wDay));

	// appdata + log + date
	strcat(path, "\\GLogger\\Log\\");
	strcat(path, date.c_str());
	if (SHCreateDirectoryExA(NULL, path, NULL) != ERROR_SUCCESS
		&& ERROR_FILE_EXISTS != GetLastError()
		&& ERROR_ALREADY_EXISTS != GetLastError())
	{
		return "";
	}
	return std::string(path);
}

// Get INFO path
inline std::string GLogger::GetLogDateINFOPath(std::string logDatePath)
{
	logDatePath += "\\INFO";
	if (SHCreateDirectoryExA(NULL, logDatePath.c_str(), NULL) != ERROR_SUCCESS
		&& ERROR_FILE_EXISTS != GetLastError()
		&& ERROR_ALREADY_EXISTS != GetLastError())
	{
		return "";
	}
	return logDatePath;
}

int main()
{
	GLogger glogger;
	glogger.Write(__FUNCTION__, "Log test!!!!!!!!!!!!!!");
	glogger.Write(__FUNCTION__, "(LOG_INFO) Log test!!!!!!!!!!!!!!", LOG_INFO);
	glogger.Write(__FUNCTION__, "(LOG_WARNING) Log test!!!!!!!!!!!!!!", LOG_WARNING);
	glogger.Write(__FUNCTION__, "(LOG_ERROR) Log test!!!!!!!!!!!!!!", LOG_ERROR);
	getchar();

	return 0;
}