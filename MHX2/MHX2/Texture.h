/****************************************************************************
 * ==> Texture -------------------------------------------------------------*
 ****************************************************************************
 * Description : Texture descriptor                                         *
 * Developer   : Jean-Milost Reymond                                        *
 ****************************************************************************
 * MIT License - mhx2 reader                                                *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, sublicense, and/or sell copies of the Software, and to       *
 * permit persons to whom the Software is furnished to do so, subject to    *
 * the following conditions:                                                *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   *
 ****************************************************************************/

#pragma once

 // std
#include <vector>
#include <map>

// classes
#include "Shader.h"

/**
* Texture
*@author Jean-Milost Reymond
*/
class Texture
{
    public:
        /**
        * Texture format
        */
        enum IEFormat
        {
            IE_FT_24bit,
            IE_FT_32bit,
        };

        /**
        * Texture target
        */
        enum IETarget
        {
            IE_TT_Texture_2D,
        };

        /**
        * Texture minifying filters
        */
        enum IEMinFilter
        {
            IE_MI_Nearest,
            IE_MI_Linear,
            IE_MI_Nearest_Mipmap_Nearest,
            IE_MI_Linear_Mipmap_Nearest,
            IE_MI_Nearest_Mipmap_Linear,
            IE_MI_Linear_Mipmap_Linear,
        };

        /**
        * Texture magnification filters
        */
        enum IEMagFilter
        {
            IE_MA_Nearest,
            IE_MA_Linear,
        };

        /**
        * Texture wrap modes
        */
        enum IEWrapMode
        {
            IE_WM_Clamp,
            IE_WM_Clamp_To_Edge,
            IE_WM_Clamp_To_Border,
            IE_WM_Mirrored_Repeat,
            IE_WM_Repeat,
        };

        IEFormat    m_Format;
        IETarget    m_Target;
        IEWrapMode  m_WrapMode;
        IEMinFilter m_MinFilter;
        IEMagFilter m_MagFilter;
        unsigned    m_HashCode;
        int         m_Width;
        int         m_Height;

        Texture();
        virtual ~Texture();

        /**
        * Clears the texture
        */
        virtual void Clear();

        /**
        * Creates the texture
        *@param pPixels - texture pixels array
        *@return true on success, otherwise false
        */
        virtual bool Create(void* pPixels) = 0;

        /**
        * Deletes the texture
        */
        virtual void Delete() = 0;

        /**
        * Selects the texture
        *@param pShader - shader
        */
        virtual void Select(const Shader* pShader) const = 0;

        /**
        * Gets the texture identifier
        *@return Texture identifier
        */
        virtual inline std::size_t GetID() const = 0;
};

/**
* Texture list
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
typedef std::vector<Texture*> Textures;

/**
* Texture for 3D models
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class ModelTexture
{
    public:
        std::string m_Name;
        Texture*    m_pTexture;
        bool        m_Enabled;

        ModelTexture();
        virtual ~ModelTexture();

        /**
        * Clears the texture
        */
        virtual void Clear();

        /**
        * Selects the texture
        *@param pShader - shader
        */
        virtual void Select(const Shader* pShader) const;
};

/**
* Model texture list
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
typedef std::vector<ModelTexture*> ModelTextures;
