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

#include "Texture.h"

 // std
#include <string>

//---------------------------------------------------------------------------
// Texture
//---------------------------------------------------------------------------
Texture::Texture() :
    m_Format(IE_FT_24bit),
    m_Target(IE_TT_Texture_2D),
    m_WrapMode(IE_WM_Repeat),
    m_MinFilter(IE_MI_Linear),
    m_MagFilter(IE_MA_Linear),
    m_Width(0),
    m_Height(0)
{}
//---------------------------------------------------------------------------
Texture::~Texture()
{}
//---------------------------------------------------------------------------
void Texture::Clear()
{
    m_Format    = IE_FT_24bit;
    m_Target    = IE_TT_Texture_2D;
    m_WrapMode  = IE_WM_Repeat;
    m_MinFilter = IE_MI_Linear;
    m_MagFilter = IE_MA_Linear;
    m_Width     = 0;
    m_Height    = 0;
}
//---------------------------------------------------------------------------
// ModelTexture
//---------------------------------------------------------------------------
ModelTexture::ModelTexture() :
    m_pTexture(NULL),
    m_Enabled(false)
{}
//---------------------------------------------------------------------------
ModelTexture::~ModelTexture()
{}
//---------------------------------------------------------------------------
void ModelTexture::Clear()
{
    m_Name.clear();

    if (m_pTexture)
        m_pTexture->Clear();

    m_Enabled = false;
}
//---------------------------------------------------------------------------
void ModelTexture::Select(const Shader* pShader) const
{
    // disabled?
    if (!m_Enabled)
        return;

    if (m_pTexture)
        m_pTexture->Select(pShader);
}
//---------------------------------------------------------------------------
