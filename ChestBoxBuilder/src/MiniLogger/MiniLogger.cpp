#include "../../Inc/MiniLogger/MiniLogger.h"

std::ofstream MiniLogger::filehandle;

MiniLogger::MiniLogger() {

}
MiniLogger::~MiniLogger() {
	if (filehandle.is_open()) {
		filehandle.close();
	}
}
void MiniLogger::CreateLogger(std::string filepath) {
	filehandle.open(filepath, std::ios::app);
}
void MiniLogger::Info(std::string msg) {
	Log(NULL, "Info", msg);
}
void MiniLogger::Warn(std::string msg) {
	Log(NULL, "Warning", msg);
}
void MiniLogger::Error(std::string msg) {
	Log(NULL, "Error", msg);
}
void MiniLogger::Fatal(std::exception* ex, std::string msg) 
{
	Log(ex, "Fatal", msg);
}
void MiniLogger::Log(std::exception* ex, std::string logType, std::string msg) {
	std::string exceptionMessage = "";
	if (ex != 0) {
		char exceptionMsg[MAX_PATH] = { 0 };
		sprintf_s(exceptionMsg, MAX_PATH, "[%s}", ex->what());
		exceptionMessage = std::string(exceptionMsg);
	}

	time_t now = time(0);
    struct tm ltm = { 0 };

	localtime_s(&ltm, &now);
	
	int year = (1900 + ltm.tm_year);
	int month = ltm.tm_mon + 1;
	int day = ltm.tm_mday;
	int hour = ltm.tm_hour;
	int min = ltm.tm_min;
	int sec = ltm.tm_sec;
	char datemsg[MAX_PATH] = { 0 };
	sprintf_s(datemsg, MAX_PATH, "[%d-%d-%d @ %d:%d:%d]", year, month, day, hour, min, sec);

	std::string date = std::string(datemsg);

	std::string logmessage = date + "[" + logType + "]" +exceptionMessage + " => " + msg + "\n";
	if (filehandle.is_open()) {
		filehandle << logmessage;
	}
}
