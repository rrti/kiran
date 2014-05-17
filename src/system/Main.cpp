#include <cstdlib>
#include <cstdio>

#include "./SDLWindow.hpp"
#include "./LuaParser.hpp"
#include "../renderer/Camera.hpp"
#include "../renderer/Scene.hpp"
#include "../renderer/RayTracer.hpp"

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("[%s] usage: %s <scene.lua>\n", __FUNCTION__, argv[0]);
		return -1;
	}

	std::string sceneFile(argv[1]);
	std::string sceneDump(argv[1]);

	LuaParser parser;

	if (!parser.Execute(sceneFile, "root")) {
		printf("[%s] error parsing %s: %s\n", __FUNCTION__, sceneFile.c_str(), (parser.GetError()).c_str());
		return -1;
	}

	SDLWindow window(parser);
	Camera camera(parser, window);
	Scene scene(parser, &camera);
	RayTracer tracer(parser, scene);

	srandom(time(NULL));
	scene.SetNumThreads(tracer.GetNumThreads());
	sceneDump.replace(sceneDump.find(".lua"), sceneDump.size(), ".ppm");

	camera.Update();
	tracer.Render(window, scene);
	window.Show();
	window.ScreenDump(sceneDump.c_str());

	return 0;
}
