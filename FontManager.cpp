#include "FontManager.h"
#include "Utils.h"
#include "SDLEngine.h"
#include <math.h>
#include <stdlib.h>

// Define static data
set<FontManager *> FontManager::s_openFonts;
// R,G,B,Unused
SDL_Color FontManager::s_defaultColor = {0, 200, 200, 0};
bool FontManager::s_isLibInitialized = false;


// Define static methods
void FontManager::init()
{
	if (!s_isLibInitialized) {		
		if (TTF_Init()) {
			fprintf(stderr, "Error while initializing TTF Library: %s", TTF_GetError());
			exit(1);
		}
		
		atexit(cleanupAll);
		
		s_isLibInitialized = true;
	}
}

// Needs to be called at exit.
void FontManager::cleanupAll()
{
	for(set<FontManager *>::iterator iter = s_openFonts.begin();
		iter != s_openFonts.end(); ++iter) {
		(*iter)->cleanup();
	}
	TTF_Quit();
}


FontManager::FontManager(const char *fontPath, int size, 
                         const SDL_Color &color)
  : m_font(NULL), m_fontPath(fontPath), m_size(size), m_color(color)
{
	init();
	s_openFonts.insert(this);
	loadFont();
}

FontManager::~FontManager()
{	
	s_openFonts.erase(this);
	cleanup();	
}

void FontManager::setColor(const SDL_Color &color)
{
	m_color = color;
}

void FontManager::setColor(short r, short g, short b)
{
	m_color.r = r;	
	m_color.g = g;
	m_color.b = b;
}
	
void FontManager::setFontPath(const char *fontPath)
{
	m_fontPath = fontPath;
	loadFont();	
}

void FontManager::setFontSize(int fontSize)
{
	m_size = fontSize;
	loadFont();
}

void FontManager::loadFont() {
	cleanup();
	m_font = TTF_OpenFont(m_fontPath, m_size);
	if (!m_font)  {
		printf("Error loading font: %s", TTF_GetError());
		exit(1);
	}
}

// Called by destructor or by 
// FontCleanup.  NULLs out the m_font field
// so that it can safely be called by both.
void FontManager::cleanup()
{
	if (m_font) {
		TTF_CloseFont(m_font);
	}
	m_font = NULL;
}

// CITATION: Based on code from
// http://www.gamedev.net/community/forums/topic.asp?topic_id=284259
// Note: If/when we want to free the texture, use
//	glDeleteTextures(1, myCachedTexture.texture);
// (Not sure if we want to ever free them, though, 'cause
// right now I'm caching them, since they get re-used.)
FontManager::CachedTexture FontManager::renderText(const string &text,
             				                       TTF_Font *font,
                            				       SDL_Color color)
{
	SDL_Surface *firstSurface;
	SDL_Surface *secondSurface;
	CachedTexture cachedTex;

	
	// Render text to an initial SDL Surface
	firstSurface = TTF_RenderText_Blended(font, text.c_str(), color);
	if (firstSurface == NULL) {
		fprintf(stderr, "TTF_RenderText_Blended failed: %s", TTF_GetError());
		exit(1);
	}

	// Round up the drawn width and height to a power of two, to optimize
	// behavior on some graphics cards
	cachedTex.text_w = firstSurface->w;
	cachedTex.text_h = firstSurface->h;
	cachedTex.image_w = Utils::nextPowerOfTwo(firstSurface->w);
	cachedTex.image_h = Utils::nextPowerOfTwo(firstSurface->h);

	// Create an intermediate surface	
	secondSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, cachedTex.image_w, cachedTex.image_h, 32, 
		0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	if (secondSurface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		exit(1);
	}
	
	// Copy rendered text to the intermediate surface
	SDL_BlitSurface(firstSurface, 0, secondSurface, 0);

	// Make non-black pixels (the font pixels) non-transparent	
	const unsigned numPixels = secondSurface->w * secondSurface->h;
	for (unsigned i = 0; i < numPixels; i++) {
		unsigned *pixel = &((unsigned *)(secondSurface->pixels))[i]; 
		if ((*pixel & 0x00ffffff) != 0) {			
			*pixel |= 0xff000000;			
		}
	}

	// Create a GL texture for the text
	glGenTextures(1, &(cachedTex.texture));
	glBindTexture(GL_TEXTURE_2D, cachedTex.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, cachedTex.image_w, cachedTex.image_h, 0, GL_RGBA, 
	             GL_UNSIGNED_BYTE, secondSurface->pixels );

	SDL_FreeSurface(firstSurface);
	SDL_FreeSurface(secondSurface);
	return cachedTex;
}

void FontManager::drawTexture(GLuint texture, const SDL_Rect *location)
{
	// Disable 3D lighting on 2D textures
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);

	// prepare to render our texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Specify how to min/magnify texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	// Don't filter any texture colors out
	glColor3f(1.0f, 1.0f, 1.0f);
	
	// Draw a Quad, onto which we'll paint the text texture
	glBegin(GL_QUADS);
		// SDL Origin is in lower-left corner
		// Hence, the GL and SDL corner coordinates are
		// specified differently.
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2f(location->x,     location->y);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(location->x + location->w, location->y);
		
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2f(location->x + location->w, location->y + location->h);
		
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2f(location->x,     location->y + location->h);
	glEnd();
}

void FontManager::enable2D()
{
	int vPort[4];
  
	glGetIntegerv(GL_VIEWPORT, vPort);
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Necessary for alpha-blending with background
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	// Disable depth test so we draw on top of stuff
	glDisable(GL_DEPTH_TEST);
}

// Undoes the changes from enable2d
void FontManager::disable2D()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void FontManager::drawCachedTexture(const CachedTexture &tex, short x, short y)
{
	enable2D();
	
	m_position.x = x;
	m_position.y = y;
	SDL_Rect location = {x, y, tex.image_w, tex.image_h};
	//SDL_GL_RenderText(text, m_font, m_color, &m_position);
	drawTexture(tex.texture, &location);  

	disable2D();
}

FontManager::CachedTexture 
lookupOrCreateTexture(FontManager &fm, TextureCache &cache, const string &str)
{
	TextureCache::iterator iter = cache.find(str);
	if (iter == cache.end()) {
		cache[str] = fm.renderText(str);
	}
	return cache[str];		
}

