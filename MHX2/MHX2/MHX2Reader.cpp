/****************************************************************************
 * ==> MHX2Reader ----------------------------------------------------------*
 ****************************************************************************
 * Description : MakeHuman .mhx2 file reader                                *
 * Developer   : Jean-Milost Reymond                                        *
 ****************************************************************************
 * MIT License - Mhx2 reader                                                *
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

#include "MHX2Reader.h"

// libraries
#include "json.h"

//---------------------------------------------------------------------------
// MHX2Reader::IItem
//---------------------------------------------------------------------------
MHX2Reader::IItem::IItem() :
    m_pParent(nullptr),
    m_IsArray(false)
{}
//---------------------------------------------------------------------------
MHX2Reader::IItem::IItem(const std::string& name, IItem* pParent, bool isArray) :
    m_Key(name),
    m_pParent(pParent),
    m_IsArray(isArray)
{}
//---------------------------------------------------------------------------
MHX2Reader::IItem::~IItem()
{
    const std::size_t count = m_Items.size();

    // clera the items
    for (std::size_t i = 0; i < count; ++i)
        delete m_Items[i];
}
//---------------------------------------------------------------------------
std::string MHX2Reader::IItem::GetKey() const
{
    return m_Key;
}
//---------------------------------------------------------------------------
void MHX2Reader::IItem::SetKey(const std::string& name)
{
    m_Key = name;
}
//---------------------------------------------------------------------------
MHX2Reader::IItem* MHX2Reader::IItem::GetParent() const
{
    return m_pParent;
}
//---------------------------------------------------------------------------
void MHX2Reader::IItem::SetParent(IItem* pParent)
{
    m_pParent = pParent;
}
//---------------------------------------------------------------------------
bool MHX2Reader::IItem::GetIsArray() const
{
    return m_IsArray;
}
//---------------------------------------------------------------------------
void MHX2Reader::IItem::SetIsArray(bool value)
{
    m_IsArray = value;
}
//---------------------------------------------------------------------------
void MHX2Reader::IItem::AddChild(IItem* pChild)
{
    const std::size_t itemCount = m_Items.size();

    // check if item was already added, to not add it twice
    for (std::size_t i = 0; i < itemCount; ++i)
        if (m_Items[i] == pChild)
            return;

    m_Items.push_back(pChild);
}
//---------------------------------------------------------------------------
void MHX2Reader::IItem::DeleteChild(IItem* pChild)
{
    const std::size_t itemCount = m_Items.size();

    // check if item was already added, to not add it twice
    for (std::size_t i = 0; i < itemCount; ++i)
        if (m_Items[i] == pChild)
        {
            delete m_Items[i];
            m_Items.erase(m_Items.begin() + i);
            return;
        }
}
//---------------------------------------------------------------------------
void MHX2Reader::IItem::DeleteChildAt(std::size_t index)
{
    // is index out of bounds?
    if (index >= m_Items.size())
        return;

    delete m_Items[index];
    m_Items.erase(m_Items.begin() + index);
}
//---------------------------------------------------------------------------
MHX2Reader::IItem* MHX2Reader::IItem::GetChildAt(std::size_t index) const
{
    // is index out of bounds?
    if (index >= m_Items.size())
        return nullptr;

    return m_Items[index];
}
//---------------------------------------------------------------------------
std::size_t MHX2Reader::IItem::GetChildCount() const
{
    return m_Items.size();
}
//---------------------------------------------------------------------------
// MHX2Reader::IVector3
//---------------------------------------------------------------------------
MHX2Reader::IVector3::IVector3() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IVector3::IVector3(const std::string& name, const Vector3F& value) :
    IItem(name),
    m_Value(value)
{}
//---------------------------------------------------------------------------
MHX2Reader::IVector3::~IVector3()
{}
//---------------------------------------------------------------------------
Vector3F MHX2Reader::IVector3::GetValue() const
{
    return m_Value;
}
//---------------------------------------------------------------------------
void MHX2Reader::IVector3::SetValue(const Vector3F& value)
{
    m_Value = value;
}
//---------------------------------------------------------------------------
// MHX2Reader::IMatrix4x4
//---------------------------------------------------------------------------
MHX2Reader::IMatrix4x4::IMatrix4x4() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IMatrix4x4::IMatrix4x4(const std::string& name, const Matrix4x4F& value) :
    IItem(name),
    m_Value(value)
{}
//---------------------------------------------------------------------------
MHX2Reader::IMatrix4x4::~IMatrix4x4()
{}
//---------------------------------------------------------------------------
Matrix4x4F MHX2Reader::IMatrix4x4::GetValue() const
{
    return m_Value;
}
//---------------------------------------------------------------------------
void MHX2Reader::IMatrix4x4::SetValue(const Matrix4x4F& value)
{
    m_Value = value;
}
//---------------------------------------------------------------------------
// MHX2Reader
//---------------------------------------------------------------------------
MHX2Reader::MHX2Reader() :
    m_pRoot(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Reader::~MHX2Reader()
{
    if (m_pRoot)
        delete m_pRoot;
}
//---------------------------------------------------------------------------
bool MHX2Reader::Open(const std::string& fileName)
{
    // no file name?
    if (fileName.empty())
        return false;

    char*       pBuffer    = NULL;
    std::FILE*  pStream    = NULL;
    std::size_t fileSize   = 0;
    std::size_t bufferSize = 0;
    bool        success    = true;

    try
    {
        // open file for read
        #ifdef _WINDOWS
             const errno_t error = fopen_s(&pStream, fileName.c_str(), "rb");

             // error occurred?
             if (error != 0)
                 return false;
         #else
             pStream = std::fopen(fileName.c_str(), "rb");
         #endif

        // is file stream opened?
        if (!pStream)
            return false;

        // get file size
        std::fseek(pStream, 0, SEEK_END);
        fileSize = std::ftell(pStream);
        std::fseek(pStream, 0, SEEK_SET);

        // copy file content to buffer
        pBuffer    = new char[fileSize + 1];
        bufferSize = std::fread(pBuffer, 1, fileSize, pStream);
    }
    catch (...)
    {
        success = false;
    }

    // close cached xml file
    if (pStream)
        std::fclose(pStream);

    std::string data;

    try
    {
        // file read succeeded?
        if (success)
            // add readed data to output
            data += std::string((const char*)pBuffer, bufferSize);
    }
    catch (...)
    {
        success = false;
    }

    // delete buffer, if needed
    if (pBuffer)
        delete[] pBuffer;

    return (success && (bufferSize == fileSize) && Read(data));
}
//---------------------------------------------------------------------------
bool MHX2Reader::Read(const std::string& data)
{
    // delete any previously opened file
    if (m_pRoot)
    {
        delete m_pRoot;
        m_pRoot = nullptr;
    }

    char*           pErrorPos  = 0;
    const char*     pErrorDesc = 0;
    int             pErrorLine = 0;
    block_allocator allocator(1 << 10);

    // read the json data
    json_value* pJsonRoot = json_parse(const_cast<char*>(data.c_str()), &pErrorPos, &pErrorDesc, &pErrorLine, &allocator);

    // succeeded?
    if (!pJsonRoot || pJsonRoot->type != JSON_OBJECT)
        return false;

    return true;

//---------------------------------------------------------------------------
