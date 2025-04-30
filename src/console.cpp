// console.cpp
#include "console.h"

std::vector<std::string> ConsoleCout::consoleOutput;

ConsoleCout& ConsoleCout::operator<<(std::ostream& (*manip)(std::ostream&)) {
	if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
		AddToConsole(buffer.str());
		buffer.str("");
		buffer.clear();
	} else {
		buffer << manip;
	}
	return *this;
}

void ConsoleCout::AddToConsole(const std::string& text) {
	consoleOutput.push_back(text);
	if (consoleOutput.size() > 100) {
		consoleOutput.erase(consoleOutput.begin());
	}
}

void ConsoleCout::ClearConsole() {
	consoleOutput.clear();
}

const std::vector<std::string>& ConsoleCout::GetLogs() {
	return consoleOutput;
}