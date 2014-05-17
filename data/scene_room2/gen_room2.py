import commands
import math
import random

def GenerateScene():

	l = 15
	k = 0.0*math.pi

	for n in range(201, 202):	
		k = k + 0.05
		if k > 2.0*math.pi:
			break
		print 'Processing image '+str(n)
		f = open('scene'+str(n)+'.lua','w')
		f.write('	root = {\n'+
'		raytracer = {\n'+
'			numThreads = 4,\n'+
'			maxRayDepth = 4,\n'+
'			antiAliasing = 1,\n'+
'			incrementalRender = 1,\n'+
'	\n'+
'			-- PHOTON MAPPING PARAMETERS\n'+
'			maxPhotonDepth = 4,\n'+
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
'			keepOpen = 1,\n'+
'			makeDump = 1,\n'+
'		},\n'+
'	\n'+
'	\n'+
'		scene = {\n'+
'			minBounds = {-25.0, -25.0, -25.0},\n'+
'			maxBounds = { 25.0,  25.0,  25.0},\n'+
'	\n'+
'			camera = {\n'+
'				pos    = { 30.0,  10.0, 30},\n'+
'				vrp    = { 0.0,  0.0, 0.0},\n'+
'				vplane = {16,   12,    0},\n'+
'		--		vfov   = 90.0,\n'+
'	\n'+
'				renderDOF    =   0,\n'+
'				fplaneDist   = 150.0,\n'+
'				lensAperture =   2,\n'+
'			},\n'+
'	\n'+
'			lights = {\n'+
'				[1] = {\n'+
'					type       = "positional",\n'+
'					position   = {0, 18, 0},\n'+
'					power      = {1000.0, 1000.0, 1000.0},\n'+
'					numPhotons = 0,\n'+
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
'					type     = "plane",\n'+
'					normal   = {0.0, -1.0 * 100, 0.0},\n'+
'					distance = -20,\n'+
'					material = "mattWhite",\n'+
'				},\n'+
'	\n'+
'				[3] = {\n'+
'					-- left plane, faces right\n'+
'					type     = "plane",\n'+
'					normal   = {1.0 * 100, 0.0, 0.0},\n'+
'					distance = -20,\n'+
'					material = "Goldenrod",\n'+
'				},\n'+
'	\n'+
'				[4] = {\n'+
'					-- right plane, faces left\n'+
'					type     = "plane",\n'+
'					normal   = {-1.0 * 100, 0.0, 0.0},\n'+
'					distance = -20,\n'+
'					material = "Wheat",\n'+
'				},\n'+
'	\n'+
'				[5] = {\n'+
'					-- rear plane, faces toward camera\n'+
'					type     = "plane",\n'+
'					normal   = {0.0, 0.0, 1.0 * 100},\n'+
'					distance = -20,\n'+
'					material = "Gold",\n'+
'				},\n'+
'	\n'+
'				[6] = {\n'+
'					-- front plane, located behind camera\n'+
'					type     = "plane",\n'+
'					normal   = {0.0, 0.0, -1.0 * 100},\n'+
'					distance = -20,\n'+
'					material = "Gold",\n'+
'				},\n'+
'	\n'+
'				[7] = {\n'+
'					-- left sphere\n'+
'					type     = "ellipse",\n'+
'					position = { -5.0, -10.0, 5.0},\n'+
'					size     = {5.0, 5.0, 5.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'	\n'+
'				[8] = {\n'+
'					-- right sphere\n'+
'					type     = "ellipse",\n'+
'					position = { 5.0, -10.0, -5.0},\n'+
'					size     = {5.0, 5.0, 5.0},\n'+
'					material = "glassRefract",\n'+
'				},\n'+
'	\n'+
'				[9] = {\n'+
'					type     = "box",\n'+
'					position = { 0.0, -15.0, 0},\n'+
'					size     = {30.0, 1.0, 30.0},\n'+
'					material = "glassWhite2",\n'+
'				},\n'+
'	\n'+
'				[10] = {\n'+
'					type     = "box",\n'+
'					position = { 0.0, 0.0, -19},\n'+
'					size     = {30.0, 30.0, 1.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'				[11] = {\n'+
'					type     = "box",\n'+
'					position = {-19.0, 0.0, 0.0},\n'+
'					size     = {1.0, 30.0, 30.0},\n'+
'					material = "glassWhite",\n'+
'				},\n'+
'			},\n'+
'	\n'+
'	\n'+
'	\n'+
'			materials = {\n'+
'				[1] = {\n'+
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
'				[2] = {\n'+
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
'				[3] = {\n'+
'					type                   = "Gold",\n'+
'					refractionIndex        = 0.0 * 100.0,\n'+
'					specularExponent       = 3.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {1.0 * 100.0, 0.84 * 100.0, 0.0 * 100.0},\n'+
'					specularReflectiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+
'	\n'+
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
'					beerCoefficient       = 20.0 * 100.0,\n'+
'					specularExponent      = 12.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.00 * 100.0, 0.00 * 100.0, 0.00 * 100.0},\n'+
'					specularReflectiveness = {0.10 * 100.0, 0.10 * 100.0, 0.10 * 100.0},\n'+
'					specularRefractiveness = {0.90 * 100.0, 0.90 * 100.0, 0.90 * 100.0},\n'+
'				},\n'+
'	\n'+
'				[8] = {\n'+
'					type                   = "glassWhite2",\n'+
'					refractionIndex        = 1.0 * 100.0;\n'+
'					specularExponent       = 12.0 * 100.0,\n'+
'	\n'+
'					diffuseReflectiveness  = {0.8 * 100.0, 0.8 * 100.0, 0.8 * 100.0},\n'+
'					specularReflectiveness = {0.2 * 100.0, 0.2 * 100.0, 0.2 * 100.0},\n'+
'					specularRefractiveness = {0.0 * 100.0, 0.0 * 100.0, 0.0 * 100.0},\n'+
'				},\n'+

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
