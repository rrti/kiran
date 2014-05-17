root = {
	raytracer = {
		numRayThreads = 4,
		maxRayDepth = 4,
		antiAliasing = 0,
		incrementalRender = 1,

		-- PHOTON MAPPING PARAMETERS
		maxPhotonDepth = 10,
		photonSearchCount = 50,
		photonSearchRadius = 3.0,
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
		minBounds = {-25.0, -25.0, -25.0},
		maxBounds = { 25.0,  25.0,  25.0},

		camera = {
			pos    = { 0.0,  0.0, 50},
			vrp    = { 0.0,  0.0, 0.0},
			vplane = {16,   12,    0},
	--		vfov   = 90.0,

			renderDOF    =   0,
			fplaneDist   = 150.0,
			lensAperture =   2,
		},

		lights = {
			[1] = {
				type       = "positional",
				position   = { 0.0, 0.0, 0.0},
				power      = {1000.0, 1000.0, 1000.0},
				numPhotons = 25000,
				fov        = 360.0,
				radius     = 0.0,
			},
		},



		objects = {
			[1] = {
				-- bottom plane (floor)
				type     = "plane",
				normal   = {0.0, 1.0 * 100, 0.0},
				distance = -20,
				material = "mattWhite",
			},

			[2] = {
				-- top plane (ceiling)
				type     = "plane",
				normal   = {0.0, -1.0 * 100, 0.0},
				distance = -20,
				material = "mattWhite",
			},

			[3] = {
				-- left plane, faces right
				type     = "plane",
				normal   = {1.0 * 100, 0.0, 0.0},
				distance = -20,
				material = "mattRed",
			},

			[4] = {
				-- right plane, faces left
				type     = "plane",
				normal   = {-1.0 * 100, 0.0, 0.0},
				distance = -20,
				material = "mattGreen",
			},

			[5] = {
				-- rear plane, faces toward camera
				type     = "plane",
				normal   = {0.0, 0.0, 1.0 * 100},
				distance = -20,
				material = "mattWhite",
			},

			[6] = {
				-- front plane, located behind camera
				type     = "plane",
				normal   = {0.0, 0.0, -1.0 * 100},
				distance = 20,
				material = "mattWhite",
			},
		},



		materials = {
			[1] = {
				type                   = "mattBlue",
				refractionIndex        = 0.0 * 100.0,
				specularExponent       = 3.0 * 100.0,

				-- PHOTON MAPPING
				diffuseReflectiveness  = {0.1 * 100.0, 0.1 * 100.0, 0.8 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
			},

			[2] = {
				type                   = "mattGreen",
				refractionIndex        = 0.0 * 100.0,
				specularExponent       = 3.0 * 100.0,

				-- PHOTON MAPPING
				diffuseReflectiveness  = {0.1 * 100.0, 0.8 * 100.0, 0.1 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
			},

			[3] = {
				type                   = "mattRed",
				refractionIndex        = 0.0 * 100.0,
				specularExponent       = 3.0 * 100.0,

				diffuseReflectiveness  = {0.8 * 100.0, 0.1 * 100.0, 0.1 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
			},

			[4] = {
				type                   = "mattWhite",
				refractionIndex        = 0, --1.0 * 100.0;
				specularExponent       = 12 * 100.0,

				diffuseReflectiveness  = {0.9 * 100.0, 0.9 * 100.0, 0.9 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
			},

			[5] = {
				type                   = "mattBlack",
				refractionIndex        = 0, --1.0 * 100.0;
				specularExponent       = 0 * 100.0,

				diffuseReflectiveness  = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
			},


			[6] = {
				type                   = "glassWhite",
				refractionIndex        = 0, --1.0 * 100.0;
				specularExponent       = 12.0 * 100.0,

				diffuseReflectiveness  = {0.4 * 100.0, 0.4 * 100.0, 0.4 * 100.0},
				specularReflectiveness = {0.6 * 100.0, 0.6 * 100.0, 0.6 * 100.0},
				specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
			},

			-- produces caustic
			[7] = {
				type                  = "glassRefract",
				refractionIndex       = 1.44 * 100.0,
				beerCoefficient       = 20.0 * 100.0,
				specularExponent      = 12.0 * 100.0,

				diffuseReflectiveness  = {0.00 * 100.0, 0.00 * 100.0, 0.00 * 100.0},
				specularReflectiveness = {0.01 * 100.0, 0.01 * 100.0, 0.01 * 100.0},
				specularRefractiveness = {0.80 * 100.0, 0.80 * 100.0, 0.80 * 100.0},
			},

			--[[
			-- dull gray
			[7] = {
				type                  = "glassRefract",
				refractionIndex       = 1.44 * 100.0,
				beerCoefficient       = 20.0 * 100.0,
				specularExponent      = 12.0 * 100.0,

				diffuseReflectiveness  = {1.0 * 100.0, 1.0 * 100.0, 1.0 * 100.0},
				specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},
				specularRefractiveness = {0.8 * 100.0, 0.8 * 100.0, 0.8 * 100.0},
			},
			--]]
		},
	},
}
