#include "first_app.h"
#include <iostream>
#include <stdexcept>
#include <array>
//#include <cassert>

namespace ahd {

	first_app::first_app() {
		loadModels();
		createPiplineLayout();
		reCreateSwapChain();
		createCommandBuffer();
	}

	first_app::~first_app() {
		vkDestroyPipelineLayout(AhdDevice.device(), piplineLayout, nullptr);
	}

	void first_app::run() {

		while (!AhdWindow.ShoudClose()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(AhdDevice.device());
	}

	void first_app::loadModels()
	{
		std::vector<Model::Vertex> vertices{
			// Positions        //Colors
			{{0.0f , -0.5f} , {1.0f , 0.0f , 0.0f}},
			{{0.5f , 0.5f}  , {0.0f , 1.0f , 0.0f}},
			{{-0.5f , 0.5f} , {0.0f , 0.0f , 1.0f}}
		};

		model = std::make_unique<Model>(AhdDevice, vertices);
	}

	void first_app::createPiplineLayout() {
		VkPipelineLayoutCreateInfo piplineLayoutInfo{};
		piplineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		piplineLayoutInfo.setLayoutCount = 0;
		piplineLayoutInfo.pSetLayouts = nullptr;
		piplineLayoutInfo.pushConstantRangeCount = 0;
		piplineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(AhdDevice.device(), &piplineLayoutInfo, nullptr, &piplineLayout) != VK_SUCCESS) {
			throw std::runtime_error("faild to create pipline layout");
		}
	}

	void first_app::createPipline() {
		assert(ahdSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(piplineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PiplineConfigInfo piplineConfig{};
		AhdPipline::defaultPiplineConfigInfo( piplineConfig );
		piplineConfig.renderPass = ahdSwapChain->getRenderPass(); 
		piplineConfig.pipelineLayout = piplineLayout;

		ahdPipline = std::make_unique<AhdPipline>( AhdDevice,  "Shaders/sample_shader.vert.spv", "Shaders/sample_shader.frag.spv", piplineConfig );
		std::cout << "Pipline created" << '\n';
	}

	void first_app::reCreateSwapChain() {
		auto extent = AhdWindow.getExten();

		while (extent.width == 0 || extent.height == 0)
		{
			extent = AhdWindow.getExten();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(AhdDevice.device());

		if (ahdSwapChain == nullptr)
			ahdSwapChain = std::make_unique<AhdSwapChain>(AhdDevice , extent);
		else
		{
			ahdSwapChain = std::make_unique<AhdSwapChain>(AhdDevice, extent, std::move(ahdSwapChain));
			if (ahdSwapChain->imageCount() != commandBuffers.size())
			{
				freeCommandBuffers();
				createCommandBuffer();
			}
		}
		createPipline();
	}

	void first_app:: createCommandBuffer(){
		
		commandBuffers.resize(ahdSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool        = AhdDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());


		if (vkAllocateCommandBuffers(AhdDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("[ERROR]:: fiald to allocate command buffers");
		}


	}


	void first_app::freeCommandBuffers()
	{
		vkFreeCommandBuffers(AhdDevice.device(), AhdDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void first_app::recordCommandBuffer(int imageIndex) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("[ERROR]:: fiald to record command buffer");
			}

			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = ahdSwapChain->getRenderPass();
			renderPassBeginInfo.framebuffer = ahdSwapChain->getFrameBuffer(imageIndex);

			renderPassBeginInfo.renderArea.offset = { 0 , 0 };
			renderPassBeginInfo.renderArea.extent = ahdSwapChain->getSwapChainExtent();


			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f , 0.1f , 0.1f ,0.1f };
			clearValues[1].depthStencil = { 1.0f , 0 };
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(ahdSwapChain->getSwapChainExtent().width);
			viewport.height = static_cast<float>(ahdSwapChain->getSwapChainExtent().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor{ {0, 0}, ahdSwapChain->getSwapChainExtent() };
			vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
			vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

			ahdPipline->bind(commandBuffers[imageIndex]);
			model->bind(commandBuffers[imageIndex]);
			model->draw(commandBuffers[imageIndex]);

			vkCmdEndRenderPass(commandBuffers[imageIndex]);
			if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			{
				throw std::runtime_error("[ERROR]:: fiald to record command buffer");
			}
	}
	void first_app :: drawFrame(){
		uint32_t imageIndex;
		auto result = ahdSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			reCreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("[ERROR]:: Faild To acquire Swap Chain Image");
		}

		recordCommandBuffer(imageIndex);
		result = ahdSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || AhdWindow.wasWindowResized())
		{
			AhdWindow.resetWindowResize();
			reCreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("[ERROR]:: Faild To presnt swap chain image");
		}

	}
}