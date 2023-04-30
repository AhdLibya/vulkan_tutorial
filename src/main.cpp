#include "first_app.h"

// std
#include <stdlib.h>
#include <stdexcept>
#include <iostream>

int main() {

	AHD::first_app app{};

	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}