#pragma once


#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

namespace AHD {

	class ahdwindow {

		public:
			ahdwindow(int w , int h , std::string name);
			~ahdwindow();

			ahdwindow(const ahdwindow&) = delete;
			ahdwindow& operator = (const ahdwindow&) = delete;

			bool ShoudClose() { return glfwWindowShouldClose(window); };

			void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface_);

			VkExtent2D getExten() { return { static_cast<uint32_t>(width) , static_cast<uint32_t>(hight) }; };

			bool wasWindowResized() { return frameBufferReszie; };

			void resetWindowResize() { frameBufferReszie = false; };

		private :

			static void frameBufferResizeWindowCallBack(GLFWwindow* window, int width ,int height);

			void initWindow();

			int width;
			int hight;

			bool frameBufferReszie = false;

			std::string windowName;

			GLFWwindow* window;
	};
}


