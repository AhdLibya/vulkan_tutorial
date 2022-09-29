#include "ahd_pipline.h"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>


namespace ahd {

	AhdPipline::AhdPipline(ahdDevice& device, const std::string& vertexfile, const std::string& fragmentfile, const PiplineConfigInfo& configInfo) : AhdDevice{device} {
		createGraphicsPipline(vertexfile , fragmentfile , configInfo);
	}

	AhdPipline :: ~AhdPipline() {
		vkDestroyShaderModule(AhdDevice.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(AhdDevice.device(), fragShaderModule, nullptr);
		vkDestroyPipeline(AhdDevice.device(), GraphicsPipline, nullptr);
	}

	std::vector<char> AhdPipline::readfile(const std::string& filepath) {

		std::ifstream file(filepath, std::ios::ate, std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("faild to open file path at : " + filepath);
		}

		size_t filesize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(filesize);

		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();
		
		return buffer;
	}

	void AhdPipline::createGraphicsPipline(const std::string& vertexfile, const std::string& fragmentfile, const PiplineConfigInfo& configInfo) {
		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot Create Gp-pipline ::  Pipline layout Not Found in ConfigInfo ");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot Create Gp-pipline ::  renderpass Not Found in ConfigInfo ");

		auto vertex   = readfile(vertexfile);
		auto fragment = readfile(fragmentfile);
		
		createShaderModule(vertex  , &vertShaderModule);
		createShaderModule(fragment, &fragShaderModule);
		VkPipelineShaderStageCreateInfo shaderStage[2];
		//vertex
		shaderStage[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStage[0].module = vertShaderModule;
		shaderStage[0].pName  = "main";
		shaderStage[0].flags  = 0;
		shaderStage[0].pNext  = nullptr;
		shaderStage[0].pSpecializationInfo = nullptr;
		// fragment
		shaderStage[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStage[1].module = fragShaderModule;
		shaderStage[1].pName  = "main";
		shaderStage[1].flags  = 0;
		shaderStage[1].pNext  = nullptr;
		shaderStage[1].pSpecializationInfo = nullptr;


		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount   = 0;
		vertexInputInfo.pVertexAttributeDescriptions    = nullptr;
		vertexInputInfo.pVertexBindingDescriptions      = nullptr;
		// Pipline Info Setup :)
		VkGraphicsPipelineCreateInfo piplineInfo{};
		piplineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		piplineInfo.stageCount          = 2;
		piplineInfo.pStages             = shaderStage;
		piplineInfo.pVertexInputState   = &vertexInputInfo;
		piplineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		piplineInfo.pViewportState      = &configInfo.viewportInfo;
		piplineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		piplineInfo.pMultisampleState   = &configInfo.multisampleInfo;
		piplineInfo.pColorBlendState    = &configInfo.colorBlendInfo;
		piplineInfo.pDepthStencilState  = &configInfo.depthStencilInfo;
		piplineInfo.pDynamicState       = nullptr;

		piplineInfo.layout     = configInfo.pipelineLayout;
		piplineInfo.renderPass = configInfo.renderPass;
		piplineInfo.subpass    = configInfo.subpass;

		piplineInfo.basePipelineIndex  = -1;
		piplineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(AhdDevice.device(), VK_NULL_HANDLE, 1, &piplineInfo, nullptr, &GraphicsPipline) != VK_SUCCESS)
		{
			throw std::runtime_error("faild to create graphics pipline");
		}

	}

	void AhdPipline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(AhdDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Faild To Create Shader Module");
		}
	}

	PiplineConfigInfo AhdPipline::defaultPiplineConfigInfo(uint32_t width, uint32_t height) {
		PiplineConfigInfo configInfo{};

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height };

		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports = &configInfo.viewport;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = &configInfo.scissor;

		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};

		return configInfo;
	}
}