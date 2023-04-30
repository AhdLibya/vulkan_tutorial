#include "Renderer.h"

//std
#include <iostream>
#include <stdexcept>
#include <array>



namespace AHD {

	Renderer::Renderer(ahdwindow& window, ahdDevice& device) :AhdDevice{ device }, AhdWindow{ window } 
	{
		reCreateSwapChain();
		createCommandBuffer();
	}

	Renderer::~Renderer() { freeCommandBuffers(); }

	void Renderer::reCreateSwapChain()
	{
		auto extent = AhdWindow.getExten();

		while (extent.width == 0 || extent.height == 0)
		{
			extent = AhdWindow.getExten();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(AhdDevice.device());

		if (ahdSwapChain == nullptr)
			ahdSwapChain = std::make_unique<AhdSwapChain>(AhdDevice, extent);
		else
		{
			std::shared_ptr<AhdSwapChain> oldSwapChain = std::move(ahdSwapChain);
			ahdSwapChain = std::make_unique<AhdSwapChain>(AhdDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*ahdSwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
		// re create sginal
	}

	void Renderer::createCommandBuffer() 
	{
		commandBuffers.resize(AhdSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = AhdDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());


		if (vkAllocateCommandBuffers(AhdDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("[ERROR]:: fiald to allocate command buffers");
		}
	}


	void Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(AhdDevice.device(), AhdDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}


	VkCommandBuffer Renderer::beginFrame()
	{
		assert(!isFrameStarted && "Con't beginFrame while frame already in progress");

		auto result = ahdSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			reCreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("[ERROR]:: Faild To acquire Swap Chain Image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("[ERROR]:: fiald to record command buffer");
		}
		return commandBuffer;
	}
	void Renderer::endFrame()
	{
		assert(isFrameStarted && "Con't Call endFrame while frame already in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("[ERROR]:: fiald to record command buffer");
		}

		auto result = ahdSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || AhdWindow.wasWindowResized())
		{
			AhdWindow.resetWindowResize();
			reCreateSwapChain();
		}else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("[ERROR]:: Faild To presnt swap chain image");
		}

		isFrameStarted = false;

		currentFrameIndex = (currentFrameIndex + 1) % AhdSwapChain::MAX_FRAMES_IN_FLIGHT;
	}
	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Con't Call beginFrame while frame already in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Con't begin Render Pass on command buffer from different frame");


		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = ahdSwapChain->getRenderPass();
		renderPassBeginInfo.framebuffer = ahdSwapChain->getFrameBuffer(currentImageIndex);

		renderPassBeginInfo.renderArea.offset = { 0 , 0 };
		renderPassBeginInfo.renderArea.extent = ahdSwapChain->getSwapChainExtent();


		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f , 0.01f , 0.01f ,0.1f };
		clearValues[1].depthStencil = { 1.0f , 0 };
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(ahdSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(ahdSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, ahdSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Con't end frame while frame already in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Con't end Render Pass on command buffer from different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}