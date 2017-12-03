#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <string>
#include <fstream>
#include "manager.h"
#include <Windows.h>

#include "processing.h"
#include "resources.h"


namespace in
{
	model* loadModel(std::string nameFile);
	track* loadTrack(std::string nameFile);

	texture* loadTexture(std::string nameFile);
	sEffect* loadEffect(std::string nameFile);
}

#endif // !INPUT_H