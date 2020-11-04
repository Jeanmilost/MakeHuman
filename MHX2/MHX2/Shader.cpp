/****************************************************************************
 * ==> Shader --------------------------------------------------------------*
 ****************************************************************************
 * Description : Basic shader language class                                *
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

#include "Shader.h"

 //---------------------------------------------------------------------------
 // Shader
 //---------------------------------------------------------------------------
Shader::Shader()
{
    PopulateAttributeDict();
}
//---------------------------------------------------------------------------
Shader::~Shader()
{}
//---------------------------------------------------------------------------
std::string Shader::GetAttributeName(IEAttribute attribute) const
{
    // search for attribute to get name
    IAttributeDictionary::const_iterator it = m_AttributeDictionary.find(attribute);

    // found it?
    if (it == m_AttributeDictionary.end())
        return "";

    return it->second;
}
//---------------------------------------------------------------------------
void Shader::SetAttributeName(IEAttribute  attribute,
                                 const std::string& name)
{
    // is name empty?
    if (!name.length())
        return;

    // change attribute name
    m_AttributeDictionary[attribute] = name;
}
//---------------------------------------------------------------------------
void Shader::PopulateAttributeDict()
{
    m_AttributeDictionary[IE_SA_Position]            = "vPosition";
    m_AttributeDictionary[IE_SA_Normal]              = "vNormal";
    m_AttributeDictionary[IE_SA_Texture]             = "vTexCoord";
    m_AttributeDictionary[IE_SA_Color]               = "vColor";
    m_AttributeDictionary[IE_SA_PerspectiveMatrix]   = "uPerspective";
    m_AttributeDictionary[IE_SA_ProjectionMatrix]    = "uProjection";
    m_AttributeDictionary[IE_SA_ViewMatrix]          = "uViewMatrix";
    m_AttributeDictionary[IE_SA_CameraMatrix]        = "uCamera";
    m_AttributeDictionary[IE_SA_ModelMatrix]         = "uModel";
    m_AttributeDictionary[IE_SA_Interpolation]       = "fInterpolation";
    m_AttributeDictionary[IE_SA_InterpolationPos]    = "viPosition";
    m_AttributeDictionary[IE_SA_InterpolationNormal] = "viNormal";
    m_AttributeDictionary[IE_SA_ColorMap]            = "sColorMap";
}
//---------------------------------------------------------------------------
