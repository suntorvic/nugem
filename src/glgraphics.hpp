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

#ifndef GLGRAPHICS_H
#define GLGRAPHICS_H

#include "window.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Nugem {

struct GlTexture {
	GLuint tid = 0;
	int w;
	int h;
	GlTexture(GLuint, int, int);
	GlTexture(const GlTexture &);
	GlTexture(GlTexture &&);
	~GlTexture();
	static GlTexture surfaceToTexture(const SDL_Surface * surface);
private:
	static std::unordered_map<GLuint, unsigned int> useCounters;
};

struct GlShader {
	GLuint shaderId;
	GlShader(GLuint);
	GlShader(GlShader &&);
	static GlShader fromFile(const std::string &, GLuint type);
	static GlShader fromString(const std::string &, GLuint type);
	void deleteShader();
	~GlShader();
	bool compile();
};

struct GlShaderProgram {
	GLuint shaderProgramId;
	void attachShader(const GlShader &);
	bool link();
	void use();
	GlShaderProgram();
	~GlShaderProgram();
};
class Game;



class GlGraphics
{
public:
	GlGraphics(Window &);
	~GlGraphics();
	void initialize(Game * game);
	void finish();
	void clear();
	void display();
	void renderSprite(size_t, size_t, size_t, size_t, size_t, size_t, size_t);
	GLint shaderProgram;
	std::vector<std::array<GLint, 2>> positionVertice;
	std::vector<std::array<GLfloat, 2>> texCoords;
	GLint uniform_mvp;
	GLint uniform_glSpriteTexture;
	GLuint atlasTid;
	GLuint inputVertexBuffer;
	GLuint texCoordsBuffer;
	GLint positionVertAttrib;
	GLint texCoordsAttrib;
	GLuint vao;
private:
	Window &mWindow;
	Game * mGame;
	SDL_GLContext mSDLGlCtx;
	GLuint spritesVAO;
};

}

#endif // GLGRAPHICS_H