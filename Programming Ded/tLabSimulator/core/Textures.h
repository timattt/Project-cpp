#ifndef T_TEXTURES
#define T_TEXTURES

#include <dirent.h>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;
using namespace std;

Texture BEAKER_LIQ[100];
Texture BEAKER_CRASHED;
Texture BACKGROUND;
Texture RULER;
Texture GLASS_LIQ[100];
Texture DROP;
Texture PUDDLE;
Texture RAG;
Texture GLASS_BROKEN;

void loadTexturePack(const char *dir_path, Texture *text) {
	DIR *dir;
	dirent *ent;
	if ((dir = opendir(dir_path)) != NULL) {
		for (int i = 0; (ent = readdir(dir)) != NULL;) {
			if (((string) ent->d_name).find(".jpg") != std::string::npos
					|| ((string) ent->d_name).find(".png")
							!= std::string::npos) {
				(text+ i)->loadFromFile(
						(string) (dir_path) + (string) (ent->d_name));
				i++;
			}
		}
		closedir(dir);
	}
}

void loadTextures() {
	cout << "Loading textures:\n";
	cout.flush();
	BEAKER_CRASHED.loadFromFile("Resources/beaker/CRASHED.jpg");
	loadTexturePack("Resources/beaker/LIQ/", BEAKER_LIQ);
	loadTexturePack("Resources/glass/LIQ/", GLASS_LIQ);
	BACKGROUND.loadFromFile("Resources/background.jpg");
	RULER.loadFromFile("Resources/RULER.jpg");
	DROP.loadFromFile("Resources/DROP.png");
	PUDDLE.loadFromFile("Resources/Puddle.png");
	RAG.loadFromFile("Resources/RAG.png");
	GLASS_BROKEN.loadFromFile("Resources/glass/GLASS_BROKEN.jpg");
	cout << "Loading finished!\n";
	cout.flush();
}

#endif
