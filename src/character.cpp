/*
 * Copyright (c) 2016 Victor Nivet
 * 
 * This file is part of Nugem.
 * 
 * Nugem is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 * Nugem is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 *  along with Nugem.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "character.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <string>
#include <ios>
#include <SDL.h>
#include "mugen/sffv1.hpp"
#include "mugen/sffv2.hpp"


namespace Nugem {

Character::Character(const char * charid): m_id(charid)
{
	   mCurrentPalette = 0;
	   mCurrentAnimStep = 0;
	   mDirectory = "chars/" + m_id;
	   mDefinitionFilename = m_id + ".def";
	loadCharacterDef((mDirectory + "/" + mDefinitionFilename).c_str());
	std::string cmdfile = (std::string) m_def["files"]["cmd"];
	loadCharacterCmd((mDirectory + "/" + cmdfile).c_str());
	std::string airfile = (std::string) m_def["files"]["anim"];
	loadCharacterAnimations((mDirectory + "/" + airfile).c_str());
}

Character::Character(Character && character)
{
	// Move
	std::swap(m_id, character.m_id);
	std::swap(m_name, character.m_name);
	std::swap(m_x, character.m_x);
	std::swap(m_y, character.m_y);
	std::swap(mCurrentPalette, character.mCurrentPalette);
	std::swap(mCurrentAnimStep, character.mCurrentAnimStep);
	std::swap(mDirectory, character.mDirectory);
	std::swap(mDefinitionFilename, character.mDefinitionFilename);
	std::swap(mSpriteFilename, character.mSpriteFilename);
	std::swap(m_animations, character.m_animations);
	std::swap(m_def, character.m_def);
}

Character::Character(const Character & character): Character(character.id().c_str())
{
}

Character::~Character()
{
}

const std::string & Character::id() const
{
	return m_id;
}

const std::string & Character::name() const
{
	return m_name;
}

Mugen::DefinitionFile & Character::def()
{
	return m_def;
}

const std::string & Character::dir() const
{
	return mDirectory;
}

void Character::loadCharacterDef(const char * filepath)
{
	m_def = Mugen::DefinitionFile(filepath);
	   mMugenVersion = (std::string) m_def["info"]["mugenversion"];
	   mSpriteFilename = (std::string) m_def["files"]["sprite"];
}

void Character::loadCharacterCmd(const char * filepath)
{
	m_cmd.readFile(filepath);
}

void Character::loadCharacterAnimations(const char * filepath)
{
	m_animations = Mugen::AnimationData(filepath);
}
/*
void Character::render()
{
	int h, w;
	//SDL_GetRendererOutputSize(renderer, &w, &h);
	Mugen::animation_t & animation = curAnimIterator->second;
	Mugen::animstep_t & animstep = animation.steps[currentAnimStep];
	spriteHandler->setSprite(animstep.group, animstep.image);
	currentGameTick++;
	if (currentGameTick >= animstep.ticks) {
		currentAnimStep++;
		currentGameTick = 0;
	}
	if (currentAnimStep >= animation.steps.size()) {
		currentAnimStep = animation.loopstart;
	} 
	if (texture)
		SDL_DestroyTexture(texture);
	spriteHandler->setPalette(currentPalette);
	SDL_Surface * surface = spriteHandler->surface();
	width = surface->w;
	height = surface->h;
	//texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_Rect DestR;
	// Centering the sprite in the middle of the screen
	x = w / 2;
	y = h / 2;
	x -= spriteHandler->getImageXAxis() + animstep.x;
	y -= spriteHandler->getImageYAxis() + animstep.y;
	DestR.x = x;
	DestR.y = y;
	DestR.w = width;
	DestR.h = height;
	SDL_RendererFlip flip = (SDL_RendererFlip) 0;
	if (animstep.hinvert)
		flip = (SDL_RendererFlip) ( SDL_FLIP_HORIZONTAL | flip );
	if (animstep.vinvert)
		flip = (SDL_RendererFlip) ( SDL_FLIP_VERTICAL | flip );
	//SDL_RenderCopyEx(renderer, texture, nullptr, &DestR, 0, nullptr, flip);
}*/

void Character::handleEvent(const SDL_Event e)
{/*
	const size_t npalettes = spriteHandler->getTotalPaletteNumber();
	if (e.type == SDL_KEYDOWN) {
		// Select surfaces based on key press
		
		// For now...
		switch (e.key.keysym.sym) {
		// Changing sprites
		case SDLK_UP:
			curAnimIterator++;
			currentAnimStep = 0;
			currentGameTick = 0;
			break;

		case SDLK_DOWN:
			if (curAnimIterator == animations.begin())
				curAnimIterator = animations.end();
			curAnimIterator--;
			currentAnimStep = 0;
			currentGameTick = 0;
			break;
			
		
		case SDLK_s:
			currentPalette--;
			break;
		
		case SDLK_f:
			currentPalette++;
			break;
		}

		currentPalette = (currentPalette + npalettes) % npalettes;
		if (curAnimIterator == animations.end())
			curAnimIterator = animations.begin();
	}*/
}

const Mugen::Sprite & Character::currentSprite() const
{
	return m_sprites.at(mCurrentPalette).at(mCurrentSprite);
}

void Character::loadForMenu()
{
	if (!m_spriteLoader.isInitialized())
		m_spriteLoader.initialize(mDirectory + "/" + mSpriteFilename, this);
	std::vector<Mugen::Spriteref> menurefs { Mugen::Spriteref(9000, 0), Mugen::Spriteref(9000, 1) };
	std::vector< std::unordered_map< Mugen::Spriteref, Mugen::Sprite > > menusprites = m_spriteLoader.load(menurefs.begin(), menurefs.end());
	   mCurrentPalette = 0;
	for ( size_t i = 0; i < menusprites.size(); i++) {
		std::unordered_map< Mugen::Spriteref, Mugen::Sprite > & palettesprites = menusprites[i]; 
		m_selectionSprite.push_back(palettesprites.at(Mugen::Spriteref(9000, 0)));
		m_faceSprite.push_back(palettesprites.at(Mugen::Spriteref(9000, 1)));
	}
}

const Mugen::Sprite & Character::faceSprite() const
{
	return m_faceSprite[mCurrentPalette];
}

const Mugen::Sprite & Character::selectionSprite() const
{
	return m_selectionSprite[mCurrentPalette];
}

}




