import commands
import math
import random

def GenerateScene():

	l = 15
	k = -35

	for n in range(201, 202):	
		k = k + 0.2
		if k > 35:
			break
		print 'Processing image '+str(n)
		f = open('scene'+str(n)+'.lua','w')
		f.write('	root = {\n'+
'		raytracer = {\n'+
'			numThreads = 4,\n'+
'			maxRayDepth = 4,\n'+
'			antiAliasing = 0,\n'+
'			incrementalRender = 1,\n'+
'	\n'+
'			-- PHOTON MAPPING PARAMETERS\n'+
'			maxPhotonDepth = 10,\n'+
'			photonSearchCount = 2000,\n'+
'			photonSearchRadius = 7.0,\n'+
'		},\n'+
'	\n'+
'		window = {\n'+
'			xsize = 1280,\n'+
'			ysize = 800,\n'+
'			title = "Kiran",\n'+
'	\n'+
'			autoShow = 1,\n'+
'			keepOpen = 0,\n'+
'			makeDump = 1,\n'+
'		},\n'+
'	\n'+
'	\n'+
'		scene = {\n'+
'			minBounds = {-85.0, -85.0, -85.0},\n'+
'			maxBounds = { 85.0,  85.0,  85.0},\n'+
'	\n'+
'			camera = {\n'+
#neat effect taken from inside glass!
'--				pos    = { -20.0,  0.0, 30},\n'+
'				pos    = { -10.0,  0.0, 32},\n'+
'				vrp    = { 0.0,  0.0, 80.0},\n'+
'				vplane = {16,   12,    0},\n'+
'				vfov   = 110.0,\n'+
'	\n'+
'				renderDOF    =   0,\n'+
'				fplaneDist   = 150.0,\n'+
'				lensAperture =   2,\n'+
'			},\n'+
'	\n'+
'			lights = {\n'+
'--[[				[1] = {\n'+
'					type       = "positional",\n'+
'					position   = {0, 10, 50},\n'+
'					power      = {1000.0, 1000.0, 1000.0},\n'+
'					numPhotons = 0,\n'+
'					fov        = 360.0,\n'+
'					radius     = 0.0,\n'+
'				},\n'+
'				[2] = {\n'+
'					type       = "positional",\n'+
'					position   = {-32, 19, 2},\n'+
'					power      = {1000.0, 1000.0, 1000.0},\n'+
'--					numPhotons = 3000000,\n'+
'					numPhotons = 0,\n'+
'					fov        = 360.0,\n'+
'					radius     = 0.0,\n'+
'				},\n'+
'--]]				[3] = {\n'+
'					type       = "positional",\n'+
'					position   = {'+str(k)+', 19, 2},\n'+
'					power      = {1000.0, 1000.0, 1000.0},\n'+
'					numPhotons = 2000000,\n'+
'					--numPhotons = 0,\n'+
'					fov        = 360.0,\n'+
'					radius     = 0.0,\n'+
'				},\n'+
'			},\n'+
'	\n'+
'	\n'+
'	\n'+
'			objects = {\n'+
'				[1] = {\n'+
'					-- bottom plane (floor)\n'+
'					type     = "plane",\n'+
'					normal   = {0.0, 1.0 * 100, 0.0},\n'+
'					distance = -20,\n'+
'					material = "Wheat",\n'+
'				},\n'+
'	\n'+
'				[2] = {\n'+
'					-- top plane (ceiling)\n'+
'					type     = "box",\n'+
'					position = {0.0, 20.0, 0.0},\n'+
'					size     = {50.0, 1.0, 90.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+
'	\n'+
'				[3] = {\n'+
'					-- left wall\n'+
'					type     = "box",\n'+
'					position = {-25.0, -5.0, -10.0},\n'+
'					size     = {1.0, 40.0, 100.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+

'	\n'+
'				[4] = {\n'+
'					-- right wall\n'+
'					type     = "box",\n'+
'					position = {25.0, -5.0, -10.0},\n'+
'					size     = {1.0, 40.0, 100.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+
'	\n'+
#bank1
'				[200] = {\n'+
'					type     = "box",\n'+
'					position = {15.0, -15.0, 0.0},\n'+
'					size     = {10.0, 1.0, 15.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
'				[201] = {\n'+
'					type     = "box",\n'+
'					position = {18.0, -12.0, 0.0},\n'+
'					size     = {4.0, 4.0, 15.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
'				[202] = {\n'+
'					type     = "box",\n'+
'					position = {16.0, -15.0, 7.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
'				[203] = {\n'+
'					type     = "box",\n'+
'					position = {16.0, -15.0, -7.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
#bank2
'				[300] = {\n'+
'					type     = "box",\n'+
'					position = {-15.0, -15.0, 0.0},\n'+
'					size     = {10.0, 1.0, 15.0},\n'+
'					material = "mattBlue",\n'+
'				},\n'+
'				[301] = {\n'+
'					type     = "box",\n'+
'					position = {-18.0, -12.0, 0.0},\n'+
'					size     = {4.0, 4.0, 15.0},\n'+
'					material = "mattBlue",\n'+
'				},\n'+
'				[302] = {\n'+
'					type     = "box",\n'+
'					position = {-16.0, -15.0, 7.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattBlue",\n'+
'				},\n'+
'				[303] = {\n'+
'					type     = "box",\n'+
'					position = {-16.0, -15.0, -7.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattBlue",\n'+
'				},\n'+

#stripes left wall
'				[205] = {\n'+
'					type     = "box",\n'+
'					position = {-24.0, 0.0, 0.0},\n'+
'					size     = {3.0, 50.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[206] = {\n'+
'					type     = "box",\n'+
'					position = {-24.0, 0.0, -12.0},\n'+
'					size     = {3.0, 50.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[207] = {\n'+
'					type     = "box",\n'+
'					position = {-24.0, 0.0, 12.0},\n'+
'					size     = {3.0, 50.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
#mirror
'				[204] = {\n'+
'					type     = "box",\n'+
'					position = {-24.0, 0.0, 0.0},\n'+
'					size     = {1.0, 25.0, 35.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
#table
'				[208] = {\n'+
'					type     = "box",\n'+
'					position = {0.0, -12.0, 0.0},\n'+
'					size     = {10.0, 1.0, 20.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+
'				[209] = {\n'+
'					type     = "cylinder",\n'+
'					position = {0.0, -16.0, 4.0},\n'+
'					size     = {6.0, 1.5, 1.5},\n'+
'					axis     = 1,\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[210] = {\n'+
'					type     = "cylinder",\n'+
'					position = {0.0, -16.0, -4.0},\n'+
'					size     = {6.0, 1.5, 1.5},\n'+
'					axis     = 1,\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[211] = {\n'+
'					-- right sphere\n'+
'					type     = "ellipse",\n'+
'					position = { 0.0, -8.5, 0.0},\n'+
'					size     = {3.0, 3.0, 3.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[212] = {\n'+
'					type     = "box",\n'+
'					position = {0.0, -16.0, 0.0},\n'+
'					size     = {8.0, 1.0, 18.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+
#facing wall-entrance
'				[213] = {\n'+
'					-- rear plane, faces toward camera\n'+
'					type     = "box",\n'+
'					position = {-20.0, 0.0, -40.0},\n'+
'					size     = {20.0, 40.0, 40.0},\n'+
'					material = "Wheat",\n'+
'				},\n'+
'				[214] = {\n'+
'					-- rear plane, faces toward camera\n'+
'					type     = "box",\n'+
'					position = {20.0, 0.0, -40.0},\n'+
'					size     = {20.0, 40.0, 40.0},\n'+
'					material = "Wheat",\n'+
'				},\n'+
'				[215] = {\n'+
'					-- rear plane, faces toward camera\n'+
'					type     = "box",\n'+
'					position = {0.0, 20.0, -40.0},\n'+
'					size     = {50.0, 20.0, 40.0},\n'+
'					material = "Wheat",\n'+
'				},\n'+
#mirror2&3
'				[216] = {\n'+
'					-- rear plane, faces toward camera\n'+
'					type     = "box",\n'+
'					position = {15.0, -2.0, -20.0},\n'+
'					size     = {5.0, 20.0, 1.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[217] = {\n'+
'					-- rear plane, faces toward camera\n'+
'					type     = "box",\n'+
'					position = {-15.0, -2.0, -20.0},\n'+
'					size     = {5.0, 20.0, 1.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
#stripes right wall
'				[405] = {\n'+
'					type     = "box",\n'+
'					position = {24.0, 0.0, 0.0},\n'+
'					size     = {3.0, 50.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[406] = {\n'+
'					type     = "box",\n'+
'					position = {24.0, 0.0, -12.0},\n'+
'					size     = {3.0, 50.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[407] = {\n'+
'					type     = "box",\n'+
'					position = {24.0, 0.0, 12.0},\n'+
'					size     = {3.0, 50.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
#mirror
'				[404] = {\n'+
'					type     = "box",\n'+
'					position = {24.0, 0.0, 0.0},\n'+
'					size     = {1.0, 25.0, 35.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[405] = {\n'+
'					-- leftglass\n'+
'					type     = "box",\n'+
'					position = {-20.0, -5.0, 30.0},\n'+
'					size     = {20.0, 40.0, 1.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+
'				[406] = {\n'+
'					-- rightglass\n'+
'					type     = "box",\n'+
'					position = {20.0, -5.0, 30.0},\n'+
'					size     = {20.0, 40.0, 1.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+
'				[407] = {\n'+
'					type     = "box",\n'+
'					position = {0.0, 12.0, 30.0},\n'+
'					size     = {50.0, 2.0, 2.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[408] = {\n'+
'					type     = "box",\n'+
'					position = {-20.0, 0.0, 40.0},\n'+
'					size     = {3.0, 50.0, 3.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[409] = {\n'+
'					type     = "box",\n'+
'					position = {20.0, 0.0, 40.0},\n'+
'					size     = {3.0, 50.0, 3.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[410] = {\n'+
'					type     = "box",\n'+
'					position = {-43.0, 0.0, 40.0},\n'+
'					size     = {3.0, 50.0, 3.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[411] = {\n'+
'					type     = "box",\n'+
'					position = {-43.0, 0.0, 80.0},\n'+
'					size     = {3.0, 50.0, 3.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[412] = {\n'+
'					type     = "box",\n'+
'					position = {-43.0, 10.0, 60.0},\n'+
'					size     = {3.0, 3.0, 45.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[413] = {\n'+
'					type     = "box",\n'+
'					position = {43.0, 0.0, 40.0},\n'+
'					size     = {3.0, 50.0, 3.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[414] = {\n'+
'					type     = "box",\n'+
'					position = {43.0, 0.0, 80.0},\n'+
'					size     = {3.0, 50.0, 3.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[415] = {\n'+
'					type     = "box",\n'+
'					position = {43.0, 10.0, 60.0},\n'+
'					size     = {3.0, 3.0, 45.0},\n'+
'					material = "Goldenrod",\n'+
'				},\n'+


#big room entrance
'				[900] = {\n'+
'					-- \n'+
'					type     = "box",\n'+
'					position = {40.0, 0.0, 40.0},\n'+
'					size     = {40.0, 40.0, 1.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+
'				[901] = {\n'+
'					-- \n'+
'					type     = "box",\n'+
'					position = {-40.0, 0.0, 40.0},\n'+
'					size     = {40.0, 40.0, 1.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+
'				[902] = {\n'+
'					-- rightwall\n'+
'					type     = "box",\n'+
'					position = {45.0, 0.0, 60.0},\n'+
'					size     = {1.0, 40.0, 60.0},\n'+
'					material = "Wheat",\n'+

'				},\n'+
'				[705] = {\n'+
'					-- rightwall mirror\n'+
'					type     = "box",\n'+
'					position = {44.0, 0.0, 60.0},\n'+
'					size     = {1.0, 40.0, 20.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[903] = {\n'+
'					-- backwall\n'+
'					type     = "box",\n'+
'					position = {0.0, 0.0, 80.0},\n'+
'					size     = {100.0, 40.0, 1.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+
'				[923] = {\n'+
'					-- backwall mirror\n'+
'					type     = "box",\n'+
'					position = {0.0, 0.0, 79.0},\n'+
'					size     = {50.0, 30.0, 1.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[904] = {\n'+
'					-- leftwall\n'+
'					type     = "box",\n'+
'					position = {-45.0, 0.0, 60.0},\n'+
'					size     = {1.0, 40.0, 60.0},\n'+
'					material = "Wheat",\n'+
'				},\n'+
'				[704] = {\n'+
'					-- leftwall mirror\n'+
'					type     = "box",\n'+
'					position = {-44.0, 0.0, 60.0},\n'+
'					size     = {1.0, 40.0, 20.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[905] = {\n'+
'					-- top plane (ceiling2)\n'+
'					type     = "box",\n'+
'					position = {0.0, 20.0, 50.0},\n'+
'					size     = {100.0, 1.0, 90.0},\n'+
'					material = "mattWhite",\n'+
'				},\n'+
#bank3
'				[500] = {\n'+
'					type     = "box",\n'+
'					position = {19.0, -15.0, 60.0},\n'+
'					size     = {10.0, 1.0, 15.0},\n'+
'					material = "mattGreen",\n'+
'				},\n'+
'				[501] = {\n'+
'					type     = "box",\n'+
'					position = {22.0, -12.0, 60.0},\n'+
'					size     = {4.0, 4.0, 15.0},\n'+
'					material = "mattGreen",\n'+
'				},\n'+
'				[502] = {\n'+
'					type     = "box",\n'+
'					position = {20.0, -15.0, 67.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattGreen",\n'+
'				},\n'+
'				[503] = {\n'+
'					type     = "box",\n'+
'					position = {20.0, -15.0, 53.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattGreen",\n'+
'				},\n'+
#bank4
'				[504] = {\n'+
'					type     = "box",\n'+
'					position = {-19.0, -15.0, 60.0},\n'+
'					size     = {10.0, 1.0, 15.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
'				[505] = {\n'+
'					type     = "box",\n'+
'					position = {-22.0, -12.0, 60.0},\n'+
'					size     = {4.0, 4.0, 15.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
'				[506] = {\n'+
'					type     = "box",\n'+
'					position = {-20.0, -15.0, 67.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
'				[507] = {\n'+
'					type     = "box",\n'+
'					position = {-20.0, -15.0, 53.0},\n'+
'					size     = {8.0, 10.0, 1.0},\n'+
'					material = "mattRed",\n'+
'				},\n'+
#table2
'				[508] = {\n'+
'					type     = "box",\n'+
'					position = {0.0, -12.0, 60.0},\n'+
'					size     = {10.0, 1.0, 20.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+
'				[509] = {\n'+
'					type     = "cylinder",\n'+
'					position = {0.0, -16.0, 64.0},\n'+
'					size     = {6.0, 1.5, 1.5},\n'+
'					axis     = 1,\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[510] = {\n'+
'					type     = "cylinder",\n'+
'					position = {0.0, -16.0, 56.0},\n'+
'					size     = {6.0, 1.5, 1.5},\n'+
'					axis     = 1,\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'				[511] = {\n'+
'					-- right sphere\n'+
'					type     = "ellipse",\n'+
'					position = { 0.0, -8.5, 60.0},\n'+
'					size     = {3.0, 3.0, 3.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[512] = {\n'+
'					type     = "box",\n'+
'					position = {0.0, -16.0, 60.0},\n'+
'					size     = {8.0, 1.0, 18.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+

'	\n'+
'	\n'+


'			},\n'+
'	\n'+
'	\n'+
'	\n'+
'			materials = {\n'+
'				[1] = {\n'+
'					type                   = "mattBlue",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					-- PHOTON MAPPING\n'+
'					diffuseReflectiveness  = {0.4 * 100.0, 0.4 * 100.0, 0.8 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[2] = {\n'+
'					type                   = "mattGreen",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					-- PHOTON MAPPING\n'+
'					diffuseReflectiveness  = {0.4 * 100.0, 0.8 * 100.0, 0.4 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[3] = {\n'+
'					type                   = "mattRed",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.8 * 100.0, 0.4 * 100.0, 0.4 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[4] = {\n'+
'					type                   = "mattWhite",\n'+
'					refractionIndex        = 0, --1.0 * 100.0;\n'+
'					specularExponent       = 12 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.9 * 100.0, 0.9 * 100.0, 0.9 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[5] = {\n'+
'					type                   = "mattBlack",\n'+
'					refractionIndex        = 0, --1.0 * 100.0;\n'+
'					specularExponent       = 0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'	\n'+
'				[6] = {\n'+
'					type                   = "glassWhite",\n'+
'					refractionIndex        = 1.0 * 100.0;\n'+
'					specularExponent       = 12.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.4 * 100.0, 0.4 * 100.0, 0.4 * 100.0},\n'+
'					specularReflectiveness = {0.6 * 100.0, 0.6 * 100.0, 0.6 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				-- produces caustic\n'+
'				[7] = {\n'+
'					type                  = "glassRefract",\n'+
'					refractionIndex       = 1.33 * 100.0,\n'+
'					beerCoefficient       = 1.0 * 100.0,\n'+
'					specularExponent      = 12.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.00 * 100.0, 0.00 * 100.0, 0.00 * 100.0},\n'+
'					specularReflectiveness = {0.10 * 100.0, 0.10 * 100.0, 0.10 * 100.0},\n'+
'					specularRefractiveness = {0.90 * 100.0, 0.90 * 100.0, 0.90 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[8] = {\n'+
'					type                   = "Goldenrod",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					-- PHOTON MAPPING\n'+
'					diffuseReflectiveness  = {0.85 * 100.0, 0.64 * 100.0, 0.12 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[9] = {\n'+
'					type                   = "Wheat",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					-- PHOTON MAPPING\n'+
'					diffuseReflectiveness  = {0.96 * 100.0, 0.87 * 100.0, 0.70 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[10] = {\n'+
'					type                   = "Gold",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularReflectiveness = {1.0 * 100.0, 0.84 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+

'			},\n'+
'		},\n'+
'	}\n'
		)
		f.close()

		commands.getstatusoutput('../../bin/kiran scene'+str(n)+'.lua')

	print 'Images done...'
	print 'Creating movie...'
	commands.getstatusoutput('for f in *ppm ; do convert -quality 100 $f `basename $f ppm`jpg; done ')
	commands.getstatusoutput('mencoder "mf://*.jpg" -mf fps=10 -o movie.avi -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=800 ')
	#commands.getstatusoutput('rm *.lua')
	commands.getstatusoutput('rm *.jpg')

GenerateScene()
