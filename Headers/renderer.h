#pragma once

#include "window.h"
#include "ahd_device.hpp"
#include "ahd_swap_chain.hpp"
// std
#include <memory>
#include <vector>
#include <cassert>

namespace AHD {
	class Renderer
	{
	public:
		
		Renderer(ahdwindow& window, ahdDevice& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;

		bool isFrameInPrgress() const { return isFrameStarted; };
		VkRenderPass getRenderPass() const { return ahdSwapChain->getRenderPass(); };

		float getAspectRatio() { return ahdSwapChain->extentAspectRatio(); }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot Get Command Buffer when frame is not in progress");
			return commandBuffers[currentImageIndex]; 
		};
		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}
		VkCommandBuffer beginFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer);
		void endFrame();
	private:
		
		void createCommandBuffer();
		void freeCommandBuffers();
		void reCreateSwapChain();
		
		ahdwindow& AhdWindow;
		ahdDevice& AhdDevice;
		std::unique_ptr<AhdSwapChain> ahdSwapChain;

		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};
}


