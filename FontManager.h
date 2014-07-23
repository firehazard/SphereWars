#ifndef FONT_MANAGER_H_
#define FONT_MANAGER_H_

#include <SDL/SDL_ttf.h>
#include "sdlheaders.h"
#include <set> 
#include <map>
#include <string>
using namespace std;

#define DEFAULT_FONT "resources/ArialBlack.ttf"
#define DEFAULT_SIZE 20

class FontManager
{
 public:
   	struct CachedTexture {
  		GLuint texture;
		unsigned short text_w, text_h;
		unsigned short image_w, image_h;
  	};
 
	FontManager(const char *fontPath=DEFAULT_FONT, 
	            int size=DEFAULT_SIZE, 
	            const SDL_Color &color=s_defaultColor);
	virtual ~FontManager();	
	void setColor(const SDL_Color &color);	
	void setColor(short r, short g, short b);
	void setFontPath(const char *fontPath);
	void setFontSize(int fontSize);
	
	void drawCachedTexture(const CachedTexture &tex, short x, short y);
	
	static void cleanupAll();
	CachedTexture renderText(const string &text)
		{ return renderText(text, m_font, m_color); }

	CachedTexture renderText(const string &text,
                             TTF_Font *font,
                             SDL_Color color);
	
	static void enable2D();
 	static void disable2D();
 	
 private:
 	TTF_Font *m_font;
	const char *m_fontPath;
 	int m_size;
 	SDL_Color m_color;
 	SDL_Rect m_position; // necessary?
 		
 	static SDL_Color s_defaultColor;
 	static set<FontManager *> s_openFonts;
 	static bool s_isLibInitialized;
 	
 	void cleanup(); 	
 	void loadFont();
 	static void init();
 	void initTextures();
	void SDL_GL_RenderText(const char *text, TTF_Font *font,
                           SDL_Color color, SDL_Rect *location);
 	void drawTexture(GLuint texture, const SDL_Rect *location);
};

typedef map<string, FontManager::CachedTexture> TextureCache;

FontManager::CachedTexture lookupOrCreateTexture(FontManager &fm, 
                                                 TextureCache &cache, 
                                                 const string &str);

#endif /*FONT_H_*/
