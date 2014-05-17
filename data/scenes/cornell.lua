
root = {
	raytracer = {
		numRayThreads = 4,
		maxRayDepth = 1,
		antiAliasing = 0,
		incrementalRender = 1,

		-- PHOTON MAPPING PARAMETERS
		maxPhotonDepth = 3,
		numPhotons = 100000,
		photonSearchCount = 300,
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


	camera = {
		pos    = { 0.0,  0.0, 85.0},
		vrp    = { 0.0,  0.0, -1.0},
		vplane = {16,   12,    0},
--		vfov   = 90.0,

		renderDOF    =   0,
		fplaneDist   = 150.0,
		lensAperture =   2,
	},


	-- NOTE: all RGB color triplets must be specified in
	-- the range [0.0, 255.0], not [0.0, 1.0] (raytracer
	-- works with normalized values internally, and only
	-- converts to [0, 255] for SDL output)
	lights = {
		[1] = {
			type      = "point",
			subtype   = "positional",
			position  = {  0.0,   15.0,    10.0},
			color     = {200.0,  200.0,  200.0},
			fov       = 360.0,
			radius    = 10.0,
		},

	},



	objects = {
		[1] = {
			-- bottom plane (floor)
			type     = "plane",
			normal   = {0.0, 1.0 * 100, 0.0},
			distance = -25,
			material = "mattWhite",
		},
		[2] = {
			-- top plane (ceiling)
			type     = "plane",
			normal   = {0.0, -1.0 * 100, 0.0},
			distance = -25,
			material = "mattWhite",
		},

		[3] = {
			-- left plane, faces right
			type     = "plane",
			normal   = {1.0 * 100, 0.0, 0.0},
			distance = -25,
			material = "mattGreen",
		},
		[4] = {
			-- right plane, faces left
			type     = "plane",
			normal   = {-1.0 * 100, 0.0, 0.0},
			distance = -25,
			material = "mattRed",
		},

		[5] = {
			-- rear plane, faces toward camera
			type     = "plane",
			normal   = {0.0, 0.0, 1.0 * 100},
			distance = -1,
			material = "mattWhite",
		},
		[6] = {
			-- back plane (to close the scene)
			type     = "plane",
			normal   = {0.0, 0.0, -1.0 * 100},
			distance = 25,
			material = "mattWhite",
		},

		[7] = {
			type     = "ellipse",
			position = { -8, -15, 5},
			size     = {10.0, 10.0, 10.0},
			material = "glassGray2",
		},

	},




	materials = {
		[1] = {
			type                  = "mattBlue",
			texture               = "glass.png",
			reflectionCoefficient = 0.0 * 100.0,
			refractionCoefficient = 0.000 * 100.0,
			refractionIndex       = 0.000 * 100.0,
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {  0.0,   0.0, 255.0},
			specularColor         = {255.0, 255.0, 255.0},
			specularExponent      = 3.0 * 100.0,
		},
		[2] = {
			type                  = "mattGreen",
			texture               = "glass.png",
			reflectionCoefficient = 0.0 * 100.0,
			refractionCoefficient = 0.000 * 100.0,
			refractionIndex       = 0.000 * 100.0,
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {  0.0, 255.0,   0.0},
			specularColor         = {255.0, 255.0, 255.0},
			specularExponent      = 3.0 * 100.0,
		},

		[3] = {
			type                  = "mattRed",
			texture               = "glass.png",
			reflectionCoefficient = 0.0 * 100.0,
			refractionCoefficient = 0.000 * 100.0,
			refractionIndex       = 0.000 * 100.0,
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {255.0, 0.0, 0.0},
			specularColor         = {200.0, 100.0, 155.0},
			specularExponent      = 3.0 * 100.0,
		},

		[4] = {
			type                  = "glassGray1",
			texture               = "glass.png",
			reflectionCoefficient = 0.8 * 100.0,
			refractionCoefficient = 0, --1.0 * 100.0,
			refractionIndex       = 0, --1.0 * 100.0;
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {50.0, 50.0, 50.0},
			specularColor         = {255.0, 255.0, 255.0},
			specularExponent      = 25.0 * 100.0,
		},

		[5] = {
			type                  = "mattWhite",
			texture               = "glass.png",
			reflectionCoefficient = 0.0 * 100.0,
			refractionCoefficient = 0, --1.0 * 100.0,
			refractionIndex       = 0, --1.0 * 100.0;
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {180.0, 150.0, 180.0},
			specularColor         = {255.0, 255.0, 255.0},
			specularExponent      = 12 * 100.0,
		},

		[6] = {
			type                  = "glassGray2",
			texture               = "glass.png",
			reflectionCoefficient = 0.8 * 100.0,
			refractionCoefficient = 0, --1.0 * 100.0,
			refractionIndex       = 0, --1.0 * 100.0;
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {200.0, 250.0, 200.0},
			specularColor         = {255.0, 255.0, 255.0},
			specularExponent      = 25.0 * 100.0,
		},

		[7] = {
			type                  = "glassWhite",
			texture               = "glass.png",
			reflectionCoefficient = 0.5 * 100.0,
			refractionCoefficient = 0, --1.0 * 100.0,
			refractionIndex       = 0, --1.0 * 100.0;
			opacityCoefficient    = 1.0 * 100.0,
			diffuseColor          = {180.0, 150.0, 180.0},
			specularColor         = {255.0, 255.0, 255.0},
			specularExponent      = 12 * 100.0,
		},
	},
}
