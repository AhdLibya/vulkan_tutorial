#pragma once

#include "window.h"
#include "ahd_pipline.h"
#include "ahd_device.hpp"
#include "ahd_swap_chain.hpp"
#include "Model.h"
// std
#include <memory>
#include <vector>

namespace ahd {
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
			void loadModels();
			void createPiplineLayout();
			void createPipline();
			void createCommandBuffer();
			void freeCommandBuffers();
			void drawFrame();
			void reCreateSwapChain();
			void recordCommandBuffer(int imageIndex);
			
			ahdwindow AhdWindow{WIDTH , HIGHT , "Hello vulkon "};
			ahdDevice AhdDevice{ AhdWindow };
			std::unique_ptr<AhdSwapChain> ahdSwapChain;

			std::unique_ptr<AhdPipline> ahdPipline;
			VkPipelineLayout piplineLayout;
			std::vector<VkCommandBuffer> commandBuffers;
			std::unique_ptr<Model> model;
			
			
	};

}

