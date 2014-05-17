root = {
	raytracer = {
		numRayThreads = 2,
		maxRayDepth = 4,
		antiAliasing = 0,
		incrementalRender = 1,
	},

	window = {
		xsize = 640,
		ysize = 480,
		title = "Kiran",

		autoShow = 1,
		keepOpen = 1,
		makeDump = 1,
	},


	scene = {
		camera = {
			-- pos = {0.0, 0.0, 1.0},
			-- vrp = {0.0, 0.0, 0.0},

			-- pos = { 2.0, 2.0,   2.0},
			-- vrp = { 0.0, 0.0,  -2.0},

			-- pos = {-4.0, 10.0,   5.0},
			-- vrp = {-4.0, 10.0, -10.0},

			-- pos = {-4.0,  4.0,   5.0},
			-- vrp = {-4.0,  0.0, -10.0},

			pos = {-35.0, 15.0,  35.0},
			vrp = {  0.0,  8.0, -10.0},
			vfov = 45.0,

			fplaneDist = 25.0,
		},


		lights = {
			[1] = {
				type       = "positional",
				position   = {  0.0,  125.0,   0.0},
				direction  = {  0.0,   -1.0,   0.0},
				power      = {100.0,  100.0, 100.0},
				numPhotons = 0,
				fov        = 180.0,
				radius     = 5.0,
			},
			[2] = {
				type       = "positional",
				position   = { 25.0,   25.0, 100.0},
				direction  = {  0.0,    0.0,  -1.0},
				power      = {100.0,    0.0,   0.0},
				numPhotons = 0,
				fov        = 180.0,
				radius     = 5.0,
			},
			[3] = {
				type       = "positional",
				position   = {  0.0,   25.0, 100.0},
				direction  = {  0.0,    0.0,  -1.0},
				power      = {  0.0,  100.0,   0.0},
				numPhotons = 0,
				fov        = 180.0,
				radius     = 5.0,
			},
			[4] = {
				type       = "positional",
				position   = {100.0,   25.0,   0.0},
				direction  = { -1.0,    0.0,   0.0},
				power      = {  0.0,    0.0, 100.0},
				numPhotons = 0,
				fov        = 180.0,
				radius     = 5.0,
			},
		},


		objects = {
			[1] = {
				type     = "ellipse",
				position = {  -5.0, 15.0, -15.0},
				size     = {4.0, 4.0, 4.0},
				material = "glassR",
			},

			[2] = {
				type     = "ellipse",
				position = {  0.0, 10.0, -10.0},
				size     = {3.0, 3.0, 3.0},
				material = "glassG",
			},

			[3] = {
				type     = "ellipse",
				position = {  5.0, 5.0, -5.0},
				size     = {2.0, 2.0, 2.0},
				material = "glassB",
			},

			[4] = {
				type     = "ellipse",
				position = { -5.0, 5.0, -7.5},
				size     = {2.0, 2.0, 2.0},
				material = "glassW",
			},

			[5] = {
				type     = "plane",
				normal   = {0.0, 1.0 * 100, 0.0},
				distance = 1,
				material = "glassW",
			},

			--[[
			[4] = {
				type = "mesh",
				model = "car.obj",
			}
			--]]
		},


		materials = {
			[1] = {
				type                   = "glassR",
				texture                = "glass.png",
				refractionIndex        = 1.333 * 100.0,
				specularExponent       = 12.0 * 100.0,

				diffuseReflectiveness  = {1.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularReflectiveness = {1.0 * 100.0, 1.0 * 100.0, 1.0 * 100.0},
			},

			[2] = {
				type                   = "glassG",
				texture                = "glass.png",
				refractionIndex        = 1.333 * 100.0,
				specularExponent       = 15.0 * 100.0,

				diffuseReflectiveness  = {0.0 * 100.0, 1.0 * 100.0, 0.0 * 100.0},
				specularReflectiveness = {1.0 * 100.0, 1.0 * 100.0, 1.0 * 100.0},
			},

			[3] = {
				type                   = "glassB",
				texture                = "glass.png",
				refractionIndex        = 1.333 * 100.0,
				specularExponent       = 11.0 * 100.0,

				diffuseReflectiveness  = {0.0 * 100.0, 0.0 * 100.0, 1.0 * 100.0},
				specularReflectiveness = {1.0 * 100.0, 1.0 * 100.0, 1.0 * 100.0},
			},

			[4] = {
				type                   = "glassW",
				texture                = "glass.png",
				refractionIndex        = 1.333 * 100.0,
				specularExponent       = 25.0 * 100.0,

				diffuseReflectiveness  = {1.0 * 100.0, 1.0 * 100.0, 1.0 * 100.0},
				specularReflectiveness = {1.0 * 100.0, 1.0 * 100.0, 1.0 * 100.0},
			},
		},
	},
}
