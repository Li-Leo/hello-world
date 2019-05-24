/************************************************
*
* NAME: font_data.c
*
* DESCRIPTION:
*   font resource
*
* REVISION HISTORY:
*   Rev 1.0 2016-3-1 ryan
* Initial revision.
*
************************************************/
#include "font_data.h"
#include "common.h"
#include "string.h"

/************************************************
* Declaration
************************************************/

typedef struct
{
    const T_CHAR* font_name;
    const T_CHAR* font_style;
    const TS_FONT* font_data;
}TS_FONT_DATA;

typedef struct
{
    TE_LANG lang;
    const TS_FONT_DATA* font_data;
}TS_LANG_FONT_DATA;

/************************************************
* Variable 
************************************************/
#include "generated_font_data.h"

static const TS_FONT_DATA* g_font_data_ptr = NULL;

/************************************************
* Function 
************************************************/


//select font data by lang
TE_BOOLEAN SelectFontDataByLang(TE_LANG lang)
{
    T_S32 i;

    for (i = 0; i < sizeof(g_lang_font_data) / sizeof(g_lang_font_data[0]); i++)
    {
        if (g_lang_font_data[i].lang == lang)
        {
            g_font_data_ptr = g_lang_font_data[i].font_data;
            return E_TRUE;
        }
    }

    return E_FALSE;
}

//get the font, it will find the first font which's heigh is same as the parameter
const TS_FONT* GetFont(T_S32 font_height)
{
    const TS_FONT_DATA* work = g_font_data_ptr;
    
    M_Assert(g_font_data_ptr);

    //find the height font
    while (work && work->font_name)
    {
        if (work->font_data->height== font_height)
        {
            return work->font_data;
        }
        work++;
    }

    return NULL;
}

//get the font by name, style and height at the select lang
//font_style: can be regular,regular_italic,bold,bold_italic
const TS_FONT* GetFontEx(const T_CHAR* font_name, const T_CHAR* font_style, T_S32 font_height)
{
    const TS_FONT_DATA* work = g_font_data_ptr;

    M_Assert(g_font_data_ptr);

    //find the font
    while (work && work->font_name)
    {
        if (work->font_data->height == font_height && strcmp(work->font_name,font_name)==0
            && strcmp(work->font_style, font_style)==0 )
        {
            return work->font_data;
        }
        work++;
    }

    return NULL;
}

//get the font by lang, name, style and height at all lang
const TS_FONT* GetFontByLang(TE_LANG lang, const T_CHAR* font_name, const T_CHAR* font_style, T_S32 font_height)
{
    const TS_FONT_DATA* work = NULL;
    T_S32 i;

    //find the lang font
    for (i = 0; i < sizeof(g_lang_font_data) / sizeof(g_lang_font_data[0]); i++)
    {
        if (g_lang_font_data[i].lang == lang)
        {
            work = g_lang_font_data[i].font_data;
            break;
        }
    }

    //find the font
    while (work && work->font_name)
    {
        if (work->font_data->height == font_height && strcmp(work->font_name, font_name) == 0
            && strcmp(work->font_style, font_style)==0 )
        {
            return work->font_data;
        }
        work++;
    }

    return NULL;
}

