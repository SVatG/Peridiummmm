	const int32_t numCubeVertices = 8;
	const vertex_t cubeVertices[] = {
		{ { F( -1 ), F( -1 ), F(  1 ) }, { F(0), F(0), F(0) } }, // 0: upper front left
		{ { F(  1 ), F( -1 ), F(  1 ) }, { F(0), F(0), F(0) } }, // 1: upper front right
		{ { F(  1 ), F(  1 ), F(  1 ) }, { F(0), F(0), F(0) } }, // 2: lower front right
		{ { F( -1 ), F(  1 ), F(  1 ) }, { F(0), F(0), F(0) } }, // 3: lower front left
		{ { F( -1 ), F( -1 ), F( -1 ) }, { F(0), F(0), F(0) } }, // 4: upper back  left
		{ { F(  1 ), F( -1 ), F( -1 ) }, { F(0), F(0), F(0) } }, // 5: upper back  right
		{ { F(  1 ), F(  1 ), F( -1 ) }, { F(0), F(0), F(0) } }, // 6: lower back  right
		{ { F( -1 ), F(  1 ), F( -1 ) }, { F(0), F(0), F(0) } }  // 7: lower back  left
	};

	const int32_t numCubeFaces = 12;
	const index_triangle_t cubeFaces[] = {
		// Front
		{0, 1, 2},
		{0, 2, 3},

		// Back
		{4, 7, 6},
		{4, 6, 5},

		// Right
		{1, 5, 6},
		{1, 6, 2},

		// Left
		{4, 0, 3},
		{4, 3, 7},

		// Top
		{4, 5, 1},
		{4, 1, 0},

		// Bottom
		{7, 2, 6},
		{7, 3, 2}
	};
	