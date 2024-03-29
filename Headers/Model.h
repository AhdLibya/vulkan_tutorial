#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ahd_device.hpp"



namespace AHD {
	class Model
	{

	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		Model(ahdDevice& device, const std::vector<Vertex>& vertices);
		~Model();

		Model(const Model&) = delete;
		Model& operator = (const Model&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:

		void createVertexBuffers(const std::vector<Vertex> &vertices);

		ahdDevice &AhdDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};

}

