#pragma once

#include "window.h"
#include "ahd_pipline.h"

namespace ahd {
	class first_app
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HIGHT = 800;
			void run();
		private:
			ahdwindow ahdwindow{WIDTH , HIGHT , "Hello vulkon "};
			AhdPipline AhdPipline{ "Shaders\\sample_shader.vert.spv" , "Shaders\\sample_shader.frag.spv" };
	};

}

