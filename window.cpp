#include "window.h"

#include <stdexcept>
namespace ahd {

	ahdwindow::ahdwindow(int h, int w, std::string name) : width{ w }, hight{ h }, windowName{ name } {
		initWindow();
	}

	ahdwindow::~ahdwindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void ahdwindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface_) {
		if (glfwCreateWindowSurface(instance , window , nullptr , surface_) != VK_SUCCESS) {
			throw std::runtime_error("faild to create window surface");
		}
	}

	void ahdwindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		 window = glfwCreateWindow(width , hight , windowName.c_str() , nullptr , nullptr);

	}

}
