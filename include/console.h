#pragma once
#include <vector>
#include <string>
#include <sstream>

class ConsoleCout {
public:
	template<typename T>
	ConsoleCout& operator<<(const T& data) {
		buffer << data;
		return *this;
	}
	
	ConsoleCout& operator<<(std::ostream& (*manip)(std::ostream&));
	
	static void AddToConsole(const std::string&);
	static void ClearConsole();
	static const std::vector<std::string>& GetLogs();

private:
	std::stringstream buffer;
	static std::vector<std::string> consoleOutput;
};
