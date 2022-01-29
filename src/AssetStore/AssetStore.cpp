#include "AssetStore\AssetStore.h"

#include <SDL_image.h>

#include "Log/Logger.h"

AssetStore::AssetStore()
{
}

AssetStore::~AssetStore()
{
}

void AssetStore::ClearAssets()
{
	for (auto texture : textures_)
	{
		SDL_DestroyTexture(texture.second);
	}
	textures_.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& asset_id, const std::string& file_path)
{
	SDL_Surface* surface = IMG_Load(file_path.c_str());
	if (!surface)
	{
		Logger::Log("Error with loading file {} - {}", file_path.c_str(), SDL_GetError());
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	textures_.emplace(asset_id, texture);
}

SDL_Texture* AssetStore::GetTexture(const std::string& asset_id)
{
	return textures_[asset_id];
}