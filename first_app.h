#pragma once

#include "window.h"

namespace ahd {
	class first_app
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HIGHT = 800;
			void run();
		private:
			ahdwindow ahdwindow{WIDTH , HIGHT , "Hello vulkon "};
	};

}

