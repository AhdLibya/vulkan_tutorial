// ahd engine
#include "first_app.h"
#include "sub_render_system.h"


//std
#include <iostream>
#include <stdexcept>
#include <array>
//#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>



namespace AHD {



	first_app::first_app() {
		loadGameObjects();
	}

	first_app::~first_app() { }

	void first_app::run() {

		SubRenderSystem RenderSystem{ AhdDevice , renderer.getRenderPass() };
        Camera camera{};
        
		while (!AhdWindow.ShoudClose()) {
			glfwPollEvents();

            float aspct = renderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspct, aspct, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspct, .1f, 10.f);

			if (auto commandBuffer = renderer.beginFrame())
			{
				renderer.beginSwapChainRenderPass(commandBuffer);
				RenderSystem.renderGameObjects(commandBuffer , gameObjects, camera);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}
		vkDeviceWaitIdle(AhdDevice.device());
	}


    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<Model> createCubeModel(ahdDevice& device, glm::vec3 offset) {
        std::vector<Model::Vertex> vertices{
            // Positions            // Colors
            // left face (white)
            {{-.5f, -.5f, -.5f}   , {.9f, .9f, .9f}},
            {{-.5f,  .5f,  .5f}   , {.9f, .9f, .9f}},
            {{-.5f, -.5f,  .5f}   , {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}   , {.9f, .9f, .9f}},
            {{-.5f,  .5f, -.5f}   , {.9f, .9f, .9f}},
            {{-.5f,  .5f,  .5f}   , {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}  , {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}    , {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}   , {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}  , {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}   , {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}    , {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f} , {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}   , {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}  , {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f} , {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}  , {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}   , {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}  , {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}    , {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}   , {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}  , {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}   , {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}    , {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f} , {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}   , {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}  , {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f} , {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}  , {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}   , {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}  , {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f} , {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f} , {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}  , {.1f, .8f, .1f}},

        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<Model>(device, vertices);
    }

	void first_app::loadGameObjects()
	{
        std::shared_ptr<Model> Model = createCubeModel(AhdDevice, { 0.0f,0.0f,0.0f });

        auto cube = GameObject::createGameObject();
        cube.model = Model;
        cube.transform.translation = { 0.0f , 0.0f , 2.5f };
        cube.transform.scale = { 0.5f , 0.5f , 0.5f };
        gameObjects.push_back(std::move(cube));
	}
}


/*


```
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

int main() {
    std::vector<Vertex> vertices = {
        // Front face
        { { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } },
        { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } },
        { {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } },
        { { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f } },

        // Back face
        { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
        { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
        { {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
        { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },

        // Top face
        { { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } },
        { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } },
        { {  1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } },

        // Bottom face
        { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f } },
        { {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f } },
        { {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f } },
        { { -1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f } },

        // Right face
        { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } },
        { {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } },
        { {  1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f } },
        { {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f } },

        // Left face
        { { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f } },
        { { -1.0f, -1.0f,  1.0f }, { 1.0f,
*/