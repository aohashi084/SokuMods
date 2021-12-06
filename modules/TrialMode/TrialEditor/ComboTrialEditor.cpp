//
// Created by PinkySmile on 23/07/2021.
//

#include <dinput.h>
#include <fstream>
#include "InputBox.hpp"
#include "Patches.hpp"
#include "ComboTrialEditor.hpp"
#include "Actions.hpp"
#include "Menu.hpp"
#include "Pack.hpp"

#ifndef _DEBUG
#define puts(...)
#define printf(...)
#endif

static SokuLib::KeyInput empty{0, 0, 0, 0, 0, 0, 0, 0};
static char data[10];

const std::map<unsigned, const char *> ComboTrialEditor::_stagesNames{
	{ SokuLib::STAGE_HAKUREI_SHRINE_BROKEN,                       "Hakurei Shrine (Broken)" },
	{ SokuLib::STAGE_FOREST_OF_MAGIC,                             "Forest of Magic" },
	{ SokuLib::STAGE_CREEK_OF_GENBU,                              "Creek of Genbu" },
	{ SokuLib::STAGE_YOUKAI_MOUNTAIN,                             "Youkai Mountain" },
	{ SokuLib::STAGE_MYSTERIOUS_SEA_OF_CLOUD,                     "Mysterious Sea of Cloud" },
	{ SokuLib::STAGE_BHAVA_AGRA,                                  "Bhava Agra" },
	{ SokuLib::STAGE_SPACE,                                       "Space" },
	{ SokuLib::STAGE_HAKUREI_SHRINE,                              "Hakurei Shrine" },
	{ SokuLib::STAGE_KIRISAME_MAGIC_SHOP,                         "Kirisame Magic Shop" },
	{ SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER,           "Scarlet Devil Mansion: Clock Tower" },
	{ SokuLib::STAGE_FOREST_OF_DOLLS,                             "Forest of Dolls" },
	{ SokuLib::STAGE_SCARLET_DEVIL_MANSION_LIBRARY,               "Scarlet Devil Mansion: Library" },
	{ SokuLib::STAGE_NETHERWORLD,                                 "Netherworld" },
	{ SokuLib::STAGE_SCARLET_DEVIL_MANSION_FOYER,                 "Scarlet Devil Mansion: Foyer" },
	{ SokuLib::STAGE_HAKUGYOKUROU_SNOWY_GARDEN,                   "Hakugyokurou Snowy Garden" },
	{ SokuLib::STAGE_BAMBOO_FOREST_OF_THE_LOST,                   "Bamboo Forest of the Lost" },
	{ SokuLib::STAGE_SHORE_OF_MISTY_LAKE,                         "Shore of Misty Lake" },
	{ SokuLib::STAGE_MORIYA_SHRINE,                               "Moriya Shrine" },
	{ SokuLib::STAGE_MOUTH_OF_GEYSER,                             "Mouth of Geyser" },
	{ SokuLib::STAGE_CATWALK_OF_GEYSER,                           "Catwalk of Geyser" },
	{ SokuLib::STAGE_FUSION_REACTOR_CORE,                         "Fusion Reactor Core" },
	{ SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER_SKETCH_BG, "Scarlet Devil Mansion: Clock Tower<br>(Sketch Background)" },
	{ SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER_BLURRY,    "Scarlet Devil Mansion: Clock Tower<br>(Blurry)" },
	{ SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER_SKETCH,    "Scarlet Devil Mansion: Clock Tower<br>(Sketch)" },
	{ 100,                                                        "Custom Stage" }
};
const std::vector<unsigned> ComboTrialEditor::_stagesIds{
	SokuLib::STAGE_HAKUREI_SHRINE_BROKEN,
	SokuLib::STAGE_FOREST_OF_MAGIC,
	SokuLib::STAGE_CREEK_OF_GENBU,
	SokuLib::STAGE_YOUKAI_MOUNTAIN,
	SokuLib::STAGE_MYSTERIOUS_SEA_OF_CLOUD,
	SokuLib::STAGE_BHAVA_AGRA,
	SokuLib::STAGE_SPACE,
	SokuLib::STAGE_HAKUREI_SHRINE,
	SokuLib::STAGE_KIRISAME_MAGIC_SHOP,
	SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER,
	SokuLib::STAGE_FOREST_OF_DOLLS,
	SokuLib::STAGE_SCARLET_DEVIL_MANSION_LIBRARY,
	SokuLib::STAGE_NETHERWORLD,
	SokuLib::STAGE_SCARLET_DEVIL_MANSION_FOYER,
	SokuLib::STAGE_HAKUGYOKUROU_SNOWY_GARDEN,
	SokuLib::STAGE_BAMBOO_FOREST_OF_THE_LOST,
	SokuLib::STAGE_SHORE_OF_MISTY_LAKE,
	SokuLib::STAGE_MORIYA_SHRINE,
	SokuLib::STAGE_MOUTH_OF_GEYSER,
	SokuLib::STAGE_CATWALK_OF_GEYSER,
	SokuLib::STAGE_FUSION_REACTOR_CORE,
	SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER_SKETCH_BG,
	SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER_BLURRY,
	SokuLib::STAGE_SCARLET_DEVIL_MANSION_CLOCK_TOWER_SKETCH,
	100,
};

bool deckSaved()
{
	auto &trial = reinterpret_cast<ComboTrialEditor &>(*loadedTrial);

	if (trial.onDeckSaved)
		return (trial.*trial.onDeckSaved)();
	return false;
}

ComboTrialEditor::ComboTrialEditor(const char *folder, const char *path, SokuLib::Character player, const nlohmann::json &json) :
	TrialEditor(folder, json)
{
	this->_path = path;
	this->_introPlayed = true;
	this->_outroPlayed = true;

	this->_weatherArrows.texture.loadFromGame("data/menu/practice/WeatherTriangle.bmp");
	this->_weatherArrows.setSize(this->_weatherArrows.texture.getSize());
	this->_weatherArrows.rect.width = this->_weatherArrows.getSize().x;
	this->_weatherArrows.rect.height = this->_weatherArrows.getSize().y;
	this->_weatherArrows.setPosition({404, 188});

	this->_weathers.texture.loadFromGame("data/menu/practice/Weather_list.bmp");
	this->_weathers.setSize({this->_weathers.texture.getSize().x, 24});
	this->_weathers.rect.width = this->_weathers.getSize().x;
	this->_weathers.rect.height = this->_weathers.getSize().y;
	this->_weathers.setPosition({422, 188});

	for (int i = 0; i < sizeof(this->_digits) / sizeof(*this->_digits); i++) {
		this->_digits[i].texture.loadFromGame(("data/menu/practice/" + std::to_string(i) + ".bmp").c_str());
		this->_digits[i].setSize({this->_digits[i].texture.getSize().x, 24});
		this->_digits[i].rect.width = this->_digits[i].getSize().x;
		this->_digits[i].rect.height = this->_digits[i].getSize().y;
		this->_digits[i].setPosition({static_cast<int>(429 + 86 / 2 - this->_digits[i].getSize().x / 4), 317});
	}

	this->_twilight.texture.loadFromResource(myModule, MAKEINTRESOURCE(112));
	this->_twilight.setSize(this->_twilight.texture.getSize());
	this->_twilight.rect.width = this->_twilight.getSize().x;
	this->_twilight.rect.height = this->_twilight.getSize().y;
	this->_twilight.setPosition({
		static_cast<int>(404 + (this->_weatherArrows.getSize().x - this->_twilight.getSize().x) / 2),
		192
	});

	this->_normal.texture.loadFromResource(myModule, MAKEINTRESOURCE(116));
	this->_normal.setSize(this->_normal.texture.getSize());
	this->_normal.rect.width = this->_normal.getSize().x;
	this->_normal.rect.height = this->_normal.getSize().y;
	this->_normal.setPosition({443, 322});

	this->_pause.texture.loadFromResource(myModule, MAKEINTRESOURCE(80));
	this->_pause.setSize(this->_pause.texture.getSize());
	this->_pause.rect.width = this->_pause.getSize().x;
	this->_pause.rect.height = this->_pause.getSize().y;

	this->_trialEditorDummy.texture.loadFromResource(myModule, MAKEINTRESOURCE(84));
	this->_trialEditorDummy.setSize(this->_trialEditorDummy.texture.getSize());
	this->_trialEditorDummy.rect.width = this->_trialEditorDummy.getSize().x;
	this->_trialEditorDummy.rect.height = this->_trialEditorDummy.getSize().y;
	this->_trialEditorDummy.setPosition({20, 0});

	this->_trialEditorMisc.texture.loadFromResource(myModule, MAKEINTRESOURCE(88));
	this->_trialEditorMisc.setSize(this->_trialEditorMisc.texture.getSize());
	this->_trialEditorMisc.rect.width = this->_trialEditorMisc.getSize().x;
	this->_trialEditorMisc.rect.height = this->_trialEditorMisc.getSize().y;
	this->_trialEditorMisc.setPosition({20, 0});

	this->_trialEditorMockup.texture.loadFromResource(myModule, MAKEINTRESOURCE(92));
	this->_trialEditorMockup.setSize(this->_trialEditorMockup.texture.getSize());
	this->_trialEditorMockup.rect.width = this->_trialEditorMockup.getSize().x;
	this->_trialEditorMockup.rect.height = this->_trialEditorMockup.getSize().y;

	this->_trialEditorPlayer.texture.loadFromResource(myModule, MAKEINTRESOURCE(96));
	this->_trialEditorPlayer.setSize(this->_trialEditorPlayer.texture.getSize());
	this->_trialEditorPlayer.rect.width = this->_trialEditorPlayer.getSize().x;
	this->_trialEditorPlayer.rect.height = this->_trialEditorPlayer.getSize().y;
	this->_trialEditorPlayer.setPosition({20, 0});

	this->_trialEditorSystem.texture.loadFromResource(myModule, MAKEINTRESOURCE(100));
	this->_trialEditorSystem.setSize(this->_trialEditorSystem.texture.getSize());
	this->_trialEditorSystem.rect.width = this->_trialEditorSystem.getSize().x;
	this->_trialEditorSystem.rect.height = this->_trialEditorSystem.getSize().y;
	this->_trialEditorSystem.setPosition({20, 0});

	this->_comboEditFG.texture.loadFromResource(myModule, MAKEINTRESOURCE(104));
	this->_comboEditFG.setSize(this->_comboEditFG.texture.getSize());
	this->_comboEditFG.rect.width = this->_comboEditFG.getSize().x;
	this->_comboEditFG.rect.height = this->_comboEditFG.getSize().y;

	this->_comboEditBG.texture.loadFromResource(myModule, MAKEINTRESOURCE(108));
	this->_comboEditBG.setSize(this->_comboEditBG.texture.getSize());
	this->_comboEditBG.rect.width = this->_comboEditBG.getSize().x;
	this->_comboEditBG.rect.height = this->_comboEditBG.getSize().y;

	if (json.contains("dolls") && json["dolls"].is_array()) {
		for (int i = 0; i < json["dolls"].size(); i++) {
			auto &obj = json["dolls"][i];

			if (!obj.is_object())
				continue;
			if (!obj.contains("x") && obj["x"].is_number())
				continue;
			if (!obj.contains("y") && obj["y"].is_number())
				continue;
			if (!obj.contains("dir") && obj["dir"].is_number())
				continue;
			this->_dolls.push_back({SokuLib::Vector2f{obj["x"], obj["y"]}, obj["dir"].get<SokuLib::Direction>()});
		}
	}

	this->_jump = getField(json, false, &nlohmann::json::is_boolean, "jump", "dummy");
	this->_failTimer = getField(json, 60, &nlohmann::json::is_number, "fail_timer");
	this->_crouching = getField(json, false, &nlohmann::json::is_boolean, "crouch", "dummy");
	this->_leftWeather = getField(json, true, &nlohmann::json::is_boolean, "affected_by_weather", "player");
	this->_rightWeather = getField(json, true, &nlohmann::json::is_boolean, "affected_by_weather", "dummy");
	memset(&this->_skills, 0xFF, sizeof(this->_skills));
	if (json.contains("skills") && json["skills"].is_array() && json["skills"].size() == characterSkills[player].size()) {
		for (int i = 0; i < json["skills"].size(); i++) {
			auto &arr = json["skills"][i];

			if (!arr.is_array() || arr.size() != 2 || !arr[0].is_number() || arr[0].get<int>() < 0 || arr[0].get<int>() > 2 || !arr[1].is_number()) {
				MessageBox(
					SokuLib::window,
					("Element #" + std::to_string(i) + " in the skill array was not valid and will be discarded").c_str(),
					"Invalid skill array",
					MB_ICONWARNING
				);
				this->_skills[i].notUsed = false;
				this->_skills[i].level = 0;
			} else {
				this->_skills[i + characterSkills[player].size() * arr[0].get<int>()].notUsed = false;
				this->_skills[i + characterSkills[player].size() * arr[0].get<int>()].level = arr[1];
			}
		}
	} else
		for (int i = 0; i < characterSkills[player].size(); i++) {
			this->_skills[i].notUsed = false;
			this->_skills[i].level = 0;
		}

	try {
		this->_hand = json.contains("hand") && json["hand"].is_array() ? json["hand"].get<std::vector<unsigned short>>() : std::vector<unsigned short>{};
	} catch (...) {}

	for (auto card : this->_hand) {
		if (card <= 20)
			continue;
		for (int i = 0; i < SokuLib::leftPlayerInfo.effectiveDeck.size; i++)
			if (card == SokuLib::leftPlayerInfo.effectiveDeck[i])
				goto ok;
		this->_hand.clear();
		break;
	ok:
		continue;
	}

	this->_weather = json.contains("weather") && json["weather"].is_number() ? static_cast<SokuLib::Weather>(json["weather"].get<int>()) : SokuLib::WEATHER_CLEAR;
	if (json.contains("weather") && json["weather"].is_string()) {
		std::string weather = json["weather"];

		std::for_each(weather.begin(), weather.end(), [](char &c){ c = tolower(c); });

		auto it = weathers.find(weather);

		if (it != weathers.end())
			this->_weather = it->second;
	}

	this->_mpp           = getField(json, false, &nlohmann::json::is_boolean, "mpp",            "player");
	this->_stones        = getField(json, false, &nlohmann::json::is_boolean, "stones",         "player");
	this->_orerries      = getField(json, false, &nlohmann::json::is_boolean, "orreries",       "player");
	this->_tickTimer     = getField(json, false, &nlohmann::json::is_boolean, "tick_timer",     "player");
	this->_privateSquare = getField(json, false, &nlohmann::json::is_boolean, "private_square", "player");

	this->_disableLimit = getField(json, false, &nlohmann::json::is_boolean, "disable_limit");
	this->_uniformCardCost = getField(json, -1, &nlohmann::json::is_number, "uniform_card_cost");
	this->_playerStartPos = getField(json, 480.f, &nlohmann::json::is_number, "pos", "player");
	this->_dummyStartPos.x = getField(json, 800.f, &nlohmann::json::is_number, "x", "dummy", "pos");
	this->_dummyStartPos.y = getField(json, 0.f, &nlohmann::json::is_number, "y", "dummy", "pos");
	try {
		this->_loadExpected(getField<std::string>(json, "", &nlohmann::json::is_string, "expected"));
	} catch (...) {}

	this->_comboSprite.texture.createFromText(this->_transformComboToString().c_str(), defaultFont12, {250, 30});
	this->_comboSprite.setPosition({388, 353});
	this->_comboSprite.setSize(this->_comboSprite.texture.getSize());
	this->_comboSprite.rect.left = 0;
	this->_comboSprite.rect.top = 0;
	this->_comboSprite.rect.width = this->_comboSprite.texture.getSize().x;
	this->_comboSprite.rect.height = this->_comboSprite.texture.getSize().y;

	this->_introSprite.texture.createFromText(this->_introRelPath.c_str(), defaultFont12, {250, 30});
	this->_introSprite.setPosition({379, 260});
	this->_introSprite.setSize(this->_introSprite.texture.getSize());
	this->_introSprite.rect.left = 0;
	this->_introSprite.rect.top = 0;
	this->_introSprite.rect.width = this->_introSprite.texture.getSize().x;
	this->_introSprite.rect.height = this->_introSprite.texture.getSize().y;

	this->_outroSprite.texture.createFromText(this->_outroRelPath.c_str(), defaultFont12, {250, 30});
	this->_outroSprite.setPosition({379, 294});
	this->_outroSprite.setSize(this->_outroSprite.texture.getSize());
	this->_outroSprite.rect.left = 0;
	this->_outroSprite.rect.top = 0;
	this->_outroSprite.rect.width = this->_outroSprite.texture.getSize().x;
	this->_outroSprite.rect.height = this->_outroSprite.texture.getSize().y;

	this->_attemptText.texture.createFromText("Editor mode", defaultFont16, {306, 124});
	this->_attemptText.setPosition({4, 58});
	this->_attemptText.setSize(this->_attemptText.texture.getSize());
	this->_attemptText.rect.left = 0;
	this->_attemptText.rect.top = 0;
	this->_attemptText.rect.width = this->_attemptText.texture.getSize().x;
	this->_attemptText.rect.height = this->_attemptText.texture.getSize().y;

	ScorePrerequisites *old = nullptr;

	if (!json.contains("score") || !json["score"].is_array() || json["score"].size() != 4)
		for (int i = 0; i < 4; i++) {
			this->_scores.emplace_back(nlohmann::json{}, old);
			old = &this->_scores.back();
		}
	else
		for (auto &j : json["score"])
			try {
				this->_scores.emplace_back(j, old);
				old = &this->_scores.back();
			} catch (std::exception &e) {}

	DWORD oldV;

	::VirtualProtect((void *)TEXT_SECTION_OFFSET, TEXT_SECTION_SIZE, PAGE_EXECUTE_READWRITE, &oldV);
	memcpy(data, (void *)0x00450121, 10);
	SokuLib::TamperNearCall(0x00450121, deckSaved);
	SokuLib::TamperNearJmp(0x00450126, 0x0045017F);
	::VirtualProtect((void *)TEXT_SECTION_OFFSET, TEXT_SECTION_SIZE, oldV, &oldV);
	::FlushInstructionCache(GetCurrentProcess(), nullptr, 0);

	for (auto &pair : ComboTrialEditor::_stagesNames) {
		auto sprites = std::pair<std::unique_ptr<SokuLib::DrawUtils::Sprite>, std::unique_ptr<SokuLib::DrawUtils::Sprite>>(
			new SokuLib::DrawUtils::Sprite(),
			new SokuLib::DrawUtils::Sprite()
		);

		if (!sprites.first->texture.loadFromGame(("data/scene/select/bg/bg_pict" + (pair.first < 10 ? std::string("0") : std::string()) + std::to_string(pair.first) + ".bmp").c_str()))
			sprites.first->texture.loadFromGame("data/scene/select/bg/bg_pict99.bmp");
		sprites.first->setSize({352, 64});
		sprites.first->rect.width = sprites.first->getSize().x;
		sprites.first->rect.height = sprites.first->getSize().y;

		if (!sprites.second->texture.loadFromGame(("data/scene/select/bg/bg_name" + (pair.first < 10 ? std::string("0") : std::string()) + std::to_string(pair.first) + ".bmp").c_str()))
			sprites.second->texture.createFromText((std::string("<br><br>") + (strstr(pair.second, "<br>") ? "" : "<br>") + pair.second).c_str(), defaultFont16, {352, 70});
		sprites.second->setSize(sprites.second->texture.getSize());
		sprites.second->rect.width = sprites.second->getSize().x;
		sprites.second->rect.height = sprites.second->getSize().y;
		this->_stagesSprites[pair.first].first.swap(sprites.first);
		this->_stagesSprites[pair.first].second.swap(sprites.second);
	}
	this->_stageRect.setSize({360, 72});
	this->_stageRect.setPosition({140, 204});
	this->_stageRect.setBorderColor(SokuLib::Color::Transparent);
	this->_stageRect.setFillColor(SokuLib::Color{0xFF, 0xFF, 0xFF, 0xA0});

	SokuLib::CSVParser parser(SokuLib::SWRUnlinked ? "data/csv/system/music_limit.csv" : "data/csv/system/music.csv");
	SokuLib::Vector2i size;

	do {
		this->_musics.emplace_back(parser.getNextCell(), parser.getNextCell(), new SokuLib::DrawUtils::Sprite());

		auto &sprite = std::get<2>(this->_musics.back());

		sprite->texture.createFromText(std::get<0>(this->_musics.back()).c_str(), defaultFont12, {220, 16}, &size);
		sprite->setSize({200, 16});
		sprite->rect.width = 200;
		sprite->rect.height = 16;
	} while (parser.goToNextLine());
	this->_musics.emplace_back("", "", new SokuLib::DrawUtils::Sprite());

	auto &sprite = std::get<2>(this->_musics.back());

	sprite->texture.createFromText("Custom music", defaultFont12, {200, 16}, &size);
	sprite->setSize(size.to<unsigned>());
	sprite->rect.width = size.x;
	sprite->rect.height = size.y;
}

ComboTrialEditor::~ComboTrialEditor()
{
	DWORD oldV;

	::VirtualProtect((void *)TEXT_SECTION_OFFSET, TEXT_SECTION_SIZE, PAGE_EXECUTE_READWRITE, &oldV);
	memcpy((void *)0x00450121, data, 10);
	::VirtualProtect((void *)TEXT_SECTION_OFFSET, TEXT_SECTION_SIZE, oldV, &oldV);
	::FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
}

bool ComboTrialEditor::update(bool &canHaveNextFrame)
{
	auto &battleMgr = SokuLib::getBattleMgr();

	if (*(char*)0x89a88c == 2)
		return true;
	if ((this->_quit || this->_needReload) && battleMgr.leftCharacterManager.objectBase.hp == 1) {
		canHaveNextFrame = false;
		return true;
	}
	if (this->_changingPlayerPos)
		battleMgr.leftCharacterManager.objectBase.position.x = this->_fakePlayerPos;
	battleMgr.rightCharacterManager.nameHidden = true;
	if (!this->_introPlayed) {
		SokuLib::displayedWeather = this->_weather;
		SokuLib::activeWeather = this->_weather;
		canHaveNextFrame = this->_firstFirst == 1;
		if (this->_firstFirst)
			this->_firstFirst--;
		else
			this->_introOnUpdate();
		if (this->_introPlayed)
			SokuLib::activeWeather = SokuLib::WEATHER_CLEAR;
		return false;
	}

	this->_rotation += 0.025;
	this->_dollAnim++;
	this->_dollAnim &= 0b11111;
	if (this->_freezeCounter) {
		canHaveNextFrame = (this->_freezeCounter % max((5 - this->_freezeCounter / 30), 1) == 0);
		this->_freezeCounter--;
	}

	if (!this->_leftWeather)
		battleMgr.leftCharacterManager.swordOfRaptureDebuffTimeLeft = 3;
	if (!this->_rightWeather)
		battleMgr.rightCharacterManager.swordOfRaptureDebuffTimeLeft = 3;
	if (!this->_outroPlayed && this->_finished && !this->_playingIntro) {
		if (!this->_dummyHit) {
			if ((*reinterpret_cast<char **>(0x8985E8))[0x494] < 22) {
				(*reinterpret_cast<char **>(0x8985E8))[0x494]++;
				return false;
			}
			this->_outroOnUpdate();
			canHaveNextFrame = false;
			if (this->_outroPlayed)
				this->_finished = false;
			return false;
		}
	}

	if ((*reinterpret_cast<char **>(0x8985E8))[0x494] && !this->_playingIntro)
		(*reinterpret_cast<char **>(0x8985E8))[0x494]--;

	if (this->_isStart || this->_quit || this->_needReload || this->_needInit) {
		this->_quit = false;
		this->_needReload = false;
		this->_initGameStart();
		return false;
	}

	if (this->_dolls.size() * 2 != SokuLib::getBattleMgr().leftCharacterManager.objects.list.size && this->_managingDolls)
		this->_initGameStart();
	else if (this->_managingDolls) {
		SokuLib::camera.backgroundTranslate.x = 640;
		SokuLib::camera.backgroundTranslate.y = 0;
		SokuLib::camera.scale = 0.5;
		if (SokuLib::inputMgrs.input.spellcard == 1 && this->_dollCursorPos != this->_dolls.size()) {
			this->_dolls.erase(this->_dolls.begin() + this->_dollCursorPos);
			this->_initGameStart();
			SokuLib::playSEWaveBuffer(0x29);
		}
		if (SokuLib::inputMgrs.input.b == 1) {
			this->_openPause();
			SokuLib::playSEWaveBuffer(0x29);
		}
		if (SokuLib::inputMgrs.input.c == 1 && this->_dollCursorPos != this->_dolls.size()) {
			SokuLib::getBattleMgr().leftCharacterManager.objects.list.vector()[this->_dollCursorPos * 2]->direction =
			this->_dolls[this->_dollCursorPos].dir = static_cast<SokuLib::Direction>(this->_dolls[this->_dollCursorPos].dir * -1);
			SokuLib::playSEWaveBuffer(0x28);
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			if (this->_dollCursorPos == this->_dolls.size()) {
				SokuLib::playSEWaveBuffer(0x28);
				this->_dolls.push_back({
					SokuLib::getBattleMgr().leftCharacterManager.objectBase.position,
					SokuLib::getBattleMgr().leftCharacterManager.objectBase.direction
				});
				this->_initGameStart();
			} else {
				SokuLib::playSEWaveBuffer(0x28 + this->_dollSelected);
				this->_dollSelected = !this->_dollSelected;
			}
		}
		if (this->_dollSelected) {
			auto &pos = SokuLib::getBattleMgr().leftCharacterManager.objects.list.vector()[this->_dollCursorPos * 2]->position;

			if (SokuLib::inputMgrs.input.horizontalAxis)
				pos.x += std::copysign(6, SokuLib::inputMgrs.input.horizontalAxis);
			if (SokuLib::inputMgrs.input.verticalAxis)
				pos.y += std::copysign(6, -SokuLib::inputMgrs.input.verticalAxis);
			this->_dolls[this->_dollCursorPos].pos = pos;
		} else {
			if (SokuLib::inputMgrs.input.horizontalAxis == -1 || SokuLib::inputMgrs.input.verticalAxis == -1) {
				if (this->_dollCursorPos == 0)
					this->_dollCursorPos = this->_dolls.size();
				else
					this->_dollCursorPos--;
				SokuLib::playSEWaveBuffer(0x27);
			} else if (SokuLib::inputMgrs.input.horizontalAxis == 1 || SokuLib::inputMgrs.input.verticalAxis == 1) {
				if (this->_dollCursorPos == this->_dolls.size())
					this->_dollCursorPos = 0;
				else
					this->_dollCursorPos++;
				SokuLib::playSEWaveBuffer(0x27);
			}
		}
	} else if (this->_comboPageDisplayed && !this->_playingIntro) {
		if (std::abs(SokuLib::inputMgrs.input.verticalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.verticalAxis) > 36 && SokuLib::inputMgrs.input.verticalAxis % 6 == 0)) {
			this->_comboEditCursor = static_cast<int>((this->_comboEditCursor + 3) + std::copysign(1, SokuLib::inputMgrs.input.verticalAxis)) % 3;
			SokuLib::playSEWaveBuffer(0x27);
		}
		if (SokuLib::inputMgrs.input.b == 1) {
			this->_openPause();
			SokuLib::playSEWaveBuffer(0x29);
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			SokuLib::playSEWaveBuffer(0x28);
			switch (this->_comboEditCursor) {
			case 0:
				notImplemented();
				break;
			case 1:
				this->_typeNewCombo();
				break;
			case 2:
				this->_playComboAfterIntro = true;
				this->_initGameStart();
				return true;
			}
		}
	}

	if (this->_tickTimer);
	else if (this->_mpp)
		battleMgr.leftCharacterManager.missingPurplePowerTimeLeft = 900;
	else if (this->_stones)
		battleMgr.leftCharacterManager.philosophersStoneTime = 900;
	else if (this->_orerries)
		battleMgr.leftCharacterManager.orreriesTimeLeft = 900;
	else if (this->_privateSquare)
		battleMgr.leftCharacterManager.privateSquare = 900 - (battleMgr.leftCharacterManager.privateSquare & 1);

	if (this->_playingIntro && SokuLib::inputMgrs.input.a != 1 && this->_waitCounter == 30 && !this->_comboPageDisplayed) {
		battleMgr.rightCharacterManager.objectBase.position.x = this->_dummyStartPos.x;
		battleMgr.rightCharacterManager.objectBase.position.y = this->_dummyStartPos.y;
		battleMgr.rightCharacterManager.objectBase.speed.y = 0;
		goto disableLimit;
	}
	if (this->_waitCounter) {
		this->_waitCounter--;
	} else if (this->_playingIntro)
		this->_playIntro();
	else if (this->_actionCounter != this->_exceptedActions.size()) {
		auto i = this->_actionCounter;

		while (i == this->_actionCounter || this->_exceptedActions[i - 1]->optional) {
			if (i >= this->_exceptedActions.size())
				break;
			for (auto act : this->_exceptedActions[i]->actions)
				if (
					addCustomActions(battleMgr.leftCharacterManager, SokuLib::leftChar) == act &&
					isStartOfMove(act, battleMgr.leftCharacterManager, SokuLib::leftChar)
				) {
					this->_actionCounter = i + 1;
					goto checkFinish;
				}
			i++;
		}
	}

checkFinish:
	if (!this->_finished && this->_scores.front().met(0)) {
		auto i = this->_actionCounter;

		while (i < this->_exceptedActions.size()) {
			if (!this->_exceptedActions[i]->optional)
				goto disableLimit;
			i++;
		}
		SokuLib::playSEWaveBuffer(44);
		if (!this->_playingIntro)
			this->_freezeCounter = 120;
		this->_finished = true;
		return false;
	}

disableLimit:
	if (this->_disableLimit) {
		battleMgr.leftCharacterManager.combo.limit = 0;
		battleMgr.rightCharacterManager.combo.limit = 0;
		battleMgr.leftCharacterManager.realLimit = 0;
		battleMgr.rightCharacterManager.realLimit = 0;
	}

	auto hit = battleMgr.rightCharacterManager.objectBase.action >= SokuLib::ACTION_STAND_GROUND_HIT_SMALL_HITSTUN &&
	           battleMgr.rightCharacterManager.objectBase.action <= SokuLib::ACTION_FORWARD_DASH;

	if (this->_playComboAfterIntro && !hit && battleMgr.leftCharacterManager.objectBase.action <= SokuLib::ACTION_STAND_GROUND_HIT_SMALL_HITSTUN) {
		this->_initGameStart();
		return false;
	}

	if (this->_actionCounter && !this->_dummyHit && !battleMgr.leftCharacterManager.timeStop && !battleMgr.rightCharacterManager.timeStop)
		this->_timer++;
	else
		this->_timer = 0;
	this->_isStart = this->_timer >= this->_failTimer;
	battleMgr.currentRound = 3;
	battleMgr.leftCharacterManager.score = 0;
	if (
		(SokuLib::activeWeather != this->_weather && this->_weather != SokuLib::WEATHER_AURORA) ||
		(this->_weather == SokuLib::WEATHER_AURORA && (SokuLib::displayedWeather != this->_weather || SokuLib::activeWeather == SokuLib::WEATHER_CLEAR))
	) {
		if (SokuLib::activeWeather == SokuLib::WEATHER_CLEAR) {
			SokuLib::weatherCounter = this->_weather == SokuLib::WEATHER_CLEAR ? 0 : 999;
			SokuLib::displayedWeather = this->_weather;
		} else
			SokuLib::weatherCounter = 0;
	} else
		SokuLib::weatherCounter = this->_weather == SokuLib::WEATHER_CLEAR ? 0 : 750;
	if (this->_dummyHit && !hit && (!this->_finished || this->_playingIntro))
		this->_isStart = true;
	else if (this->_dummyHit && !hit)
		this->_dummyHit = false;
	this->_dummyHit |= hit;
	if (!this->_dummyHit && !this->_finished) {
		battleMgr.rightCharacterManager.objectBase.position.x = this->_dummyStartPos.x;
		if (!this->_jump) {
			battleMgr.rightCharacterManager.objectBase.position.y = this->_dummyStartPos.y;
			battleMgr.rightCharacterManager.objectBase.speed.y = 0;
		}

		if (!this->_playingIntro && this->_outroRequ == 1) {
			try {
				this->_outroRequ = 0;
				this->_initAnimations(false, true);
				this->_outroPlayed = false;
				this->_finished = true;
			} catch (std::exception &e) {
				MessageBox(
					SokuLib::window,
					e.what(),
					"Outro loading error",
					MB_ICONERROR
				);
			}
		} else if (!this->_playingIntro && this->_introRequ == 1) {
			try {
				this->_introRequ = 0;
				ComboTrialEditor::_initVanillaGame();
				this->_initAnimations(true, false);
				this->_introPlayed = false;
			} catch (std::exception &e) {
				MessageBox(
					SokuLib::window,
					e.what(),
					"Intro loading error",
					MB_ICONERROR
				);
				this->_initGameStart();
			}
		}
	}
	return false;
}

void ComboTrialEditor::render() const
{
	if (!this->_introPlayed)
		return this->_introOnRender();

	if (this->_finished && !this->_outroPlayed && !this->_dummyHit)
		return this->_outroOnRender();

	if (this->_finished && !this->_playingIntro)
		return;
	if (this->_managingDolls && this->_dollCursorPos < this->_dolls.size()) {
		auto pos = this->_dolls[this->_dollCursorPos].pos - SokuLib::Vector2f{32 - 7, -32};
		pos.y *= -1;
		auto result = (pos + SokuLib::camera.translate) * SokuLib::camera.scale;

		displaySokuCursor(
			result.to<int>(),
			(SokuLib::Vector2u{64, 64} * SokuLib::camera.scale).to<unsigned>()
		);
	}

	if (!this->_playingIntro)
		this->_attemptText.draw();
	else
		return;

	if (this->_comboPageDisplayed) {
		this->_comboEditBG.draw();
		displaySokuCursor({331, static_cast<int>(111 + 26 * this->_comboEditCursor)}, {112, 16});
		this->_comboEditFG.draw();
	}

	SokuLib::Vector2i pos = {160, 60};
	auto last = 0;

	if (this->_recordingCombo) {
		for (const auto &elem : this->_recordBuffer) {
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0x60, 0x60, 0x60};
			elem->sprite.setPosition(pos);
			elem->sprite.draw();
			pos.y += elem->sprite.getSize().y;
		}
		return;
	}

	for (int i = 0; i < this->_exceptedActions.size(); i++) {
		auto &elem = this->_exceptedActions[i];

		if (this->_actionCounter == i)
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0x60, 0xFF, 0x60};
		else if (this->_actionCounter > i)
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0x60, 0x60, 0x60};
		else if (elem->optional)
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0xFF, 0xFF, 0x60};
		else {
			bool good = false;

			for (int j = i; j > 0 && this->_exceptedActions[j - 1]->optional; j--)
				good |= (j - 1) == this->_actionCounter;

			if (good)
				elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0x60, 0xFF, 0x60};
			else
				elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor::White;
		}
		elem->sprite.setPosition(pos);
		elem->sprite.draw();
		pos.y += elem->sprite.getSize().y;
	}
}

int ComboTrialEditor::getScore()
{
	int index = 0;

	while (index < this->_scores.size() && this->_scores[index].met(0))
		index++;
	return index - 1;
}

void ComboTrialEditor::_initGameStart()
{
	auto &battleMgr = SokuLib::getBattleMgr();

	if (this->_currentDoll && battleMgr.leftCharacterManager.objects.list.size) {
		auto obj = battleMgr.leftCharacterManager.objects.list.vector()[battleMgr.leftCharacterManager.objects.list.size - 2];

		obj->action = static_cast<SokuLib::Action>(805);
		obj->animate();
		while (obj->actionBlockId != 8)
			obj->animate2();
		obj->position = this->_dolls[this->_currentDoll - 1].pos;
		obj->direction = this->_dolls[this->_currentDoll - 1].dir;
		battleMgr.leftCharacterManager.aliceDollCount = 0;
	} else if (SokuLib::leftChar == SokuLib::CHARACTER_ALICE) {
		for (auto &obj : battleMgr.leftCharacterManager.objects.list.vector()) {
			if (
				(obj->action == 805 && obj->image->number >= 304 && obj->image->number <= 313) || //This is Alice's doll (C)
				(obj->action == 825 && obj->image->number >= 322 && obj->image->number <= 325)    //This is Alice's doll (d22)
			) {
				obj->action = static_cast<SokuLib::Action>(805);
				obj->animate();
				obj->actionBlockId = 7;
			}
		}
	}
	if (this->_currentDoll != this->_dolls.size()) {
		battleMgr.leftCharacterManager.objectBase.position.y = 0;
		battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_5C;
		battleMgr.leftCharacterManager.objectBase.animate();
		while (battleMgr.leftCharacterManager.objectBase.frameCount != 9)
			battleMgr.leftCharacterManager.objectBase.doAnimation();
		this->_lastSize = battleMgr.leftCharacterManager.objects.list.size;
		this->_currentDoll++;
		this->_needInit = true;
		return;
	}

	this->_dollSelected = false;
	this->_needInit = false;
	this->_currentDoll = 0;
	this->_isStart = false;
	this->_dummyHit = false;
	this->_finished = false;
	this->_playingIntro = this->_playComboAfterIntro;
	this->_playComboAfterIntro = false;
	this->_actionCounter = 0;
	if (this->_first)
		this->_playBGM();
	this->_first = false;
	if (this->_playingIntro) {
		this->_waitCounter += 30;
		(*reinterpret_cast<char **>(0x8985E8))[0x494] = 22;
	}

	this->_firstFirst = 0;
	battleMgr.leftCharacterManager.combo.limit = 0;
	battleMgr.leftCharacterManager.combo.damages = 0;
	battleMgr.leftCharacterManager.combo.nbHits = 0;
	battleMgr.leftCharacterManager.combo.rate = 0;
	battleMgr.leftCharacterManager.cardGauge = 0;
	battleMgr.leftCharacterManager.hand.size = 0;
	for (auto card : this->_hand) {
		auto obj = battleMgr.leftCharacterManager.addCard(card);

		if (this->_uniformCardCost)
			obj->cost = this->_uniformCardCost;
	}

	battleMgr.leftCharacterManager.objectBase.hp = 10000;
	battleMgr.leftCharacterManager.currentSpirit = 1000;
	battleMgr.leftCharacterManager.maxSpirit = 1000;

	if (this->_mpp) {
		puts("Init MPP");
		battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_USING_SC_ID_205;
		battleMgr.leftCharacterManager.objectBase.animate();
		while (battleMgr.leftCharacterManager.objectBase.frameCount != 80)
			battleMgr.leftCharacterManager.objectBase.doAnimation();
	} else if (this->_stones) {
		puts("Init Philosophers' Stone");
		if (battleMgr.leftCharacterManager.philosophersStoneTime <= 1 || this->_tickTimer)
			battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_USING_SC_ID_205;
		else
			battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_IDLE;
		battleMgr.leftCharacterManager.objectBase.animate();
	} else if (this->_orerries) {
		puts("Init Orreries");
		battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_USING_SC_ID_215;
		battleMgr.leftCharacterManager.objectBase.animate();
	} else if (this->_privateSquare) {
		puts("Init Private Square");
		battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_USING_SC_ID_201;
		battleMgr.leftCharacterManager.objectBase.animate();
	} else {
		battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_IDLE;
		battleMgr.leftCharacterManager.objectBase.animate();
	}
	battleMgr.leftCharacterManager.objectBase.position.x = this->_playerStartPos;
	battleMgr.leftCharacterManager.objectBase.position.y = 0;
	battleMgr.leftCharacterManager.objectBase.speed.x = 0;
	battleMgr.leftCharacterManager.objectBase.speed.y = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.xRotation = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.yRotation = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.zRotation = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.scale.x = 1;
	battleMgr.leftCharacterManager.objectBase.renderInfos.scale.y = 1;
	if (SokuLib::leftChar == SokuLib::CHARACTER_SANAE) {
		battleMgr.leftCharacterManager.suwakoTimeLeft = 0;
		battleMgr.leftCharacterManager.kanakoTimeLeft = 0;
	}
	//(*(void (__thiscall **)(SokuLib::ObjListManager &, int))&*battleMgr.leftCharacterManager.objects.offset_0x00)(battleMgr.leftCharacterManager.objects, 0);
	//battleMgr.leftCharacterManager.objects;
	memcpy(&battleMgr.leftCharacterManager.skillMap, &this->_skills, sizeof(this->_skills));
	if (SokuLib::leftChar == SokuLib::CHARACTER_ALICE)
		battleMgr.leftCharacterManager.aliceDollCount = this->_dolls.size();

	battleMgr.rightCharacterManager.objectBase.hp = 10000;
	battleMgr.rightCharacterManager.currentSpirit = 1000;
	battleMgr.rightCharacterManager.maxSpirit = 1000;
	battleMgr.rightCharacterManager.objectBase.renderInfos.xRotation = 0;
	battleMgr.rightCharacterManager.objectBase.renderInfos.yRotation = 0;
	battleMgr.rightCharacterManager.objectBase.renderInfos.zRotation = 0;
	if (this->_dummyStartPos.y == 0)
		battleMgr.rightCharacterManager.objectBase.action = this->_crouching ? SokuLib::ACTION_CROUCHED : SokuLib::ACTION_IDLE;
	else
		battleMgr.rightCharacterManager.objectBase.action = SokuLib::ACTION_FALLING;
	battleMgr.rightCharacterManager.objectBase.animate();
	battleMgr.rightCharacterManager.objectBase.position.x = this->_dummyStartPos.x;
	battleMgr.rightCharacterManager.objectBase.position.y = this->_dummyStartPos.y;
	battleMgr.rightCharacterManager.objectBase.speed.x = 0;
	battleMgr.rightCharacterManager.objectBase.speed.y = 0;
	battleMgr.rightCharacterManager.objectBase.direction =
		battleMgr.rightCharacterManager.objectBase.position.x > battleMgr.leftCharacterManager.objectBase.position.x ?
		SokuLib::LEFT : SokuLib::RIGHT;
}

void ComboTrialEditor::_loadExpected(const std::string &expected)
{
	bool par = false;
	char last = ' ';

	this->_exceptedActions.clear();
	this->_exceptedActions.emplace_back(new SpecialAction());
	for (auto c : expected) {
		par |= c == '(';
		par &= c != ')';
		if (!par && c == ' ') {
			if (last != ' ')
				this->_exceptedActions.emplace_back(new SpecialAction());
		} else
			this->_exceptedActions.back()->name += c;
		last = c;
	}
	for (auto &action : this->_exceptedActions)
		action->parse();
}

void ComboTrialEditor::_playIntro()
{
	if (this->_actionCounter == this->_exceptedActions.size())
		return;

	auto &battleMgr = SokuLib::getBattleMgr();
	auto &arr = this->_exceptedActions[this->_actionCounter];

	if (this->_actionWaitCounter < arr->delay) {
		this->_actionWaitCounter++;
		return;
	}
	arr->counter = (arr->counter + 1) % arr->inputs.size();
	if (
		addCustomActions(battleMgr.leftCharacterManager, SokuLib::leftChar) == arr->actions[0] &&
		isStartOfMove(arr->actions[0], battleMgr.leftCharacterManager, SokuLib::leftChar)
	) {
		if (arr->chargeTime) {
			arr->chargeCounter++;
		} else {
			arr->counter = 0;
			this->_actionWaitCounter = 0;
			this->_actionCounter++;
		}
	}
	if (arr->chargeCounter) {
		if (arr->chargeCounter == arr->chargeTime) {
			arr->counter = 0;
			arr->chargeCounter = 0;
			this->_actionWaitCounter = 0;
			this->_actionCounter++;
		} else
			arr->chargeCounter++;
	}
}

void ComboTrialEditor::editPlayerInputs(SokuLib::KeyInput &originalInputs)
{
	if (this->_managingDolls || (this->_comboPageDisplayed && !this->_playingIntro)) {
		memset(&originalInputs, 0, sizeof(originalInputs));
		return;
	}
	if (this->_changingPlayerPos) {
		this->_fakePlayerPos += 6 * originalInputs.horizontalAxis;
		this->_fakePlayerPos = min(1240, max(40, this->_fakePlayerPos));
		originalInputs.b = 0;
		originalInputs.c = 0;
		originalInputs.d = 0;
		originalInputs.verticalAxis = 0;
		originalInputs.spellcard = 0;
		originalInputs.changeCard = 0;
		originalInputs.horizontalAxis = 0;
		if (originalInputs.a == 1) {
			this->_openPause();
			this->_playerStartPos = this->_fakePlayerPos;
			this->_changingPlayerPos = false;
			SokuLib::playSEWaveBuffer(0x28);
			originalInputs.a = 0;
		}
		return;
	}
	if (this->_changingDummyPos) {
		this->_dummyStartPos.x += 6 * originalInputs.horizontalAxis;
		this->_dummyStartPos.x = min(1300, max(0, this->_dummyStartPos.x));
		this->_dummyStartPos.y -= 6 * originalInputs.verticalAxis;
		this->_dummyStartPos.y = min(1300, max(0, this->_dummyStartPos.y));
		originalInputs.b = 0;
		originalInputs.c = 0;
		originalInputs.d = 0;
		originalInputs.horizontalAxis = 0;
		originalInputs.verticalAxis = 0;
		originalInputs.spellcard = 0;
		originalInputs.changeCard = 0;
		if (originalInputs.a == 1) {
			this->_openPause();
			this->_changingDummyPos = false;
			this->_jump &= this->_dummyStartPos.y == 0;
			this->_crouching &= this->_dummyStartPos.y == 0;
			SokuLib::playSEWaveBuffer(0x28);
			originalInputs.a = 0;
		}
		return;
	}
	if (this->_playingIntro) {
		if (this->_actionCounter == this->_exceptedActions.size())
			return static_cast<void>(memset(&originalInputs, 0, sizeof(originalInputs)));
		if (this->_waitCounter)
			return static_cast<void>(memset(&originalInputs, 0, sizeof(originalInputs)));

		auto &arr = this->_exceptedActions[this->_actionCounter];

		if (arr->delay > this->_actionWaitCounter)
			return static_cast<void>(memset(&originalInputs, 0, sizeof(originalInputs)));
		if (arr->chargeCounter == 0)
			originalInputs = arr->inputs[arr->counter];
		else {
			originalInputs = arr->inputs.back();
			originalInputs.a *= 2;
			originalInputs.b *= 2;
			originalInputs.c *= 2;
			originalInputs.d *= 2;
			originalInputs.horizontalAxis *= 2;
			originalInputs.verticalAxis *= 2;
			originalInputs.changeCard *= 2;
			originalInputs.spellcard *= 2;
		}
		originalInputs.horizontalAxis *= SokuLib::getBattleMgr().leftCharacterManager.objectBase.direction;
		return;
	}
	if (this->_playComboAfterIntro || this->_finished || this->_firstFirst) {
		memset(&originalInputs, 0, sizeof(originalInputs));
		return;
	}
}

SokuLib::KeyInput ComboTrialEditor::getDummyInputs()
{
	return {0, this->_crouching - (this->_jump && this->_waitCounter < 30), 0, 0, 0, 0, 0, 0};
}

SokuLib::Action ComboTrialEditor::_getMoveAction(SokuLib::Character chr, std::string &name)
{
	auto error = false;

	try {
		auto act = actionsFromStr.at(name);
		auto pos = name.find("sc2");

		if (act >= SokuLib::ACTION_DEFAULT_SKILL1_B && act <= SokuLib::ACTION_ALT2_SKILL5_AIR_C) {
			auto input = characterSkills[chr].at(name[name.size() - 2] - '1');
			auto move = ((act - 500) / 20) * 3 + ((act - 500) % 20 / 5);
			auto skillName = characterCards[chr][100 + (move % 3) * characterSkills[chr].size() + move / 3].first;

			name = input + name.back() + " (" + (skillName.empty() ? "Unknown skill" : skillName) + ")";
		}
		try {
			if (pos != std::string::npos) {
				auto scId = std::stoul(name.substr(pos + 2, 3));
				auto &entry = characterCards[chr].at(scId);

				name = name.substr(0, pos) + std::to_string(entry.second) + "SC (" + entry.first + ")";
			}
		} catch (...) {
			printf("%u %s %s\n", name.size(), name.c_str(), name.substr(pos + 2, 3).c_str());
			error = true;
			if (error) {
				assert(false);
				throw;
			}
		}
		return act;
	} catch (...) {
		if (error)
			throw;
	}

	int start = name[0] == 'j';
	int realStart = (name[start] == 'a' ? 2 : 1) + start;
	auto move = name.substr(realStart, name.size() - realStart - 1);
	auto &inputs = characterSkills[chr];
	auto it = std::find(inputs.begin(), inputs.end(), move);

	if (it == inputs.end())
		throw std::exception();
	try {
		auto act = actionsFromStr.at(name.substr(0, realStart) + "skill" + std::to_string(it - inputs.begin() + 1) + name[name.size() - 1]);
		auto moveId = ((act - 500) / 20) * 3 + ((act - 500) % 20 / 5);
		auto skillName = characterCards[chr][100 + (moveId % 3) * characterSkills[chr].size() + moveId / 3].first;

		name = name.substr(realStart, name.size() - realStart - 1) + name.back() + " (" + (skillName.empty() ? "Unknown skill" : skillName) + ")";
		return act;
	} catch (std::exception &e) {
		printf("%s\n", (name.substr(0, realStart) + "skill" + std::to_string(it - inputs.begin() + 1) + name.back()).c_str());
		assert(false);
		throw;
	}
}

void ComboTrialEditor::onMenuClosed(MenuAction action)
{
	switch (action) {
	case RETRY:
		this->_playingIntro = true;
		this->_actionCounter = this->_exceptedActions.size();
		break;
	case GO_TO_NEXT_TRIAL:
	case RETURN_TO_TRIAL_SELECT:
		this->_next = SokuLib::SCENE_SELECT;
		break;
	case RETURN_TO_TITLE_SCREEN:
		this->_next = SokuLib::SCENE_TITLE;
		break;
	}
}

SokuLib::Scene ComboTrialEditor::getNextScene()
{
	return this->_next;
}

void ComboTrialEditor::_initVanillaGame()
{
	auto &battleMgr = SokuLib::getBattleMgr();

	battleMgr.leftCharacterManager.combo.limit = 0;
	battleMgr.leftCharacterManager.combo.damages = 0;
	battleMgr.leftCharacterManager.combo.nbHits = 0;
	battleMgr.leftCharacterManager.combo.rate = 0;
	battleMgr.leftCharacterManager.objectBase.hp = 10000;
	battleMgr.leftCharacterManager.currentSpirit = 1000;
	battleMgr.leftCharacterManager.maxSpirit = 1000;
	battleMgr.rightCharacterManager.objectBase.direction = SokuLib::RIGHT;
	battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_IDLE;
	battleMgr.leftCharacterManager.objectBase.animate();
	battleMgr.leftCharacterManager.objectBase.position.x = 480;
	battleMgr.leftCharacterManager.objectBase.position.y = 0;
	battleMgr.leftCharacterManager.objectBase.speed.x = 0;
	battleMgr.leftCharacterManager.objectBase.speed.y = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.xRotation = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.yRotation = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.zRotation = 0;
	battleMgr.leftCharacterManager.objectBase.renderInfos.scale.x = 1;
	battleMgr.leftCharacterManager.objectBase.renderInfos.scale.y = 1;
	if (SokuLib::leftChar == SokuLib::CHARACTER_SANAE) {
		battleMgr.leftCharacterManager.suwakoTimeLeft = 0;
		battleMgr.leftCharacterManager.kanakoTimeLeft = 0;
	}

	battleMgr.rightCharacterManager.objectBase.hp = 10000;
	battleMgr.rightCharacterManager.currentSpirit = 1000;
	battleMgr.rightCharacterManager.maxSpirit = 1000;
	battleMgr.rightCharacterManager.objectBase.renderInfos.xRotation = 0;
	battleMgr.rightCharacterManager.objectBase.renderInfos.yRotation = 0;
	battleMgr.rightCharacterManager.objectBase.renderInfos.zRotation = 0;
	battleMgr.rightCharacterManager.objectBase.action = SokuLib::ACTION_IDLE;
	battleMgr.rightCharacterManager.objectBase.animate();
	battleMgr.rightCharacterManager.objectBase.position.x = 800;
	battleMgr.rightCharacterManager.objectBase.position.y = 0;
	battleMgr.rightCharacterManager.objectBase.speed.x = 0;
	battleMgr.rightCharacterManager.objectBase.speed.y = 0;
	battleMgr.rightCharacterManager.objectBase.direction = SokuLib::LEFT;

	SokuLib::camera.translate = {-320, 420};
	SokuLib::camera.backgroundTranslate = {640, 0};
	SokuLib::camera.scale = 1;

	SokuLib::activeWeather = SokuLib::WEATHER_CLEAR;
	SokuLib::weatherCounter = 0;
}

int ComboTrialEditor::pauseOnUpdate()
{
	if (this->_playingIntro) {
		this->_initGameStart();
		if (this->_comboPageDisplayed)
			return false;
	}
	if (this->_comboPageDisplayed)
		this->_comboPageDisplayed = false;
	if (this->_recordingCombo) {
		this->_comboPageDisplayed = true;
		SokuLib::playSEWaveBuffer(0x28);
		return false;
	}
	if (this->_managingDolls) {
		this->_managingDolls = false;
		SokuLib::camera = this->_oldCamera;
	}
	if (this->_changingPlayerPos || this->_changingDummyPos) {
		this->_changingPlayerPos = false;
		this->_changingDummyPos = false;
		this->_dummyStartPos = this->_dummyStartPosTmp;
		SokuLib::getBattleMgr().leftCharacterManager.objectBase.position.x = this->_playerStartPos;
	}
	if (this->_selectingCharacters) {
		if (SokuLib::checkKeyOneshot(DIK_ESCAPE, false, false, false) || SokuLib::inputMgrs.input.b == 1) {
			SokuLib::playSEWaveBuffer(0x29);
			this->_selectingCharacters = false;
			return true;
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			auto str = orderChrs[this->_chrCursorPos];
			auto it = std::find_if(
				chrs.begin(),
				chrs.end(),
				[str](const std::pair<unsigned, std::string> &s) {
					return s.second == str;
				}
			);

			this->_needReload = true;
			SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
			SokuLib::playSEWaveBuffer(0x28);
			this->_selectingCharacters = false;
			*this->_characterEdit = static_cast<SokuLib::Character>(it->first);
			return true;
		}
		this->_selectingCharacterUpdate();
		return true;
	}

	if (this->_selectingStage) {
		if (SokuLib::checkKeyOneshot(DIK_ESCAPE, false, false, false) || SokuLib::inputMgrs.input.b == 1) {
			SokuLib::playSEWaveBuffer(0x29);
			this->_selectingStage = false;
			return true;
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			if (this->_stageCursor == ComboTrialEditor::_stagesIds.size() - 1) {
				auto id = InputBox("Enter stage id", "New stage id", std::to_string((int)*(char *)0x899D0C));

				try {
					auto i = std::stoul(id);

					if (i > 255)
						throw std::out_of_range("");
					*(unsigned char *)0x899D0C = i;
				} catch (std::invalid_argument &) {
					SokuLib::playSEWaveBuffer(0x29);
					return true;
				} catch (std::out_of_range &) {
					SokuLib::playSEWaveBuffer(0x29);
					return true;
				}
			} else
				*(char *)0x899D0C = ComboTrialEditor::_stagesIds[this->_stageCursor];
			SokuLib::playSEWaveBuffer(0x28);
			this->_needReload = true;
			this->_selectingStage = false;
			return true;
		}
		if (std::abs(SokuLib::inputMgrs.input.verticalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.verticalAxis) > 36 && SokuLib::inputMgrs.input.verticalAxis % 6 == 0)) {
			this->_stageCursor += ComboTrialEditor::_stagesIds.size() + std::copysign(1, SokuLib::inputMgrs.input.verticalAxis);
			this->_stageCursor %= ComboTrialEditor::_stagesIds.size();
			this->_setupStageSprites();
			SokuLib::playSEWaveBuffer(0x27);
		}
		return true;
	}

	if (this->_selectingMusic) {
		if (SokuLib::checkKeyOneshot(DIK_ESCAPE, false, false, false) || SokuLib::inputMgrs.input.b == 1) {
			SokuLib::playSEWaveBuffer(0x29);
			this->_selectingMusic = false;
			return true;
		}
		if (SokuLib::inputMgrs.input.a == 1) {
			if (this->_musicCursor == this->_musics.size() - 1) {
				auto music = InputBox("Enter music path", "New music", this->_musicReal);

				if (music.empty())
					return SokuLib::playSEWaveBuffer(0x29), true;

				auto musicLStart = InputBox("Enter music loop start position (in seconds)", "Loop start", std::to_string(this->_loopStart));

				if (music.empty())
					return SokuLib::playSEWaveBuffer(0x29), true;

				auto musicLEnd = InputBox("Enter music loop end position (in seconds)", "Loop end", std::to_string(this->_loopEnd));

				if (musicLEnd.empty())
					return SokuLib::playSEWaveBuffer(0x29), true;

				try {
					std::stod(musicLEnd);
					this->_loopStart = std::stod(musicLStart);
					this->_loopEnd = std::stod(musicLEnd);
				} catch (std::invalid_argument &) {
					SokuLib::playSEWaveBuffer(0x29);
					return true;
				} catch (std::out_of_range &) {
					SokuLib::playSEWaveBuffer(0x29);
					return true;
				}
				this->_music = this->_musicReal = music;
				for (auto pos = this->_music.find("{{pack_path}}"); pos != std::string::npos; pos = this->_music.find("{{pack_path}}"))
					this->_music.replace(pos, strlen("{{pack_path}}"), this->_folder);
			} else {
				this->_music = this->_musicReal = std::get<1>(this->_musics[this->_musicCursor]);
				this->_loopStart = 0;
				this->_loopEnd = 0;
			}
			this->_playBGM();
			SokuLib::playSEWaveBuffer(0x28);
		}
		if (std::abs(SokuLib::inputMgrs.input.verticalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.verticalAxis) > 36 && SokuLib::inputMgrs.input.verticalAxis % 6 == 0)) {
			if (SokuLib::inputMgrs.input.verticalAxis < 0 && this->_musicCursor <= 1) {
				while (this->_musicCursor < this->_musics.size() - 2)
					this->_musicCursor += 2;
				if (this->_musics.size() > 38)
					this->_musicTop = (this->_musicCursor - 36) - (this->_musicCursor % 2);
			} else if (SokuLib::inputMgrs.input.verticalAxis > 0 && this->_musicCursor >= this->_musics.size() - 2) {
				while (this->_musicCursor > 1)
					this->_musicCursor -= 2;
				this->_musicTop = 0;
			} else
				this->_musicCursor += std::copysign(2, SokuLib::inputMgrs.input.verticalAxis);
			while (this->_musicTop > this->_musicCursor)
				this->_musicTop -= 2;
			while (this->_musicTop + 38 <= this->_musicCursor)
				this->_musicTop += 2;
			SokuLib::playSEWaveBuffer(0x27);
		}
		if (std::abs(SokuLib::inputMgrs.input.horizontalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.horizontalAxis) > 36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0)) {
			if (this->_musicCursor != this->_musics.size() - 1 || this->_musics.size() % 2 == 0)
				this->_musicCursor ^= 1;
			SokuLib::playSEWaveBuffer(0x27);
		}
		return true;
	}

	if (SokuLib::checkKeyOneshot(DIK_ESCAPE, false, false, false) || SokuLib::inputMgrs.input.b == 1) {
		SokuLib::playSEWaveBuffer(0x29);
		if (!this->_selectedSubcategory) {
			reloadRequest = this->_needReload;
			return this->_menuCursorPos = 0;
		}
		this->_menuCursorPos = this->_selectedSubcategory;
		this->_selectedSubcategory = 0;
		return true;
	}
	if (SokuLib::inputMgrs.input.a == 1) {
		if (this->_selectedSubcategory) {
			if ((this->*ComboTrialEditor::callbacks[this->_selectedSubcategory][this->_menuCursorPos])())
				return true;
			reloadRequest = this->_needReload;
			return false;
		}
		if (ComboTrialEditor::callbacks[this->_menuCursorPos].size() <= 1) {
			if ((this->*ComboTrialEditor::callbacks[this->_menuCursorPos][0])())
				return true;
			reloadRequest = this->_needReload;
			return false;
		}
		this->_selectedSubcategory = this->_menuCursorPos;
		this->_menuCursorPos = 0;
		SokuLib::playSEWaveBuffer(0x28);
	}

	if (std::abs(SokuLib::inputMgrs.input.verticalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.verticalAxis) > 36 && SokuLib::inputMgrs.input.verticalAxis % 6 == 0)) {
		if (this->_selectedSubcategory) {
			this->_menuCursorPos += ComboTrialEditor::callbacks[this->_selectedSubcategory].size() + std::copysign(1, SokuLib::inputMgrs.input.verticalAxis);
			this->_menuCursorPos %= ComboTrialEditor::callbacks[this->_selectedSubcategory].size();
		} else {
			this->_menuCursorPos += 8 + std::copysign(1, SokuLib::inputMgrs.input.verticalAxis);
			this->_menuCursorPos %= 8;
		}
		SokuLib::playSEWaveBuffer(0x27);
	}

	if (std::abs(SokuLib::inputMgrs.input.horizontalAxis) == 1 || (std::abs(SokuLib::inputMgrs.input.horizontalAxis) > 36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0)) {
		if (this->_selectedSubcategory == 3) {
			if (this->_menuCursorPos == 6) {
				this->_uniformCardCost = static_cast<SokuLib::Weather>(static_cast<int>(this->_uniformCardCost + 6 + std::copysign(1, SokuLib::inputMgrs.input.horizontalAxis)) % 6);
				this->_initGameStart();
				SokuLib::playSEWaveBuffer(0x27);
			}
		} else if (this->_selectedSubcategory == 4) {
			if (this->_menuCursorPos == 2) {
				this->_weather = static_cast<SokuLib::Weather>(static_cast<int>(this->_weather + 22 + std::copysign(1, SokuLib::inputMgrs.input.horizontalAxis)) % 22);
				SokuLib::playSEWaveBuffer(0x27);
			}
		}
	}
	return true;
}

int ComboTrialEditor::pauseOnRender() const
{
	if (this->_comboPageDisplayed)
		return true;
	if (this->_selectedSubcategory)
		displaySokuCursor({
			61,
			static_cast<int>(129 + 32 * this->_selectedSubcategory)
		}, {256, 16});
	displaySokuCursor({
		this->_selectedSubcategory ? 324 : 61,
		static_cast<int>(129 + 32 * this->_menuCursorPos)
	}, {256, 16});
	this->_pause.draw();
	this->_trialEditorMockup.draw();
	if (this->_selectedSubcategory)
		(&this->_trialEditorMockup)[this->_selectedSubcategory].draw();
	(this->*ComboTrialEditor::renderCallbacks[this->_selectedSubcategory])();
	if (this->_selectingCharacters)
		this->_selectingCharacterRender();
	if (this->_selectingStage) {
		auto &last1 = this->_stagesSprites.at(ComboTrialEditor::_stagesIds[this->_stageCursor <= 1 ? ComboTrialEditor::_stagesIds.size() - (2 - this->_stageCursor) : this->_stageCursor - 2]);
		auto &last = this->_stagesSprites.at(ComboTrialEditor::_stagesIds[this->_stageCursor == 0 ? ComboTrialEditor::_stagesIds.size() - 1 : this->_stageCursor - 1]);
		auto &current = this->_stagesSprites.at(ComboTrialEditor::_stagesIds[this->_stageCursor]);
		auto &next = this->_stagesSprites.at(ComboTrialEditor::_stagesIds[this->_stageCursor == ComboTrialEditor::_stagesIds.size() - 1 ? 0 : this->_stageCursor + 1]);
		auto &next1 = this->_stagesSprites.at(ComboTrialEditor::_stagesIds[this->_stageCursor >= ComboTrialEditor::_stagesIds.size() - 2 ? (ComboTrialEditor::_stagesIds.size() - this->_stageCursor) : this->_stageCursor + 2]);

		editSeatEmpty.draw();
		last1.first->draw();
		last1.second->draw();
		last.first->draw();
		last.second->draw();
		next.first->draw();
		next.second->draw();
		next1.first->draw();
		next1.second->draw();
		this->_stageRect.draw();
		current.first->draw();
		current.second->draw();
	}
	if (this->_selectingMusic) {
		editSeatEmpty.draw();
		for (int j = this->_musicTop, i = 0; j < this->_musics.size() && i < 38; j++, i++) {
			auto &sprite = std::get<2>(this->_musics[j]);
			SokuLib::Vector2i pos{
				static_cast<int>(i % 2 == 0 ? editSeatEmpty.getPosition().x + 10 : editSeatEmpty.getPosition().x + editSeatEmpty.getSize().x - 210),
				editSeatEmpty.getPosition().y + 10 + i / 2 * 20
			};

			sprite->setPosition(pos);
			if (j == this->_musicCursor)
				displaySokuCursor(pos, {256, 16});
			sprite->draw();
		}
		if (this->_musicTop) {
			upArrow.setPosition({304, editSeatEmpty.getPosition().y + 5});
			upArrow.setSize({32, 32});
			upArrow.draw();
		}
		if (this->_musicTop + 38 < this->_musics.size()) {
			downArrow.setPosition({304, static_cast<int>(editSeatEmpty.getPosition().y + editSeatEmpty.getSize().y - 37)});
			downArrow.setSize({32, 32});
			downArrow.draw();
		}
	}
	return true;
}

const std::vector<bool (ComboTrialEditor::*)()> ComboTrialEditor::callbacks[] = {
	{&ComboTrialEditor::returnToGame},
	{	//Player
		&ComboTrialEditor::setPlayerCharacter,//Set character
		&ComboTrialEditor::setPlayerPosition, //Set position
		&ComboTrialEditor::setPlayerDeck,     //Set deck
		&ComboTrialEditor::setPlayerSkills,   //Set skills
		&ComboTrialEditor::setPlayerHand,     //Set hand
		&ComboTrialEditor::setPlayerWeather,  //Affected by weather
		&ComboTrialEditor::setPlayerDolls     //Manage dolls
	},
	{	//Dummy
		&ComboTrialEditor::setDummyCharacter,//Set character
		&ComboTrialEditor::setDummyPosition, //Set position
		&ComboTrialEditor::setDummyDeck,     //Set deck
		&ComboTrialEditor::setDummyCrouch,   //Crouch
		&ComboTrialEditor::setDummyJump,     //Jump
		&ComboTrialEditor::setDummyWeather   //Affected by weather
	},
	{	//System
		&ComboTrialEditor::setCRankRequ,    //C rank
		&ComboTrialEditor::setBRankRequ,    //B rank
		&ComboTrialEditor::setARankRequ,    //A rank
		&ComboTrialEditor::setSRankRequ,    //S rank
		&ComboTrialEditor::setCounterHit,   //Counter hit
		&ComboTrialEditor::setLimitDisabled,//Limit disable
		&ComboTrialEditor::setCardCosts,    //Card cost
		&ComboTrialEditor::setCombo         //Combo
	},
	{	//Misc
		&ComboTrialEditor::setStage,    //Stage
		&ComboTrialEditor::setMusic,    //Music
		&ComboTrialEditor::setWeather,  //Weather
		&ComboTrialEditor::setFailTimer,//Fail timer
		&ComboTrialEditor::setIntro,    //Intro
		&ComboTrialEditor::setOutro,    //Outro
		&ComboTrialEditor::playIntro,   //Play intro
		&ComboTrialEditor::playOutro,   //Play outro
		&ComboTrialEditor::playPreview  //Play preview
	},
	{&ComboTrialEditor::saveReturnToCharSelect},//Save
	{&ComboTrialEditor::returnToCharSelect},
	{&ComboTrialEditor::returnToTitleScreen}
};
void (ComboTrialEditor::* const ComboTrialEditor::renderCallbacks[])() const = {
	&ComboTrialEditor::noRender,
	&ComboTrialEditor::playerRender,
	&ComboTrialEditor::dummyRender,
	&ComboTrialEditor::systemRender,
	&ComboTrialEditor::miscRender
};

bool ComboTrialEditor::notImplemented()
{
	SokuLib::playSEWaveBuffer(0x29);
	MessageBox(SokuLib::window, "Not implemented", "Not implemented", MB_ICONINFORMATION);
	return true;
}

bool ComboTrialEditor::returnToGame()
{
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::setPlayerCharacter()
{
	this->_selectingCharacters = true;
	this->_characterEdit = &SokuLib::leftChar;
	this->_chrCursorPos = std::find(orderChrs.begin(), orderChrs.end(), chrs[SokuLib::leftChar]) - orderChrs.begin();
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setPlayerPosition()
{
	this->_changingPlayerPos = true;
	this->_dummyStartPosTmp = this->_dummyStartPos;
	this->_fakePlayerPos = SokuLib::getBattleMgr().leftCharacterManager.objectBase.position.x = this->_playerStartPos;
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::setPlayerDeck()
{
	int ret;
	int width;
	int height;
	int count = 0;

	if (SokuLib::textureMgr.loadTexture(&ret, ("data/stand/" + chrs[SokuLib::leftChar] + ".bmp").c_str(), &width, &height)) {
		this->_characterSprite.init(ret, 0, 0, width, height);
		for (auto &vertice: this->_characterSprite.vertices)
			vertice.color = SokuLib::Color{0x80, 0x80, 0x80, 0xFF};
	}

	this->_fakeProfile = SokuLib::profile1;
	SokuLib::textureMgr.createTextTexture(&ret, "Temporary Profile", *reinterpret_cast<SokuLib::SWRFont *>(0x897020), 300, 50, &width, &height);
	this->_fakeProfile.sprite.init(ret, 0, 0, width, height);
	this->_fakeProfile.sprite.vertices[2].color = SokuLib::Color::Yellow;
	this->_fakeProfile.sprite.vertices[3].color = SokuLib::Color::Yellow;
	this->_deckEditMenu = ((SokuLib::ProfileDeckEdit *(__thiscall *)(void *, SokuLib::Profile &, SokuLib::Character, SokuLib::Sprite &))(*(unsigned *)0x44D52A + 0x44D52E))(
		SokuLib::NewFct(0x840),
		this->_fakeProfile,
		SokuLib::leftChar,
		this->_characterSprite
	);

	this->_deckEditMenu->editedDeck->clear();
	for (int i = 0; i < SokuLib::leftPlayerInfo.effectiveDeck.size; i++) {
		auto card = SokuLib::leftPlayerInfo.effectiveDeck[i];
		auto iter = this->_deckEditMenu->editedDeck->find(card);

		if (iter == this->_deckEditMenu->editedDeck->end())
			(*this->_deckEditMenu->editedDeck)[card] = 1;
		else
			iter->second++;
		count++;
	}
	this->_deckEditMenu->displayedNumberOfCards = count;

	this->onDeckSaved = &ComboTrialEditor::_copyDeckToPlayerDeck;
	SokuLib::playSEWaveBuffer(0x28);
	SokuLib::activateMenu(this->_deckEditMenu);
	return true;
}

bool ComboTrialEditor::setPlayerSkills()
{
	int ret;
	int width;
	int height;
	int count = 0;
	auto skills = this->_getSkills();

	if (SokuLib::textureMgr.loadTexture(&ret, ("data/stand/" + chrs[SokuLib::leftChar] + ".bmp").c_str(), &width, &height)) {
		this->_characterSprite.init(ret, 0, 0, width, height);
		for (auto &vertice: this->_characterSprite.vertices)
			vertice.color = SokuLib::Color{0x80, 0x80, 0x80, 0xFF};
	}

	this->_fakeProfile = SokuLib::profile1;
	SokuLib::textureMgr.createTextTexture(&ret, "Temporary Profile", *reinterpret_cast<SokuLib::SWRFont *>(0x897020), 300, 50, &width, &height);
	this->_fakeProfile.sprite.init(ret, 0, 0, width, height);
	this->_fakeProfile.sprite.vertices[2].color = SokuLib::Color::Yellow;
	this->_fakeProfile.sprite.vertices[3].color = SokuLib::Color::Yellow;
	this->_deckEditMenu = ((SokuLib::ProfileDeckEdit *(__thiscall *)(void *, SokuLib::Profile &, SokuLib::Character, SokuLib::Sprite &))(*(unsigned *)0x44D52A + 0x44D52E))(
		SokuLib::NewFct(0x840),
		this->_fakeProfile,
		SokuLib::leftChar,
		this->_characterSprite
	);

	this->_deckEditMenu->editedDeck->clear();
	for (int i = 0; i < skills.size(); i++) {
		(*this->_deckEditMenu->editedDeck)[100 + i + skills[i][0] * skills.size()] = skills[i][1];
		count += skills[i][1];
	}
	this->_deckEditMenu->displayedNumberOfCards = count;

	this->onDeckSaved = &ComboTrialEditor::_copyDeckToPlayerSkills;
	SokuLib::playSEWaveBuffer(0x28);
	SokuLib::activateMenu(this->_deckEditMenu);
	return true;
}

bool ComboTrialEditor::setPlayerHand()
{
	int ret;
	int width;
	int height;
	int count = 0;

	if (SokuLib::textureMgr.loadTexture(&ret, ("data/stand/" + chrs[SokuLib::leftChar] + ".bmp").c_str(), &width, &height)) {
		this->_characterSprite.init(ret, 0, 0, width, height);
		for (auto &vertice: this->_characterSprite.vertices)
			vertice.color = SokuLib::Color{0x80, 0x80, 0x80, 0xFF};
	}

	this->_fakeProfile = SokuLib::profile1;
	SokuLib::textureMgr.createTextTexture(&ret, "Temporary Profile", *reinterpret_cast<SokuLib::SWRFont *>(0x897020), 300, 50, &width, &height);
	this->_fakeProfile.sprite.init(ret, 0, 0, width, height);
	this->_fakeProfile.sprite.vertices[2].color = SokuLib::Color::Yellow;
	this->_fakeProfile.sprite.vertices[3].color = SokuLib::Color::Yellow;
	this->_deckEditMenu = ((SokuLib::ProfileDeckEdit *(__thiscall *)(void *, SokuLib::Profile &, SokuLib::Character, SokuLib::Sprite &))(*(unsigned *)0x44D52A + 0x44D52E))(
		SokuLib::NewFct(0x840),
		this->_fakeProfile,
		SokuLib::leftChar,
		this->_characterSprite
	);

	this->_deckEditMenu->editedDeck->clear();
	for (auto card : this->_hand) {
		auto iter = this->_deckEditMenu->editedDeck->find(card);

		if (iter == this->_deckEditMenu->editedDeck->end())
			(*this->_deckEditMenu->editedDeck)[card] = 1;
		else
			iter->second++;
		count++;
	}
	this->_deckEditMenu->displayedNumberOfCards = count;

	this->onDeckSaved = &ComboTrialEditor::_copyDeckToPlayerHand;
	SokuLib::playSEWaveBuffer(0x28);
	SokuLib::activateMenu(this->_deckEditMenu);
	return true;
}

bool ComboTrialEditor::setPlayerWeather()
{
	this->_leftWeather = !this->_leftWeather;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setPlayerDolls()
{
	if (SokuLib::leftChar != SokuLib::CHARACTER_ALICE) {
		SokuLib::playSEWaveBuffer(0x29);
		return true;
	}
	SokuLib::playSEWaveBuffer(0x28);
	this->_dollCursorPos = 0;
	this->_managingDolls = true;
	this->_initGameStart();
	return false;
}

bool ComboTrialEditor::setDummyCharacter()
{
	this->_selectingCharacters = true;
	this->_characterEdit = &SokuLib::rightChar;
	this->_chrCursorPos = std::find(orderChrs.begin(), orderChrs.end(), chrs[SokuLib::rightChar]) - orderChrs.begin();
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setDummyPosition()
{
	this->_changingDummyPos = true;
	this->_dummyStartPosTmp = this->_dummyStartPos;
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.position.x = this->_playerStartPos;
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::setDummyDeck()
{
	int ret;
	int width;
	int height;
	int count = 0;
	auto old = SokuLib::profile1.sprite.dxHandle;

	if (SokuLib::textureMgr.loadTexture(&ret, ("data/stand/" + chrs[SokuLib::rightChar] + ".bmp").c_str(), &width, &height)) {
		this->_characterSprite.init(ret, 0, 0, width, height);
		for (auto &vertice: this->_characterSprite.vertices)
			vertice.color = SokuLib::Color{0x80, 0x80, 0x80, 0xFF};
	}

	this->_fakeProfile = SokuLib::profile1;
	SokuLib::textureMgr.createTextTexture(&ret, "Temporary Profile", *reinterpret_cast<SokuLib::SWRFont *>(0x897020), 300, 50, &width, &height);
	this->_fakeProfile.sprite.init(ret, 0, 0, width, height);
	this->_fakeProfile.sprite.vertices[2].color = SokuLib::Color::Yellow;
	this->_fakeProfile.sprite.vertices[3].color = SokuLib::Color::Yellow;
	this->_deckEditMenu = ((SokuLib::ProfileDeckEdit *(__thiscall *)(void *, SokuLib::Profile &, SokuLib::Character, SokuLib::Sprite &))(*(unsigned *)0x44D52A + 0x44D52E))(
		SokuLib::NewFct(0x840),
		this->_fakeProfile,
		SokuLib::rightChar,
		this->_characterSprite
	);

	this->_deckEditMenu->editedDeck->clear();
	for (int i = 0; i < SokuLib::rightPlayerInfo.effectiveDeck.size; i++) {
		auto card = SokuLib::rightPlayerInfo.effectiveDeck[i];
		auto iter = this->_deckEditMenu->editedDeck->find(card);

		if (iter == this->_deckEditMenu->editedDeck->end())
			(*this->_deckEditMenu->editedDeck)[card] = 1;
		else
			iter->second++;
		count++;
	}
	this->_deckEditMenu->displayedNumberOfCards = count;

	this->onDeckSaved = &ComboTrialEditor::_copyDeckToDummyDeck;
	SokuLib::playSEWaveBuffer(0x28);
	SokuLib::activateMenu(this->_deckEditMenu);
	return true;
}

bool ComboTrialEditor::setDummyCrouch()
{
	if (this->_dummyStartPos.y)
		return SokuLib::playSEWaveBuffer(0x29), true;
	this->_crouching = !this->_crouching;
	this->_jump = false;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setDummyJump()
{
	if (this->_dummyStartPos.y)
		return SokuLib::playSEWaveBuffer(0x29), true;
	this->_jump = !this->_jump;
	this->_crouching = false;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setDummyWeather()
{
	this->_rightWeather = !this->_rightWeather;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setCRankRequ()
{
	return notImplemented();
}

bool ComboTrialEditor::setBRankRequ()
{
	return notImplemented();
}

bool ComboTrialEditor::setARankRequ()
{
	return notImplemented();
}

bool ComboTrialEditor::setSRankRequ()
{
	return notImplemented();
}

bool ComboTrialEditor::setCounterHit()
{
	this->_counterHit = !this->_counterHit;
	if (this->_counterHit)
		applyCounterHitOnlyPatch();
	else
		removeCounterHitOnlyPatch();
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setLimitDisabled()
{
	this->_disableLimit = !this->_disableLimit;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setCardCosts()
{
	return true;
}

bool ComboTrialEditor::setCombo()
{
	this->_comboPageDisplayed = true;
	this->_comboEditCursor = 0;
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::setStage()
{
	auto it = std::find(ComboTrialEditor::_stagesIds.begin(), ComboTrialEditor::_stagesIds.end(), *(char *)0x899D0C);

	this->_stageCursor = ((it == ComboTrialEditor::_stagesIds.end()) ? (ComboTrialEditor::_stagesIds.size() - 1) : (it - ComboTrialEditor::_stagesIds.begin()));
	this->_setupStageSprites();
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setMusic()
{
	auto it = std::find_if(this->_musics.begin(), this->_musics.end(), [this](const std::tuple<std::string, std::string, std::unique_ptr<SokuLib::DrawUtils::Sprite>> &t){
		return this->_musicReal == std::get<1>(t);
	});

	SokuLib::playSEWaveBuffer(0x28);
	this->_selectingMusic = true;
	this->_musicCursor = it == this->_musics.end() ? this->_musics.size() - 1 : it - this->_musics.begin();
	if (this->_musicCursor > 38)
		this->_musicTop = (this->_musicCursor - 36) - (this->_musicCursor % 2);
	else
		this->_musicTop = 0;
	return true;
}

bool ComboTrialEditor::setWeather()
{
	return true;
}

bool ComboTrialEditor::setFailTimer()
{
	return notImplemented();
}

bool ComboTrialEditor::setIntro()
{
	this->_introRelPath = InputBox("Enter new intro path.", "New intro", this->_introRelPath);
	this->_introPath = this->_folder + this->_introRelPath;
	this->_introSprite.texture.createFromText(this->_introRelPath.c_str(), defaultFont12, {250, 30});
	this->_introSprite.setPosition({379, 260});
	this->_introSprite.setSize(this->_introSprite.texture.getSize());
	this->_introSprite.rect.left = 0;
	this->_introSprite.rect.top = 0;
	this->_introSprite.rect.width = this->_introSprite.texture.getSize().x;
	this->_introSprite.rect.height = this->_introSprite.texture.getSize().y;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::setOutro()
{
	this->_outroRelPath = InputBox("Enter new outro path.", "New outro", this->_outroRelPath);
	this->_outroPath = this->_folder + this->_outroRelPath;
	this->_outroSprite.texture.createFromText(this->_outroRelPath.c_str(), defaultFont12, {250, 30});
	this->_outroSprite.setPosition({379, 294});
	this->_outroSprite.setSize(this->_outroSprite.texture.getSize());
	this->_outroSprite.rect.left = 0;
	this->_outroSprite.rect.top = 0;
	this->_outroSprite.rect.width = this->_outroSprite.texture.getSize().x;
	this->_outroSprite.rect.height = this->_outroSprite.texture.getSize().y;
	SokuLib::playSEWaveBuffer(0x28);
	return true;
}

bool ComboTrialEditor::playIntro()
{
	this->_introRequ++;
	this->_menuCursorPos = 0;
	this->_selectedSubcategory = 0;
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::playOutro()
{
	this->_outroRequ++;
	this->_menuCursorPos = 0;
	this->_selectedSubcategory = 0;
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::playPreview()
{
	this->_menuCursorPos = 0;
	this->_selectedSubcategory = 0;
	this->_playComboAfterIntro = true;
	this->_initGameStart();
	SokuLib::playSEWaveBuffer(0x28);
	return false;
}

bool ComboTrialEditor::saveReturnToCharSelect()
{
	if (!this->save())
		return SokuLib::playSEWaveBuffer(0x29), true;
	SokuLib::playSEWaveBuffer(0x28);
	this->_quit = true;
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
	this->_next = SokuLib::SCENE_SELECT;
	return false;
}

bool ComboTrialEditor::returnToCharSelect()
{
	SokuLib::playSEWaveBuffer(0x28);
	this->_quit = true;
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
	this->_next = SokuLib::SCENE_SELECT;
	return false;
}

bool ComboTrialEditor::returnToTitleScreen()
{
	SokuLib::playSEWaveBuffer(0x28);
	this->_quit = true;
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
	this->_next = SokuLib::SCENE_TITLE;
	return false;
}

void ComboTrialEditor::_setupStageSprites()
{
	auto &last1 = this->_stagesSprites[ComboTrialEditor::_stagesIds[this->_stageCursor <= 1 ? ComboTrialEditor::_stagesIds.size() - (2 - this->_stageCursor) : this->_stageCursor - 2]];
	auto &last  = this->_stagesSprites[ComboTrialEditor::_stagesIds[this->_stageCursor == 0 ? ComboTrialEditor::_stagesIds.size() - 1 : this->_stageCursor - 1]];
	auto &next  = this->_stagesSprites[ComboTrialEditor::_stagesIds[this->_stageCursor == ComboTrialEditor::_stagesIds.size() - 1 ? 0 : this->_stageCursor + 1]];
	auto &next1 = this->_stagesSprites[ComboTrialEditor::_stagesIds[this->_stageCursor >= ComboTrialEditor::_stagesIds.size() - 2 ? (ComboTrialEditor::_stagesIds.size() - this->_stageCursor) : this->_stageCursor + 2]];
	auto &current = this->_stagesSprites[ComboTrialEditor::_stagesIds[this->_stageCursor]];

	this->_selectingStage = true;
	last1.first->setPosition({144, 208 - 128});
	last1.second->setPosition({144, 208 - 128});
	last1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0x00;
	last1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0x00;
	last1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0xA0;
	last1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0xA0;
	last1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0x00;
	last1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0x00;
	last1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0xA0;
	last1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0xA0;
	last.first->setPosition({144, 208 - 64});
	last.second->setPosition({144, 208 - 64});
	last.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0xA0;
	last.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0xA0;
	last.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0xFF;
	last.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0xFF;
	last.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0xA0;
	last.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0xA0;
	last.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0xFF;
	last.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0xFF;
	current.first->setPosition({144, 208});
	current.second->setPosition({144, 208});
	for (auto &tint : current.first->fillColors)
		tint.a = 0xFF;
	for (auto &tint : current.second->fillColors)
		tint.a = 0xFF;
	next.first->setPosition({144, 208 + 64});
	next.second->setPosition({144, 208 + 64});
	next.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0xFF;
	next.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0xFF;
	next.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0xA0;
	next.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0xA0;
	next.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0xFF;
	next.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0xFF;
	next.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0xA0;
	next.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0xA0;
	next1.first->setPosition({144, 208 + 128});
	next1.second->setPosition({144, 208 + 128});
	next1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0xA0;
	next1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0xA0;
	next1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0x00;
	next1.first->fillColors[ SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0x00;
	next1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_LEFT_CORNER    ].a = 0xA0;
	next1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_TOP_RIGHT_CORNER   ].a = 0xA0;
	next1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_RIGHT_CORNER].a = 0x00;
	next1.second->fillColors[SokuLib::DrawUtils::GradiantRect::RECT_BOTTOM_LEFT_CORNER ].a = 0x00;
}

bool ComboTrialEditor::save() const
{
	try {
		auto itls = swrCharacters.find(SokuLib::leftChar);
		auto itrs = swrCharacters.find(SokuLib::rightChar);
		auto itlv = validCharacters.find(SokuLib::leftChar);
		auto itrv = validCharacters.find(SokuLib::rightChar);
		std::vector<unsigned short> ldeck;
		std::vector<unsigned short> rdeck;

		ldeck.reserve(SokuLib::leftPlayerInfo.effectiveDeck.size);
		for (int i = 0; i < SokuLib::leftPlayerInfo.effectiveDeck.size; i++)
			ldeck.push_back(SokuLib::leftPlayerInfo.effectiveDeck[i]);
		rdeck.reserve(SokuLib::rightPlayerInfo.effectiveDeck.size);
		for (int i = 0; i < SokuLib::rightPlayerInfo.effectiveDeck.size; i++)
			rdeck.push_back(SokuLib::rightPlayerInfo.effectiveDeck[i]);

		nlohmann::json json{
			{ "type", "combo" },
			{ "player", {
				{ "pos",                this->_playerStartPos },
				{ "character",          itls == swrCharacters.end() ? itlv->second : itls->second},
				{ "palette",            SokuLib::leftPlayerInfo.palette },
				{ "deck",               ldeck }
			}},
			{ "dummy", {
				{ "pos",{
					{ "x", this->_dummyStartPos.x },
					{ "y", this->_dummyStartPos.y }
				}},
				{ "character",          itrs == swrCharacters.end() ? itrv->second : itrs->second},
				{ "palette",            SokuLib::rightPlayerInfo.palette },
				{ "deck",               rdeck },
			}},
			{ "stage", (int)*(char *)0x899D0C },
			{ "music", this->_musicReal },
			{ "expected", this->_transformComboToString() },
			{ "skills", this->_getSkills() },
			{ "score", this->_getScoresJson() }
		};

		if (this->_mpp)
			json["player"]["mpp"]                 = this->_mpp;
		if (this->_stones)
			json["player"]["stones"]              = this->_stones;
		if (this->_orerries)
			json["player"]["orerries"]            = this->_orerries;
		if (this->_tickTimer)
			json["player"]["tickTimer"]           = this->_tickTimer;
		if (this->_privateSquare)
			json["player"]["privateSquare"]       = this->_privateSquare;
		if (!this->_leftWeather)
			json["player"]["affected_by_weather"] = this->_leftWeather;

		if (!this->_rightWeather)
			json["dummy"]["affected_by_weather"]  = this->_rightWeather;
		if (this->_jump)
			json["dummy"]["jump"]  = this->_jump;
		if (this->_crouching)
			json["dummy"]["crouch"]  = this->_crouching;

		if (this->_loopStart)
			json["music_loop_start"] = this->_loopStart;
		if (this->_loopEnd)
			json["music_loop_end"] = this->_loopEnd;
		if (this->_counterHit)
			json["counter_hit"] = this->_counterHit;
		if (this->_uniformCardCost)
			json["uniform_card_cost"] = this->_uniformCardCost;
		if (this->_disableLimit)
			json["disable_limit"] = this->_disableLimit;
		if (this->_weather != SokuLib::WEATHER_CLEAR)
			json["weather"] = this->_weather;
		if (!this->_hand.empty())
			json["hand"] = this->_hand;
		if (!this->_introPath.empty())
			json["intro"] = this->_introRelPath;
		if (!this->_outroPath.empty())
			json["outro"] = this->_outroRelPath;
		if (SokuLib::leftChar == SokuLib::CHARACTER_ALICE && !this->_dolls.empty()) {
			nlohmann::json dolls = nlohmann::json::array();

			for (auto &doll : this->_dolls)
				dolls.push_back({
					{"dir", doll.dir},
					{"x",   doll.pos.x},
					{"y",   doll.pos.y}
				});
			json["dolls"] = dolls;
		}

		unlink((this->_path + ".backup").c_str());
		if (rename(this->_path.c_str(), (this->_path + ".backup").c_str()) != 0)
			throw std::invalid_argument("Cannot rename " + this->_path + " to " + this->_path + ".backup");

		std::ofstream stream{this->_path};

		if (stream.fail()) {
			rename((this->_path + ".backup").c_str(), this->_path.c_str());
			throw std::invalid_argument(this->_path + ": " + strerror(errno));
		}
		stream << json.dump(4);
		stream.close();
		return true;
	} catch (std::exception &e) {
		MessageBox(SokuLib::window, e.what(), "Saving error", MB_ICONERROR);
		return false;
	}
}

std::vector<std::array<unsigned int, 2>> ComboTrialEditor::_getSkills() const
{
	std::vector<std::array<unsigned int, 2>> result;
	auto size = characterSkills[SokuLib::leftChar].size();

	result.reserve(size);
	for (int i = 0; i < size; i++) {
		unsigned skill = 0;
		unsigned level = 0;

		for (int j = 0; j < 3; j++) {
			if (!this->_skills[i + j * size].notUsed) {
				skill = j;
				level = this->_skills[i + j * size].level;
			}
		}
		result.push_back({skill, level});
	}
	return result;
}

nlohmann::json ComboTrialEditor::_getScoresJson() const
{
	nlohmann::json result = nlohmann::json::array();

	result.push_back({
		{ "min_hits", this->_scores[0].hits },
		{ "min_damage", this->_scores[0].damage },
		{ "min_limit", this->_scores[0].minLimit }
	});
	for (int i = 1; i < 4; i++) {
		nlohmann::json tmp = nlohmann::json::object();

		if (this->_scores[i].hits != this->_scores[i - 1].hits)
			tmp["min_hits"] = this->_scores[i].hits;
		if (this->_scores[i].damage != this->_scores[i - 1].damage)
			tmp["min_damage"] = this->_scores[i].damage;
		if (this->_scores[i].minLimit != this->_scores[i - 1].minLimit)
			tmp["min_limit"] = this->_scores[i].minLimit;
		if (this->_scores[i].attempts != this->_scores[i - 1].attempts)
			tmp["max_attempts"] = this->_scores[i].attempts;
		result.push_back(tmp);
	}
	return result;
}

std::string ComboTrialEditor::_transformComboToString() const
{
	std::string str;
	bool start = true;

	for (auto &move : this->_exceptedActions) {
		if (!start)
			str += " ";
		start = false;
		if (move->optional)
			str += "!";
		for (int i = 0; i < move->actions.size(); i++) {
			auto action = move->actions[i];
			auto it = std::find_if(actionsFromStr.begin(), actionsFromStr.end(), [action](const std::pair<std::string, SokuLib::Action> &p1){
				return p1.second == action;
			});

			if (i != 0)
				str += "/";
			if (it == actionsFromStr.end())
				throw std::invalid_argument("Cannot find action string for action " + std::to_string(action));
			if (action == SokuLib::ACTION_FLY || (action >= SokuLib::ACTION_DEFAULT_SKILL1_B && action < SokuLib::ACTION_USING_SC_ID_200))
				str += move->actionsStr[i];
			else if (it->first == "44")
				str += "d4";
			else if (it->first == "66")
				str += "d6";
			else if (it->first == "1a" && SokuLib::leftChar != SokuLib::CHARACTER_YOUMU)
				str += "2a";
			else
				str += it->first;
		}
		if (move->chargeTime)
			str += "[" + std::to_string(move->chargeTime) + "]";
		if (move->delay)
			str += ":" + std::to_string(move->delay);
	}
	return str;
}

void ComboTrialEditor::_selectingCharacterUpdate()
{
	bool playSound = false;

	if (SokuLib::inputMgrs.input.verticalAxis == -1 || (SokuLib::inputMgrs.input.verticalAxis < -36 && SokuLib::inputMgrs.input.verticalAxis % 6 == 0)) {
		playSound = true;
		do {
			if (this->_chrCursorPos % 9 == 0)
				this->_chrCursorPos += 8;
			else
				this->_chrCursorPos -= 1;
		} while (this->_chrCursorPos >= orderChrs.size());
		if (SokuLib::inputMgrs.input.horizontalAxis == -1 || (SokuLib::inputMgrs.input.horizontalAxis < -36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0))  {
			if (this->_chrCursorPos < 9)
				while (this->_chrCursorPos + 9 < orderChrs.size())
					this->_chrCursorPos += 9;
			else
				this->_chrCursorPos -= 9;
		} else if (SokuLib::inputMgrs.input.horizontalAxis == 1 || (SokuLib::inputMgrs.input.horizontalAxis > 36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0)) {
			this->_chrCursorPos += 9;
			if (this->_chrCursorPos >= orderChrs.size())
				this->_chrCursorPos %= 9;
		}
	} else if (SokuLib::inputMgrs.input.verticalAxis == 1 || (SokuLib::inputMgrs.input.verticalAxis > 36 && SokuLib::inputMgrs.input.verticalAxis % 6 == 0)) {
		playSound = true;
		do {
			if (this->_chrCursorPos % 9 == 8)
				this->_chrCursorPos -= 8;
			else
				this->_chrCursorPos += 1;
		} while (this->_chrCursorPos >= orderChrs.size());
		if (SokuLib::inputMgrs.input.horizontalAxis == -1 || (SokuLib::inputMgrs.input.horizontalAxis < -36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0))  {
			if (this->_chrCursorPos < 9)
				while (this->_chrCursorPos + 9 < orderChrs.size())
					this->_chrCursorPos += 9;
			else
				this->_chrCursorPos -= 9;
		} else if (SokuLib::inputMgrs.input.horizontalAxis == 1 || (SokuLib::inputMgrs.input.horizontalAxis > 36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0)) {
			this->_chrCursorPos += 9;
			if (this->_chrCursorPos >= orderChrs.size())
				this->_chrCursorPos %= 9;
		}
	} else if (SokuLib::inputMgrs.input.horizontalAxis == -1 || (SokuLib::inputMgrs.input.horizontalAxis < -36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0)) {
		playSound = true;
		if (this->_chrCursorPos < 9)
			while (this->_chrCursorPos + 9 < orderChrs.size())
				this->_chrCursorPos += 9;
		else
			this->_chrCursorPos -= 9;
	} else if (SokuLib::inputMgrs.input.horizontalAxis == 1 || (SokuLib::inputMgrs.input.horizontalAxis > 36 && SokuLib::inputMgrs.input.horizontalAxis % 6 == 0)) {
		playSound = true;
		this->_chrCursorPos += 9;
		if (this->_chrCursorPos >= orderChrs.size())
			this->_chrCursorPos %= 9;
	}
	if (playSound)
		SokuLib::playSEWaveBuffer(0x27);
}

void ComboTrialEditor::_selectingCharacterRender() const
{
	int i = 0;

	editSeatEmpty.draw();
	for (auto &chr : chrs) {
		auto &sprite = *charactersFaces[i];

		if (this->_chrCursorPos == i)
			displaySokuCursor({
				127 + 100 * (i / 9),
				69 + 40 * (i % 9),
			}, {80, 16});
		sprite.draw();
		i++;
	}
}

void ComboTrialEditor::_openPause() const
{
	SokuLib::activateMenu(((void *(__thiscall *)(void *, SokuLib::BattleManager &))0x444F40)(SokuLib::NewFct(0x78), SokuLib::getBattleMgr()));
}

void ComboTrialEditor::noRender() const
{
}

void ComboTrialEditor::playerRender() const
{
	if (this->_leftWeather) {
		tickSprite.setPosition({304, 287});
		tickSprite.draw();
	}
}

void ComboTrialEditor::dummyRender() const
{
	if (this->_crouching) {
		tickSprite.setPosition({304, 222});
		tickSprite.draw();
	}
	if (this->_jump) {
		tickSprite.setPosition({304, 254});
		tickSprite.draw();
	}
	if (this->_rightWeather) {
		tickSprite.setPosition({304, 286});
		tickSprite.draw();
	}
}

void ComboTrialEditor::systemRender() const
{
	if (this->_counterHit) {
		tickSprite.setPosition({303, 254});
		tickSprite.draw();
	}
	if (this->_disableLimit) {
		tickSprite.setPosition({303, 286});
		tickSprite.draw();
	}
	this->_comboSprite.draw();
	if (this->_uniformCardCost == 0)
		this->_normal.draw();
	else
		this->_digits[this->_uniformCardCost % (sizeof(this->_digits) / sizeof(*this->_digits))].draw();
}

void ComboTrialEditor::miscRender() const
{
	this->_introSprite.draw();
	this->_outroSprite.draw();
	this->_weatherArrows.draw();
	if (this->_weather == SokuLib::WEATHER_TWILIGHT)
		this->_twilight.draw();
	else {
		this->_weathers.rect.top = (this->_weather + 1) % 22 * 24;
		this->_weathers.draw();
	}
}

bool ComboTrialEditor::_copyDeckToPlayerDeck()
{
	if (this->_deckEditMenu->displayedNumberOfCards != 20 && this->_deckEditMenu->displayedNumberOfCards) {
		MessageBox(SokuLib::window, "A trial deck must either contain no card or 20 cards.", "Invalid deck", MB_ICONERROR);
		SokuLib::playSEWaveBuffer(0x29);
		this->_deckEditMenu->saveDialogDisplayed = false;
		return true;
	}
	SokuLib::leftPlayerInfo.effectiveDeck.clear();
	for (auto &pair : *this->_deckEditMenu->editedDeck) {
		if (pair.second > 4) {
			MessageBox(SokuLib::window, "Cannot have more than 4 copies of the same card in a deck.", "Invalid deck", MB_ICONERROR);
			SokuLib::playSEWaveBuffer(0x29);
			this->_deckEditMenu->saveDialogDisplayed = false;
			return true;
		}
		for (int i = 0; i < pair.second; i++)
			SokuLib::leftPlayerInfo.effectiveDeck.push_back(pair.first);
	}
	assert(SokuLib::leftPlayerInfo.effectiveDeck.size == this->_deckEditMenu->displayedNumberOfCards);
	this->_needReload = true;
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
	return false;
}

bool ComboTrialEditor::_copyDeckToDummyDeck()
{
	if (this->_deckEditMenu->displayedNumberOfCards != 20 && this->_deckEditMenu->displayedNumberOfCards) {
		MessageBox(SokuLib::window, "A trial deck must either contain no card or 20 cards.", "Invalid deck", MB_ICONERROR);
		SokuLib::playSEWaveBuffer(0x29);
		this->_deckEditMenu->saveDialogDisplayed = false;
		return true;
	}
	SokuLib::rightPlayerInfo.effectiveDeck.clear();
	for (auto &pair : *this->_deckEditMenu->editedDeck) {
		if (pair.second > 4) {
			MessageBox(SokuLib::window, "Cannot have more than 4 copies of the same card in a deck.", "Invalid deck", MB_ICONERROR);
			SokuLib::playSEWaveBuffer(0x29);
			this->_deckEditMenu->saveDialogDisplayed = false;
			return true;
		}
		for (int i = 0; i < pair.second; i++)
			SokuLib::rightPlayerInfo.effectiveDeck.push_back(pair.first);
	}
	assert(SokuLib::rightPlayerInfo.effectiveDeck.size == this->_deckEditMenu->displayedNumberOfCards);
	this->_needReload = true;
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
	return false;
}

bool ComboTrialEditor::_copyDeckToPlayerSkills()
{
	auto size = characterSkills[SokuLib::leftChar].size();

	memset(&this->_skills, 0xFF, sizeof(this->_skills));
	for (int i = 0; i < size; i++) {
		bool hasSkill = false;

		for (int j = 0; j < 3; j++) {
			auto lvl = (*this->_deckEditMenu->editedDeck).find(100 + i + j * size);

			if (lvl != this->_deckEditMenu->editedDeck->end() && lvl->second != 0) {
				this->_skills[i + j * size].notUsed = false;
				this->_skills[i + j * size].level = lvl->second;
				hasSkill = true;
			}
		}
		if (!hasSkill) {
			this->_skills[i].notUsed = false;
			this->_skills[i].level = 0;
		}
	}
	memcpy(&SokuLib::getBattleMgr().leftCharacterManager.skillMap, &this->_skills, sizeof(this->_skills));
	return false;
}

bool ComboTrialEditor::_copyDeckToPlayerHand()
{
	if (this->_deckEditMenu->displayedNumberOfCards > 5) {
		MessageBox(SokuLib::window, "A hand cannot contain more than 5 cards.", "Invalid hand", MB_ICONERROR);
		SokuLib::playSEWaveBuffer(0x29);
		this->_deckEditMenu->saveDialogDisplayed = false;
		return true;
	}
	for (auto &pair : *this->_deckEditMenu->editedDeck) {
		for (int i = 0; i < SokuLib::leftPlayerInfo.effectiveDeck.size; i++)
			if (SokuLib::leftPlayerInfo.effectiveDeck[i] == pair.first)
				goto allGood;
		MessageBox(SokuLib::window, ("Your deck doesn't contain any " + characterCards[SokuLib::leftChar][pair.first].first + ".").c_str(), "Invalid hand", MB_ICONERROR);
		SokuLib::playSEWaveBuffer(0x29);
		this->_deckEditMenu->saveDialogDisplayed = false;
		return true;
allGood:
		continue;
	}
	this->_hand.clear();
	for (auto &pair : *this->_deckEditMenu->editedDeck)
		for (int i = 0; i < pair.second; i++)
			this->_hand.push_back(pair.first);
	assert(this->_hand.size() == this->_deckEditMenu->displayedNumberOfCards);
	this->_initGameStart();
	SokuLib::getBattleMgr().leftCharacterManager.objectBase.hp = 1;
	return false;
}

void ComboTrialEditor::_typeNewCombo()
{
	std::string old = this->_transformComboToString();
	std::string str = InputBox("Enter new combo", "New combo", old);

	if (str.empty())
		return SokuLib::playSEWaveBuffer(0x29);
	try {
		this->_loadExpected(str);
		this->_comboSprite.texture.createFromText(str.c_str(), defaultFont12, {250, 30});
		this->_comboSprite.setSize(this->_comboSprite.texture.getSize());
		this->_comboSprite.rect.width = this->_comboSprite.texture.getSize().x;
		this->_comboSprite.rect.height = this->_comboSprite.texture.getSize().y;
		return SokuLib::playSEWaveBuffer(0x28);
	} catch (std::exception &e) {
		MessageBox(SokuLib::window, ("Invalid combo string: " + std::string(e.what())).c_str(), "Invalid combo", MB_ICONERROR);
	}
	try {
		this->_loadExpected(old);
	} catch (...) {}
	SokuLib::playSEWaveBuffer(0x29);
}

void ComboTrialEditor::SpecialAction::parse()
{
	std::string chargeStr;
	std::string delayStr;
	bool d = false;
	bool p = false;

	this->moveName.clear();
	for (auto c : this->name) {
		if (c == ':' && !p) {
			d = !d;
			if (d && !delayStr.empty())
				throw std::invalid_argument("Multiple delays found for move " + this->name);
		}
		if (c == '[' && !p && !d) {
			p = true;
			if (!chargeStr.empty())
				throw std::invalid_argument("Multiple charge timers found for move " + this->name);
		}
		if (c == '!') {
			this->optional = true;
			continue;
		}
		p &= c != ']' && !d;
		if (c == '[' || c == ']' || c == ':')
			continue;
		if (d)
			delayStr += c;
		else if (p)
			chargeStr += c;
		else
			this->moveName += std::tolower(c);
	}
	printf("Move %s -> %s [%s] :%s: -> ", this->name.c_str(), this->moveName.c_str(), chargeStr.c_str(), delayStr.c_str());

	std::string move;
	std::string firstMove;

	this->actionsStr.clear();
	this->actions.clear();
	this->realMoveName = this->moveName;
	try {
		size_t pos;
		std::string str = this->moveName;
		std::string real;

		do {
			pos = str.find('/');
			move = str.substr(0, pos);
			if (firstMove.empty())
				firstMove = move;
			this->actionsStr.push_back(move);
			this->actions.push_back(_getMoveAction(SokuLib::leftChar, move));
			if (pos != std::string::npos) {
				str = str.substr(pos + 1);
				printf("%i/", this->actions.back());
				real += move + "/";
			} else {
				printf("%i ", this->actions.back());
				real += move;
			}
		} while (pos != std::string::npos);
		this->moveName = real;
	} catch (std::exception &) {
		printf("INVALID\n");
		throw std::invalid_argument(move + " is not a recognized move name");
	}

	try {
		if (!chargeStr.empty())
			this->chargeTime = std::stoul(chargeStr);
		else
			this->chargeTime = 0;
		printf("%i ", this->chargeTime);
	} catch (std::exception &) {
		printf("INVALID\n");
		throw std::invalid_argument(chargeStr + " is not a valid charge timer");
	}

	try {
		if (!delayStr.empty())
			this->delay = std::stoul(delayStr);
		else
			this->delay = 0;
		printf("%i ", this->delay);
	} catch (std::exception &) {
		printf("INVALID ");
		throw std::invalid_argument(delayStr + " is not a valid delay");
	}

	try {
		this->inputs = actionStrToInputs.at(firstMove);
	} catch (...) {
		throw std::invalid_argument(firstMove + " is not yet implemented");
	}

	SokuLib::Vector2i realSize;

	if (this->chargeTime) {
		int dig = std::isdigit(this->moveName.back());
		int index = this->moveName.size() - 2;

		while (index >= 0 && !dig && !std::isdigit(this->moveName[index]))
			index--;
		this->moveName = this->moveName.substr(0, index + 1) + "[" + this->moveName.substr(index + 1) + "]";
	}
	puts(this->moveName.c_str());
	this->sprite.texture.createFromText(this->moveName.c_str(), defaultFont16, {400, 20}, &realSize);
	this->sprite.setSize(realSize.to<unsigned>());
	this->sprite.rect.width = realSize.x;
	this->sprite.rect.height = realSize.y;
}

ComboTrialEditor::ScorePrerequisites::ScorePrerequisites(const nlohmann::json &json, const ComboTrialEditor::ScorePrerequisites *other)
{
	if (other)
		*this = *other;

	if (json.contains("max_attempts")) {
		if (!json["max_attempts"].is_number())
			throw std::invalid_argument("Field \"max_attempts\" is specified but not a number");
		this->attempts = json["max_attempts"];
		if (!this->attempts)
			throw std::exception();
	}
	if (json.contains("min_hits")) {
		if (!json["min_hits"].is_number())
			throw std::invalid_argument("Field \"min_hits\" is specified but not a number");
		this->hits = json["min_hits"];
	}
	if (json.contains("min_damage")) {
		if (!json["min_damage"].is_number())
			throw std::invalid_argument("Field \"min_damage\" is specified but not a number");
		this->damage = json["min_damage"];
	}
	if (json.contains("min_limit")) {
		if (!json["min_limit"].is_number())
			throw std::invalid_argument("Field \"min_limit\" is specified but not a number");
		this->minLimit = json["min_limit"];
	}
}

bool ComboTrialEditor::ScorePrerequisites::met(unsigned currentAttempts) const
{
	auto &battle = SokuLib::getBattleMgr();

	if (this->attempts <= currentAttempts)
		return false;
	if (this->hits > battle.leftCharacterManager.combo.nbHits)
		return false;
	if (this->damage > battle.leftCharacterManager.combo.damages)
		return false;
	if (this->minLimit > battle.leftCharacterManager.combo.limit)
		return false;
	return true;
}