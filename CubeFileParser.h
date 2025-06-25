#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <regex>

struct Settings {
	int size;
};

Settings parseSettings(std::ifstream& input);

std::vector<glm::vec3> parse(std::ifstream& file)
{
	std::vector<glm::vec3> cubeVerts;
	Settings set;
	set = parseSettings(file);
	string lines;
	string line;
	while (file) {
		std::getline(file, line);
		if (line.empty()) {
			continue;
		}
		lines += line;
		lines += " ,pattern matched: ";
		std::smatch sm;
		while (std::regex_search(line, sm, std::regex("0((\.[0-9]+))"))) {
			lines += sm[0];
			lines += " ";
			line = sm.suffix();
		}
		lines += "\n";
	}
	std::cout << lines;
	return cubeVerts;
}

Settings parseSettings(std::ifstream& input)
{
	Settings set;
	std::string line;
	std::string prefix{ "LUT_3D_SIZE" };

	std::getline(input, line);
	
	while (-1 == line.rfind(prefix, 0) && input.good())
	{
		std::getline(input, line);
	}
	//line.erase(remove_if(line.begin(), line.end(), " "));
	line = line.substr(prefix.size(), line.size());
	std::cout << "cube dimension: " << line << std::endl;

	try {
		set.size = std::stoi(line);
	}
	catch (std::invalid_argument& e) {
		std::cerr << e.what() << std::endl;
	}

	return set;
}