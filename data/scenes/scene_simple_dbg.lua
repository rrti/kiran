
root = {
	raytracer = {
		numThreads = 2,
		maxRayDepth = 1,
		antiAliasing = 1,
		incrementalRender = 1,
	},

	window = {
		xsize = 1440,
		ysize =  900,
		title = "Kiran",

		autoShow = 1,
		keepOpen = 1,
		makeDump = 1,
	},

	scene = {
		camera = {
			-- pos = { 0.0,   5.0,   1.0},
			-- pos = { 1.0,   7.0,   1.0},
			-- pos = { 0.0,   5.0,   1.0},
			-- pos = { 6.0,   5.0,   3.0},
			-- pos = { 4.0,   7.0,   0.0},
			-- pos = { 0.0, 35.0, -5.0},
			-- pos = {10.0,  7.0, 10.0},
			-- vrp = { 0.0,  4.0,  0.0},
			--
			pos = {22.0, 14.0, 18.0},
			vrp = { 0.0,  6.0,  0.0},
		--	pos = { 0.0,  4.0, 18.0},
		--	vrp = { 0.0,  4.0,  0.0},
			vfov = 90.0,

			renderDOF    =  0,
			fplaneDist   = 20.0,
			lensAperture =  2,
		},


		lights = {
			[1] = {
				type      = "positional",
				position  = {  0.0,  125.0,   0.0},
				direction = {  0.0,   -1.0,   0.0},
				power     = {255.0,  255.0, 255.0},
				fov       = 180.0,
				radius    = 10.0,
			},

			--[[
			[2] = {
				type      = "positional",
				position  = { 125.0,  125.0, 525.0},
				direction = {  -1.0,    0.0,   0.0},
				power     = { 255.0,  255.0, 255.0},
				fov       = 180.0,
			},
			--]]
		},


		objects = {
			[1] = {
				type     = "ellipse",
				position = { 0.0, 5.0, -10.0},
				size     = {4.0, 4.0, 4.0},
				material = "glassBlue",
			},

			[2] = {
				type     = "ellipse",
				position = { 0.0,  5.0, -25.0},
				size     = {3.0, 6.0, 3.0},
				material = "glassGreen",
			},

			[3] = {
				type     = "box",
				position = { 0.0,  2.0,  12.0},
				size     = { 5.0,  3.0,   3.0},
				material = "glassGreen",
			},

			[4] = {
				type     = "cylinder",
				position = {  6.0,  6.0,  0.0},
				size     = {  8.0,  1.0,  2.0},  -- (h, ry, rz)
				axis     = 0,
				material = "glassGreen",
			},
			[5] = {
				type     = "cylinder",
				position = {  0.0,  8.0,  0.0},
				size     = {  8.0,  1.0,  1.0},  -- (h, rx, rz)
				axis     = 1,
				material = "glassGreen",
			},
			[6] = {
				type     = "cylinder",
				position = {  0.0,  6.0,  6.0},
				size     = {  8.0,  1.0,  2.0},  -- (h, rx, ry)
				axis     = 2,
				material = "glassGreen",
			},

			[7] = {
				type     = "plane",
				normal   = {0.0, 1.0 * 100, 0.0},
				distance = -2,
				material = "glassRed",
			},
		},


		materials = {
			[1] = {
				type                   = "glassBlue",
				texture                = "glass.png",
			--	refractionIndex        = 1.333 * 100.0,
				refractionIndex        = 0.950 * 100.0,
				specularExponent       = 12.0 * 100.0,

				diffuseReflectiveness  = {0.0 * 100, 0.0 * 100, 1.0 * 100},
				specularReflectiveness = {1.0 * 100, 1.0 * 100, 1.0 * 100},
			},
			[2] = {
				type                   = "glassGreen",
				texture                = "glass.png",
				refractionIndex        = 0.00 * 100.0,
				specularExponent       = 12.0 * 100.0,

				diffuseReflectiveness  = {0.0 * 100, 1.0 * 100, 0.0 * 100},
				specularReflectiveness = {1.0 * 100, 1.0 * 100, 1.0 * 100},
			},

			[3] = {
				type                   = "glassRed",
				texture                = "glass.png",
				refractionIndex        = 0, --1.0 * 100.0;
				specularExponent       = 25.0 * 100.0,

				diffuseReflectiveness  = {1.0 * 100, 0.0 * 100, 0.0 * 100},
				specularReflectiveness = {1.0 * 100, 1.0 * 100, 1.0 * 100},
			},
		},
	},
}
