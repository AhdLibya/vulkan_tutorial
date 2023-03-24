#pragma once

#include "ahd_device.hpp"
#include "Model.h"
// std 
#include <string>
#include <vector>

namespace ahd {

	struct PiplineConfigInfo { 
		PiplineConfigInfo(const PiplineConfigInfo&) = delete;
		PiplineConfigInfo& operator=(const PiplineConfigInfo&) = delete;
		PiplineConfigInfo() = default;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class AhdPipline {
		public :
			AhdPipline(ahdDevice &device , const std::string& vertexfile , const std::string& fragmentfile , const PiplineConfigInfo& configInfo);
			~AhdPipline();
			
			AhdPipline(const AhdPipline&) = delete;

			AhdPipline operator=(const AhdPipline&) = delete;

			static void defaultPiplineConfigInfo(PiplineConfigInfo& configInfo);
			void bind(VkCommandBuffer commandbuffer);


		private:
			static std::vector<char> readfile(const std::string& filepath);

			void createGraphicsPipline(const std::string& vertexfile, const std::string& fragmentfile , const PiplineConfigInfo& configInfo);

			void createShaderModule(const std::vector<char> &code , VkShaderModule *shaderModule);

			ahdDevice& AhdDevice;
			VkPipeline GraphicsPipline;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;
	};
}
