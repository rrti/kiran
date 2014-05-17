#ifndef KIRAN_SDL_WINDOW_HDR
#define KIRAN_SDL_WINDOW_HDR

#include <vector>

#include "../math/vec3fwd.hpp"
#include "../math/vec3.hpp"

namespace boost {
	class mutex;
}

struct LuaParser;
struct SDL_Surface;
struct SDLWindow {
public:
	SDLWindow(LuaParser&);
	~SDLWindow();

	bool InitSDL(unsigned int, unsigned int, const char*);
	void SwapBuffers(unsigned int);
	void NormalizeBuffers();

	void DrawRect(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
	void SetPixel(unsigned int, unsigned int, const math::vec3f&);
	void SetPixel(unsigned int, unsigned int, unsigned int);
	unsigned int GetPixel(unsigned int, unsigned int);

	unsigned int GetSizeX() const;
	unsigned int GetSizeY() const;
	unsigned int GetHSizeX() const;
	unsigned int GetHSizeY() const;

	void Show();
	void ScreenDump(const char*);

private:
	static bool initialized;
	static boost::mutex swapBufferMutex;
	static boost::mutex setPixelMutex;

	SDL_Surface* surface;

	unsigned int sw, sh;

	std::vector<math::vec3f> pixels;

	math::vec3f maxPixel;
	math::vec3f sumPixel;
	math::vec3f avgPixel;

	bool autoShow;
	bool keepOpen;
	bool makeDump;
};

#endif
