#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <ctime>

class MiniLogger {
public:
	MiniLogger();
	~MiniLogger();
	static void CreateLogger(std::string filenamepath);
	static void Info(std::string);
	static void Warn(std::string);
	static void Error(std::string);
	static void Fatal(std::exception*, std::string);
	static void Log(std::exception*, std::string, std::string);
private:
	static std::ofstream filehandle;
};