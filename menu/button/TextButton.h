#ifndef TEXTBUTTON_H_
#define TEXTBUTTON_H_
#include "Button.h"
#include "../../FontManager.h"

class TextButton : public Button
{
 public:
	TextButton();
	~TextButton();	
	
	void setText(const string &text);
	void setTextSize(int textSize);
	void setTextColor(short r, short g, short b);
	
	void drawMe();
	int getWidth();
	int getHeight();
	int getX();
	int getY();
	
 private:
 	string m_text;
 	int m_textSize;
 	SDL_Color m_textColor; 	
 	FontManager m_fontMngr;
 	FontManager::CachedTexture m_texture;
 	FontManager::CachedTexture m_highlightTexture;
 	
 	static SDL_Color s_defaultColor;
 	static SDL_Color s_highlightColor;
 	
 	bool m_isTextFinalized; 	
 	void freeTextureIfValid();
};

#endif /*TEXTBUTTON_H_*/
