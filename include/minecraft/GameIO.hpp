#ifndef _MINECRAFT_GAMEIO_HPP_
#define _MINECRAFT_GAMEIO_HPP_

#include <map>
#include <string>
#include <stdexcept>
#include <minecraft/Map.hpp>
#include <minecraft/Character.hpp>

namespace minecraft {
	class GameIO {
		private:
			char* m_file; // Could be a FILE stream type or something like this
			Map* m_map;
			Character* m_character;
			std::map<std::string,Cube*>* m_gameObjects;
		public:
			GameIO() {};
			GameIO(char* filePath) { m_file = filePath; }
			void SetCharacter(Character* character) { m_character = character; }
			void SetMap(Map* map) { m_map = map; }
			void SetGameObjects(std::map<std::string,Cube*>* dictionary) { m_gameObjects = dictionary; }
			void LoadMap() throw(std::logic_error);
			void GenerateMap(size_t size);
			void AddGold(int size);
			void AddDeposit(int size);
			void CoverWithDirt(int size);
			void DeleteLonely(int size);
			void AddNoise(int factor);
			void LoadInventory() {} // Why not ?
			void SaveMap();
			void SaveInventory() {}

			std::map<std::string,Cube*>* GetGameObjects() { return m_gameObjects; }

			float dot(float x, float y, float z, float* g);
			float noise(float xin, float yin, float zin);
			float simplex_noise(int octaves, float x, float y, float z);
	};
}

#endif
