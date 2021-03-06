#include "../includes/logger.h"
using namespace std;

Logger::Logger(std::string filename) {
	this->filename = filename;
}

Logger::~Logger() {
	//close file if something happened while the file was still open
	lock_guard<mutex> lock(m);
	if (myFile.is_open())
		myFile.close();
}

//open close and clear the log file
void Logger::clearlogfile() {
	lock_guard<mutex> lock(m);
	myFile.open(filename, std::fstream::trunc);

	//close file
	if (myFile.is_open())
		myFile.close();
}

void Logger::log(std::string data) {
	lock_guard<mutex> lock(m);
	myFile.open(filename, std::fstream::app);
	if (!myFile.is_open())
		return;

	std::string myline;

	myFile << data;

	//close file
	if (myFile.is_open())
		myFile.close();
}
