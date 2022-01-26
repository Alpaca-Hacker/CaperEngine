#pragma once
#include <map>
#include <SDL.h>
#include <string>

class AssetStore
{
public:
	AssetStore();
	~AssetStore();

	void ClearAssets();
	void AddTexture(SDL_Renderer* renderer, const std::string& asset_id, const std::string& file_path);
	SDL_Texture* GetTexture(const std::string& asset_id);

private:
	std::map<std::string, SDL_Texture*> textures_;
};
