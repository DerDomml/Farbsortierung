#ifndef __FONT_H__
#define __FONT_H__

#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief  Font structure used on my LCD libraries
*/
typedef struct FONT_DEFINITION {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;

/**
* @brief  String length and height
*/
typedef struct FONT_CONTENT_DEFINITION {
	uint16_t Length;      /*!< String length in units of pixels */
	uint16_t Height;      /*!< String height in units of pixels */
} FontStringSize;

extern  FontDef Font_11x18;
extern  FontDef Font_16x26;
extern  FontDef Font_7x10 ;

void TFT_FontInit();

/**
* @brief  Puts single character to LCD
* @param  c: Character to be displayed
* @param  *font: Pointer to @ref TM_FontDef_t used font
* @param  foreground: Color for char
* @param  background: Color for char background
* @retval None
*/
void TFT_TextPutc(char c,  FontDef* font, uint32_t foreground, uint32_t background)   ;

/**
* @brief  Puts string to LCD
* @param  *str: Pointer to first character
* @param  *font: Pointer to @ref TM_FontDef_t used font
* @param  foreground: Color for string
* @param  background: Color for string background
* @retval None
*/
void TFT_TextPuts(char* str,  FontDef *font, uint32_t foreground, uint32_t background)   ;

/**
* @brief  Gets width and height of box with text
* @param  *str: Pointer to first character
* @param  *font: Pointer to @ref TM_FontDef_t used font
* @param  *width: Pointer to variable to store width
* @param  *height: Pointer to variable to store height
* @retval None
*/
void TFT_TextGetStringSize(char* str,  FontDef* font, uint16_t* width, uint16_t* height)  ;



uint16_t TFT_TextGetCursorX() ;
uint16_t TFT_TextGetCursorY() ;

#ifdef __cplusplus
}
#endif


#endif
