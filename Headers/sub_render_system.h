#pragma once

// ahd engine
#include "ahd_pipline.h"
#include "ahd_device.hpp"
#include "GameObject.h"
#include "camera.h"
// std
#include <memory>
#include <vector>

namespace AHD {
	class SubRenderSystem
	{
	public:
		
		SubRenderSystem(ahdDevice& device, VkRenderPass renderPass);
		~SubRenderSystem();

		SubRenderSystem(const SubRenderSystem&) = delete;
		SubRenderSystem& operator = (const SubRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std:: vector<GameObject>& gameObjects, const Camera& camera);

	private:
		void createPiplineLayout();
		void createPipline(VkRenderPass renderPass);
		

		ahdDevice& AhdDevice;
		std::unique_ptr<AhdPipline> ahdPipline;
		VkPipelineLayout piplineLayout;
	};

}