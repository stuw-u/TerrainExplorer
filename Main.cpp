#include <iostream>

#include "Source/Application.h"



// Takes care of sending the proper error message in case of exception thrown while running the code.
void ErrorMessage(const std::string& message) {
#ifdef __WIN32
	MessageBox(nullptr, message.c_str(), "Error", MB_OK);
	std::cerr << message << std::endl;
#elif __linux__ || __APPLE__
	///@TODO Test
	const std::string command = "zenity --error --text \"" + message + "\"";
	system(command.c_str());
#else
	std::cerr << message << std::endl;
	std::cin.ignore();
#endif
}


int main() {

	try {
		Application app;
		app.RunGameLoop();
		return EXIT_SUCCESS;
	}
	catch(std::exception& e) {
		ErrorMessage(e.what());
		return EXIT_FAILURE;
	}
}