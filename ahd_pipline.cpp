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

		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		std::string ERROMESSAGE = "[ERROR]: Failed To Open File!\n [FILE]: ";

		if (!file.is_open())
		{
			throw std::runtime_error(ERROMESSAGE.append(filepath));
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	void AhdPipline::createGraphicsPipline(const std::string& vertexfile, const std::string& fragmentfile, const PiplineConfigInfo& configInfo) {
		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot Create Gp-pipline ::  Pipline layout Not Found in ConfigInfo ");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot Create Gp-pipline ::  renderpass Not Found in ConfigInfo ");
		std::cout << "Creating GP Pipline" << '\n';
		auto vertex   = readfile(vertexfile);
		auto fragment = readfile(fragmentfile);
		std::cout << "Creating GP Pipline stage 1" << '\n';
		createShaderModule(vertex  , &vertShaderModule);
		createShaderModule(fragment, &fragShaderModule);
		VkPipelineShaderStageCreateInfo shaderStage[2];
		std::cout << "Creating GP Pipline stage 2" << '\n';
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


		auto bindingDescriotions   = Model::Vertex::getBindingDescriptions();
		auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();

		std::cout << "Creating GP Pipline stage 3" << '\n';
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(bindingDescriotions.size());
		vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions      = bindingDescriotions.data();
		/*
		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports    = &configInfo.viewport;
		viewportInfo.scissorCount  = 1;
		viewportInfo.pScissors     = &configInfo.scissor;
		*/
		
		

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
		piplineInfo.pDynamicState       = &configInfo.dynamicStateInfo;

		piplineInfo.layout     = configInfo.pipelineLayout;
		piplineInfo.renderPass = configInfo.renderPass;
		piplineInfo.subpass    = configInfo.subpass;

		piplineInfo.basePipelineIndex  = -1;
		piplineInfo.basePipelineHandle = VK_NULL_HANDLE;

		std::cout << "Creating GP Pipline stage 4" << '\n';
		if (vkCreateGraphicsPipelines(AhdDevice.device(), VK_NULL_HANDLE, 1, &piplineInfo, nullptr, &GraphicsPipline) != VK_SUCCESS)
		{
			throw std::runtime_error("faild to create graphics pipline");
		}

	}

	void AhdPipline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		std::cout << "Creating Shader Module stage 1" << '\n';
		VkShaderModuleCreateInfo createShaderModuleInfo{};
		createShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createShaderModuleInfo.codeSize = code.size();
		createShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(AhdDevice.device(), &createShaderModuleInfo, nullptr, shaderModule) != VK_SUCCESS) {
			std::cout << "failed to create shader module" << '\n';
			throw std::runtime_error("failed to create shader module");
		}
		std::cout << "Creating Shader Module stage 2" << '\n';
	}

	void AhdPipline:: bind(VkCommandBuffer commandbuffer) {
		vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipline);
	}

	void AhdPipline::defaultPiplineConfigInfo(PiplineConfigInfo& configInfo) {

		std::cout << "creating PiplineConfig Info struct" << '\n';
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		std::cout << "Compile stage 1" << '\n';

		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports    =  nullptr;
		configInfo.viewportInfo.scissorCount  = 1;
		configInfo.viewportInfo.pScissors     = nullptr;
		configInfo.viewportInfo.flags = 0;
		std::cout << "Compile stage 2" << '\n';
		
		std::cout << "Compile stage 4" << '\n';
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
		std::cout << "Compile stage 5" << '\n';
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
		configInfo.multisampleInfo.flags = 0;
		std::cout << "Compile stage 6" << '\n';
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
		std::cout << "Compile stage 7" << '\n';
		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
		std::cout << "Compile stage 8" << '\n';
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.flags = 0;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};
		std::cout << "Compile stage 9" << '\n';

		configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
		configInfo.dynamicStateInfo.dynamicStateCount =
			static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
		configInfo.dynamicStateInfo.flags = 0;
	}
}