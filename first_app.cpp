#include "first_app.h"


namespace ahd {

	void first_app::run() {

		while (!ahdwindow.ShoudClose()) {
			glfwPollEvents();
		}

	}
}