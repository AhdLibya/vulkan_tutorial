#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace ahd {

	class ahdwindow {

		public:
			ahdwindow(int w , int h , std::string name);
			~ahdwindow();

			ahdwindow(const ahdwindow&) = delete;
			ahdwindow& operator = (const ahdwindow&) = delete;

			bool ShoudClose() { return glfwWindowShouldClose(window); };

		private :

			void initWindow();

			const int width;
			const int hight;

			std::string windowName;

			GLFWwindow* window;
	};
}


