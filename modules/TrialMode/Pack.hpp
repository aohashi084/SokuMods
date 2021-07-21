//
// Created by PinkySmile on 20/07/2021.
//

#ifndef SWRSTOYS_PACK_HPP
#define SWRSTOYS_PACK_HPP


#include <nlohmann/json.hpp>
#include <SokuLib.hpp>

#ifdef _DEBUG
#define PANIC abort()
#else
#define PANIC
#endif

struct Icon {
	SokuLib::Vector2f translate = {0, 0};
	SokuLib::DrawUtils::Sprite sprite;

	Icon(const std::string &path, const nlohmann::json &object);
};

class Scenario {
private:
	std::string _name;
	std::string _description;

public:
	std::string file;
	SokuLib::DrawUtils::Sprite name;
	SokuLib::DrawUtils::Sprite preview;
	SokuLib::DrawUtils::Sprite description;

	Scenario(int i, const std::string &path, const nlohmann::json &object);
};

class Pack {
private:
	std::string _name;
	std::string _error;
	std::string _author;

public:
	std::string category;
	std::unique_ptr<Icon> icon;
	std::vector<std::unique_ptr<Scenario>> scenarios;
	SokuLib::DrawUtils::Sprite name;
	SokuLib::DrawUtils::Sprite error;
	SokuLib::DrawUtils::Sprite author;

	Pack(const std::string &path, const nlohmann::json &object);
};

extern std::vector<std::string> uniqueCategories;
extern std::vector<std::shared_ptr<Pack>> loadedPacks;
extern std::map<unsigned, std::string> validCharacters;
extern const std::map<unsigned, std::string> swrCharacters;
extern bool hasSoku2;
extern char packsLocation[1024 + MAX_PATH];

void loadPacks();


#endif //SWRSTOYS_PACK_HPP