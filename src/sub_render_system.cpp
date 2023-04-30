#include "sub_render_system.h"
#include "defines.h"
#include <iostream>
#include <stdexcept>
#include <array>
//#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>



namespace AHD {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;
	};


	SubRenderSystem::SubRenderSystem(ahdDevice& device, VkRenderPass renderPass): AhdDevice{device}
	{
		createPiplineLayout();
		createPipline(renderPass);
	}

	SubRenderSystem::~SubRenderSystem() {
		vkDestroyPipelineLayout(AhdDevice.device(), piplineLayout, nullptr);
	}



	void SubRenderSystem::createPiplineLayout() {

		VkPushConstantRange PushConstantRange{};
		PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		PushConstantRange.offset = 0;
		PushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo piplineLayoutInfo{};
		piplineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		piplineLayoutInfo.setLayoutCount = 0;
		piplineLayoutInfo.pSetLayouts = nullptr;
		piplineLayoutInfo.pushConstantRangeCount = 1;
		piplineLayoutInfo.pPushConstantRanges = &PushConstantRange;

		if (vkCreatePipelineLayout(AhdDevice.device(), &piplineLayoutInfo, nullptr, &piplineLayout) != VK_SUCCESS) {
			throw std::runtime_error("faild to create pipline layout");
		}
	}

	void SubRenderSystem::createPipline(VkRenderPass renderPass) {
		assert(piplineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PiplineConfigInfo piplineConfig{};
		AhdPipline::defaultPiplineConfigInfo(piplineConfig);
		piplineConfig.renderPass = renderPass;
		piplineConfig.pipelineLayout = piplineLayout;

		ahdPipline = std::make_unique<AhdPipline>(AhdDevice, "Shaders/sample_shader.vert.spv", "Shaders/sample_shader.frag.spv", piplineConfig);
		LOG("Pipline created");
	}


	void SubRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer , std::vector<GameObject>& gameObjects, const Camera& camera)
	{
		ahdPipline->bind(commandBuffer);
		for (auto& object : gameObjects)
		{

			object.transform.rotation.y = glm::mod(object.transform.rotation.y + 0.01f, glm::two_pi<float>());
			object.transform.rotation.x = glm::mod(object.transform.rotation.x + 0.005f, glm::two_pi<float>());
			SimplePushConstantData push{};
			push.color = object.color;
			push.transform = camera.getProjection() * object.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				piplineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			object.model->bind(commandBuffer);
			object.model->draw(commandBuffer);
		}
	}

}