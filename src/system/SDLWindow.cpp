#include <SDL/SDL.h>
#include <boost/thread/mutex.hpp>

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#include "./Defines.hpp"

#include "./SDLWindow.hpp"
#include "./LuaParser.hpp"
#include "../math/vec3.hpp"

boost::mutex SDLWindow::swapBufferMutex;
boost::mutex SDLWindow::setPixelMutex;

bool SDLWindow::initialized = false;

SDLWindow::SDLWindow(LuaParser& parser): surface(NULL) {
	const LuaTable* rootTable = parser.GetRootTbl();
	const LuaTable* windowTable = rootTable->GetTblVal("window");

	const int wx = int(windowTable->GetFltVal("xsize", 640.0f));
	const int wy = int(windowTable->GetFltVal("ysize", 480.0f));
	const std::string& wt = windowTable->GetStrVal("title", "Kiran");

	autoShow = bool(windowTable->GetFltVal("autoShow", 1.0f));
	keepOpen = bool(windowTable->GetFltVal("keepOpen", 1.0f));
	makeDump = bool(windowTable->GetFltVal("makeDump", 1.0f));

	std::cout << "[SDLWindow::SDLWindow]" << std::endl;
	std::cout << "\tWHITE_BALANCE_RANGE_LIMITING: " << WHITE_BALANCE_RANGE_LIMITING  << std::endl;
	std::cout << "\tWHITE_BALANCE_RANGE_R(100):   " << WHITE_BALANCE_RANGE_R(100.0f) << std::endl;
	std::cout << "\tWHITE_BALANCE_RANGE_G(100):   " << WHITE_BALANCE_RANGE_G(100.0f) << std::endl;
	std::cout << "\tWHITE_BALANCE_RANGE_B(100):   " << WHITE_BALANCE_RANGE_B(100.0f) << std::endl;

	// set the window dimensions to an uneven number of pixels
	InitSDL(wx + ((wx & 1)? 0: 1), wy + ((wy & 1)? 0: 1), wt.c_str());
}

SDLWindow::~SDLWindow() {
	if (initialized) {
		pixels.clear();

		SDL_Quit();
	}
}

bool SDLWindow::InitSDL(unsigned int w, unsigned int h, const char* t) {
	if (!initialized) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
			std::cout << "[SDLWindow::InitSDL] initialization error " << SDL_GetError() << std::endl;
			return false;
		}

		SDL_WM_SetIcon(NULL, NULL);
		SDL_WM_SetCaption(t, NULL);

		if (autoShow) {
			surface = SDL_SetVideoMode(w, h, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
		} else {
			surface = NULL;
		}

		sw = w;
		sh = h;
		initialized = (surface != NULL);

		pixels.resize(sw * sh);
	}

	return initialized;
}

void SDLWindow::SwapBuffers(unsigned int numThreads) {
	if (surface != NULL) {
		if (numThreads > 1) {
			boost::mutex::scoped_lock lock(swapBufferMutex);
			SDL_Flip(surface);
		} else {
			SDL_Flip(surface);
		}
	}
}

void SDLWindow::NormalizeBuffers() {
	std::cout << "[SDLWindow::NormalizeBuffers]" << std::endl;
	std::cout << "\tmaxPixel:            " << maxPixel.str() << std::endl;
	std::cout << "\tsumPixel:            " << sumPixel.str() << std::endl;
	std::cout << "\tavgPixel:            " << avgPixel.str() << std::endl;
	std::cout << "\tmaxPixel / avgPixel: " << (maxPixel / avgPixel).str() << std::endl;

	if (maxPixel.sqLen3D() > 0.0f) {
		#if (WHITE_BALANCE_RANGE_LIMITING == 1)
		// limit the max. intensity to <WHITE_BALANCE_RANGE> times the average
		maxPixel.x = std::min(maxPixel.x, WHITE_BALANCE_RANGE_R(avgPixel.x));
		maxPixel.y = std::min(maxPixel.y, WHITE_BALANCE_RANGE_G(avgPixel.y));
		maxPixel.z = std::min(maxPixel.z, WHITE_BALANCE_RANGE_B(avgPixel.z));
		#endif

		// take the maximum over all channels
		const float maxValue = std::max(maxPixel.x, std::max(maxPixel.y, maxPixel.z));

		for (unsigned int x = 0; x < sw; x++) {
			for (unsigned int y = 0; y < sh; y++) {
				#if (WHITE_BALANCE_RANGE_LIMITING == 1)
				pixels[y * sw + x].x = std::min(pixels[y * sw + x].x / maxValue, 1.0f);
				pixels[y * sw + x].y = std::min(pixels[y * sw + x].y / maxValue, 1.0f);
				pixels[y * sw + x].z = std::min(pixels[y * sw + x].z / maxValue, 1.0f);

				SetPixel(x, y, (pixels[y * sw + x]));
				#else
				SetPixel(x, y, (pixels[y * sw + x] /= maxValue));
				#endif
			}
		}
	}
}

void SDLWindow::Show() {
	if (surface == NULL) {
		return;
	}

	SwapBuffers(1);

	while (keepOpen) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: {
					keepOpen = false;
				} break;
			}
		}
	}
}

void SDLWindow::DrawRect(unsigned int rx, unsigned int ry, unsigned int rw, unsigned int rh, Uint32 c) {
	for (unsigned int i = rx; i < rx + rw; i++) { SetPixel(i,       ry,      c); }
	for (unsigned int i = rx; i < rx + rw; i++) { SetPixel(i,       ry + rh, c); }

	for (unsigned int i = ry; i < ry + rh; i++) { SetPixel(rx,      i,       c); }
	for (unsigned int i = ry; i < ry + rh; i++) { SetPixel(rx + rw, i,       c); }
}


void SDLWindow::SetPixel(unsigned int x, unsigned int y, const math::vec3f& c) {
	{
		// limited scope, threads only write their own pixels
		boost::mutex::scoped_lock lock(setPixelMutex);

		maxPixel.x  = std::max(maxPixel.x, c.x);
		maxPixel.y  = std::max(maxPixel.y, c.y);
		maxPixel.z  = std::max(maxPixel.z, c.z);
		sumPixel   += c;
		avgPixel    = sumPixel / (sw * sh);
	}

	#if (DEBUG_ASSERTS_SDLWINDOW == 1)
	assert(maxPixel.x != M_INF() && maxPixel.x != M_NAN());
	assert(maxPixel.y != M_INF() && maxPixel.y != M_NAN());
	assert(maxPixel.z != M_INF() && maxPixel.z != M_NAN());
	#endif

	pixels[y * sw + x] = c;

	// note: <c> is not necessarily normalized at this point
	const Uint8 r = pixels[y * sw + x].x * 255.0f;
	const Uint8 g = pixels[y * sw + x].y * 255.0f;
	const Uint8 b = pixels[y * sw + x].z * 255.0f;

	if (surface != NULL) {
		SetPixel(x, y, SDL_MapRGB(surface->format, r, g, b));
	}
}

void SDLWindow::SetPixel(unsigned int x, unsigned int y, unsigned int c) {
	if (x >= sw || y >= sh) {
		return;
	}

	Uint8* p =
		(Uint8*) surface->pixels +
		(y * surface->pitch) +
		(x * surface->format->BytesPerPixel);
	// Uint32* p = (Uint32*) surface->pixels + y + x;
	// *p = c;

	switch (surface->format->BytesPerPixel) {
		case 1: {
			(*(Uint8*) p) = c;
		} break;

		case 2: {
			(*(Uint16*) p) = c;
		} break;

		case 3: {
			p = (Uint8*) p;

			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (c >> 16) & 0xFF;
				p[1] = (c >>  8) & 0xFF;
				p[2] = (c >>  0) & 0xFF;
			} else {
				p[0] = (c >>  0) & 0xFF;
				p[1] = (c >>  8) & 0xFF;
				p[2] = (c >> 16) & 0xFF;
			}
		} break;

		case 4: {
			(*(Uint32*) p) = c;
		} break;
	}
}

unsigned int SDLWindow::GetPixel(unsigned int x, unsigned int y) {
	if (surface != NULL) {
		Uint8* p =
			(Uint8*) surface->pixels +
			(y * surface->pitch) +
			(x * surface->format->BytesPerPixel);

		switch (surface->format->BytesPerPixel) {
			case 1: {
				return (*(Uint8*) p);
			} break;

			case 2: {
				return (*(Uint16*) p);
			} break;

			case 3: {
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					return ((p[0] << 16) | (p[1] << 8) | (p[2] <<  0));
				} else {
					return ((p[0] <<  0) | (p[1] << 8) | (p[2] << 16));
				}
			} break;

			case 4: {
				return (*(Uint32*) p);
			} break;

			default: {
				return 0;
			} break;
		}
	} else {
		// note: pixels must be normalized before calling this
		// note: SDL's internal pixel format is ARGB, not RGBA
		return ((int(pixels[y * sw + x].x * 255) << 16) | (int(pixels[y * sw + x].y * 255) << 8) | (int(pixels[y * sw + x].z * 255) << 0));
	}
}



unsigned int SDLWindow::GetSizeX() const { return (sw); }
unsigned int SDLWindow::GetSizeY() const { return (sh); }
unsigned int SDLWindow::GetHSizeX() const { return (sw >> 1); }
unsigned int SDLWindow::GetHSizeY() const { return (sh >> 1); }

void SDLWindow::ScreenDump(const char* fname) {
	if (!makeDump) {
		return;
	}

	std::ofstream f(fname, std::ios::out);
	std::stringstream ss;

	ss << "P3" << std::endl;
	ss << "## comment" << std::endl;
	ss << sw << " " << sh << std::endl;
	ss << "255" << std::endl;

	// output in row-major order (all
	// columns in row 0, then all in
	// row 1, etc)
	const unsigned int size = sw * sh;

	for (unsigned int i = 0; i < size; i++) {
		const unsigned int pxl = GetPixel((i % sw), (i / sw));

		Uint8 r = (pxl >> 16) & 0xFF;
		Uint8 g = (pxl >>  8) & 0xFF;
		Uint8 b = (pxl >>  0) & 0xFF;

		if (surface != NULL) {
			SDL_GetRGB(pxl, surface->format, &r, &g, &b);
		}

		ss << int(r) << " " << int(g) << " " << int(b) << std::endl;
	}

	f << ss.str();
	f.flush();
	f.close();
}
