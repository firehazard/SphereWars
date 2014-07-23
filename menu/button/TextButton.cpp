#include "TextButton.h"
#include "../../Constants.h"
#include <climits>
#include <assert.h>

#define TEXT_UNINITIALIZED ""
#define INVALID_TEXTURE UINT_MAX

#define DEFAULT_STANDARD_SIZE  20
#define HIGHLIGHT_SIZE_DELTA 5


SDL_Color TextButton::s_defaultColor = {0, 200, 200, 0};
SDL_Color TextButton::s_highlightColor = {0, 230, 230, 0};

TextButton::TextButton()
{
	m_texture.texture = INVALID_TEXTURE;
	m_highlightTexture.texture = INVALID_TEXTURE;
	m_isTextFinalized = false;
	m_textSize = DEFAULT_STANDARD_SIZE;
	m_textColor = s_defaultColor;	
	m_text = TEXT_UNINITIALIZED;

}

TextButton::~TextButton()
{	
	freeTextureIfValid();
}

void TextButton::freeTextureIfValid()
{
	// Free old texture
	if (m_texture.texture != INVALID_TEXTURE) {
		glDeleteTextures(1, &m_texture.texture);
		glDeleteTextures(1, &m_highlightTexture.texture);
	}
}

void TextButton::setText(const string &text)
{	
	if (text.size() > 0) {
		m_text = text;
		freeTextureIfValid();
		m_fontMngr.setFontSize(m_textSize);
		m_fontMngr.setColor(m_textColor);
		m_texture = m_fontMngr.renderText(m_text);
		
		m_fontMngr.setColor(s_highlightColor);
		m_fontMngr.setFontSize(m_textSize + HIGHLIGHT_SIZE_DELTA);
		m_highlightTexture = m_fontMngr.renderText(m_text);
		
		m_isTextFinalized = false;
	}
}

void TextButton::setTextSize(int textSize)
{
	m_textSize = textSize;
	setText(m_text); // Rebuilds textures with new size	
}

void TextButton::setTextColor(short r, short g, short b)
{
	m_textColor.r = r;
	m_textColor.g = g;
	m_textColor.b = b;
	
	setText(m_text); // Rebuilds textures with new size	
}


void TextButton::drawMe()
{
	Button::drawMe();

	FontManager::CachedTexture *textureToUse;
	int x = m_x;
	int y = SCREEN_HEIGHT - m_y - m_texture.image_h;
	if (m_isHovered) {
		textureToUse = &m_highlightTexture;	
		x -= (m_highlightTexture.text_w - m_texture.text_w) / 2; 
		y += (m_highlightTexture.text_h - m_texture.text_h) / 2;
		y -= (m_highlightTexture.image_h - m_texture.image_h);  
	} else {
		textureToUse = &m_texture;	
	}
	
	m_fontMngr.drawCachedTexture(*textureToUse, x, y); 
}

int TextButton::getWidth()
{
	return m_texture.text_w;
}

int TextButton::getHeight()
{
	return m_texture.text_h;
}
