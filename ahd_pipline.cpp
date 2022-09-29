#include "ahd_pipline.h"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>



namespace ahd {

	AhdPipline::AhdPipline(ahdDevice& device, const std::string& vertexfile, const std::string& fragmentfile, const PiplineConfigInfo& configInfo) : AhdDevice{device} {
		createGraphicsPipline(vertexfile , fragmentfile , configInfo);
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
		auto vertex   = readfile(vertexfile);
		auto fragment = readfile(fragmentfile);

		std::cout << "fragment file size is :" << fragment.size() << std::endl;
		std::cout << "vertex file size is : "  << vertex.size() << std::endl;

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
}