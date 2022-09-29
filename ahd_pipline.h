#pragma once

#include "ahd_device.hpp"

// std 
#include <string>
#include <vector>

namespace ahd {

	struct PiplineConfigInfo { };

	class AhdPipline {
		public :
			AhdPipline(ahdDevice &device , const std::string& vertexfile , const std::string& fragmentfile , const PiplineConfigInfo& configInfo);
			~AhdPipline() {};

			AhdPipline(const AhdPipline&) = delete;

			void operator=(const AhdPipline&) = delete;

			static PiplineConfigInfo defaultPiplineConfigInfo(uint32_t width , uint32_t height);

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
