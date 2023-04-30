#pragma once

// ahd engine
#include "window.h"
#include "ahd_device.hpp"
#include "GameObject.h"
#include "renderer.h"
// std
#include <memory>
#include <vector>

namespace AHD {
	class first_app
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HIGHT = 800;
			void run();

			first_app();
			~first_app();

			first_app(const first_app&) = delete;
			first_app& operator = (const first_app&) = delete;

		private:
			void loadGameObjects();
			
			ahdwindow AhdWindow{WIDTH , HIGHT , "VULKAN_RENDER_DEMP"};
			ahdDevice AhdDevice{ AhdWindow };
			Renderer renderer{ AhdWindow  ,AhdDevice };
			std::vector<GameObject> gameObjects;
	};

}

