/****************************************************************************
 * ==> Vertex --------------------------------------------------------------*
 ****************************************************************************
 * Description : Vertex descriptor                                          *
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

#include "Vertex.h"

 // std
#include <memory>

//---------------------------------------------------------------------------
// Vertex
//---------------------------------------------------------------------------
Vertex::Vertex() :
    m_Stride(0),
    m_Type(IE_VT_Unknown),
    m_Format(IE_VF_None),
    m_CoordType(IE_VC_XYZ)
{}
//---------------------------------------------------------------------------
Vertex::~Vertex()
{}
//---------------------------------------------------------------------------
Vertex* Vertex::Clone() const
{
    // clone vertex
    std::unique_ptr<Vertex> pClone(new Vertex());
    pClone->m_Name      = m_Name;
    pClone->m_Stride    = m_Stride;
    pClone->m_Type      = m_Type;
    pClone->m_Format    = m_Format;
    pClone->m_CoordType = m_CoordType;
    return pClone.release();
}
//---------------------------------------------------------------------------
std::size_t Vertex::CalculateStride() const
{
    std::size_t stride;

    // search for coordinate type
    switch (m_CoordType)
    {
        case IE_VC_XY:
            stride = 2;
            break;

        case IE_VC_XYZ:
            stride = 3;
            break;

        default:
            throw new std::exception("Unknown coordinate type");
    }

    // do include normals?
    if (m_Format & Vertex::IE_VF_Normals)
        stride += 3;

    // do include texture coordinates?
    if (m_Format & Vertex::IE_VF_TexCoords)
        stride += 2;

    // do include vertex color?
    if (m_Format & Vertex::IE_VF_Colors)
        stride += 4;

    return stride;
}
//---------------------------------------------------------------------------
bool Vertex::CompareFormat(const Vertex& other) const
{
    return (m_Stride    == other.m_Stride &&
            m_Type      == other.m_Type &&
            m_Format    == other.m_Format &&
            m_CoordType == other.m_CoordType);
}
//---------------------------------------------------------------------------
