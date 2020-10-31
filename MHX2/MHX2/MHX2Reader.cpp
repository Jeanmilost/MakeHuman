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

 //---------------------------------------------------------------------------
 // MHX2Reader::IItem
 //---------------------------------------------------------------------------
MHX2Reader::IItem::IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IItem::~IItem()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IItem::ParseVector(json_value* pJson, Vector3F& vector, std::size_t& index, std::string& error) const
{
    // no source data?
    if (!pJson)
    {
        error = "Parse vector - no source json";
        return false;
    }

    // is index out of bounds?
    if (index >= 3)
    {
        error = "Parse vector - index is out of bounds";
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
        {
            // read matrix line
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!ParseVector(it, vector, index, error))
                    return false;

            return true;
        }

        case JSON_INT:
            // read the next value. May be an int if value is 0 or 1
            switch (index)
            {
                case 0:  vector.m_X = float(pJson->int_value); break;
                case 1:  vector.m_Y = float(pJson->int_value); break;
                case 2:  vector.m_Z = float(pJson->int_value); break;
                default: return false;
            }

            ++index;
            return true;

        case JSON_FLOAT:
            // read the next value
            switch (index)
            {
                case 0:  vector.m_X = pJson->float_value; break;
                case 1:  vector.m_Y = pJson->float_value; break;
                case 2:  vector.m_Z = pJson->float_value; break;
                default: return false;
            }

            ++index;
            return true;
    }

    error = "Parse vector - unknown data type";
    return false;
}
//---------------------------------------------------------------------------
bool MHX2Reader::IItem::ParseMatrix(json_value* pJson, Matrix4x4F& matrix, std::size_t& x, std::size_t& y, std::string& error) const
{
    // no source data?
    if (!pJson)
    {
        error = "Parse matrix - no source json";
        return false;
    }

    // is x or y index out of bounds?
    if (x >= 4 || y >= 4)
    {
        error = "Parse matrix - index is out of bounds";
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
        {
            // read matrix line
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!ParseMatrix(it, matrix, x, y, error))
                    return false;

            // go to next line
            x = 0;
            ++y;
            return true;
        }

        case JSON_INT:
            // read the next value. May be an int if value is 0 or 1
            matrix.m_Table[y][x] = float(pJson->int_value);
            ++x;
            return true;

        case JSON_FLOAT:
            // read the next value
            matrix.m_Table[y][x] = pJson->float_value;
            ++x;
            return true;
    }

    error = "Parse matrix - unknown data type";
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IBone
//---------------------------------------------------------------------------
MHX2Reader::IBone::IBone() :
    IItem(),
    m_Roll(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Reader::IBone::~IBone()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IBone::Parse(json_value* pJson, std::string& error)
{
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
        {
            // search for name
            if (!pJson->name)
            {
                // if no name and only one child, assume that it's the bone class itself, so iterate through children
                for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    if (!Parse(it, error))
                        return false;

                return true;
            }
            else
            if (std::strcmp(pJson->name, "head") == 0)
            {
                std::size_t index = 0;
                return ParseVector(pJson, m_Head, index, error);
            }
            else
            if (std::strcmp(pJson->name, "tail") == 0)
            {
                std::size_t index = 0;
                return ParseVector(pJson, m_Tail, index, error);
            }
            else
            if (std::strcmp(pJson->name, "matrix") == 0)
            {
                std::size_t x = 0;
                std::size_t y = 0;

                return IItem::ParseMatrix(pJson, m_Matrix, x, y, error);
            }

            error = "Parse bone - unknown object or array";
            return false;
        }

        case JSON_STRING:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "name") == 0)
                {
                    m_Name = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "parent") == 0)
                {
                    m_Parent = pJson->string_value;
                    return true;
                }

            error = "Parse bone - unknown string value";
            return false;

        case JSON_INT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "roll") == 0)
                {
                    m_Roll = float(pJson->int_value);
                    return true;
                }

            error = "Parse bone - unknown int value";
            return false;

        case JSON_FLOAT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "roll") == 0)
                {
                    m_Roll = pJson->float_value;
                    return true;
                }

            error = "Parse bone - unknown float value";
            return false;
    }

    error = "Parse bone - unknown data type";
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::ISkeleton
//---------------------------------------------------------------------------
MHX2Reader::ISkeleton::ISkeleton() :
    IItem(),
    m_Scale(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Reader::ISkeleton::~ISkeleton()
{
    const std::size_t count = m_Bones.size();

    // iterate through bones and delete them
    for (std::size_t i = 0; i < count; ++i)
        delete m_Bones[i];
}
//---------------------------------------------------------------------------
bool MHX2Reader::ISkeleton::Parse(json_value* pJson, std::string& error)
{
    error = "Parse skeleton - unknown data type";
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader
//---------------------------------------------------------------------------
MHX2Reader::MHX2Reader() :
    m_pModel(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Reader::~MHX2Reader()
{
    if (m_pModel)
        delete m_pModel;
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
    // delete any previously opened model
    if (m_pModel)
    {
        delete m_pModel;
        m_pModel = nullptr;
    }

    char*           pErrorPos  = 0;
    const char*     pErrorDesc = 0;
    int             pErrorLine = 0;
    block_allocator allocator(1 << 10);

    // read the json data
    json_value* pJson = json_parse(const_cast<char*>(data.c_str()), &pErrorPos, &pErrorDesc, &pErrorLine, &allocator);

    // succeeded?
    if (!pJson || pJson->type != JSON_OBJECT)
        return false;

    // create the model
    m_pModel = new IModel();

    return Parse(pJson, m_pModel, IE_T_Model);
}
//---------------------------------------------------------------------------
/*REM
#include <Windows.h>
#include <sstream>
#define INDENT(n) for (int i = 0; i < n; ++i) ::OutputDebugStringA("    ")
void print(json_value* value, int indent = 0)
{
    INDENT(indent);
    if (value->name) ::OutputDebugStringA((std::string(value->name) + "=").c_str());
    switch (value->type)
    {
        case JSON_NULL:
            ::OutputDebugStringA("null\n");
            break;
        case JSON_OBJECT:
        case JSON_ARRAY:
            ::OutputDebugStringA(value->type == JSON_OBJECT ? "{\n" : "[\n");
            for (json_value* it = value->first_child; it; it = it->next_sibling)
            {
                print(it, indent + 1);
            }
            INDENT(indent);
            ::OutputDebugStringA(value->type == JSON_OBJECT ? "}\n" : "]\n");
            break;
        case JSON_STRING:
            ::OutputDebugStringA((std::string(value->string_value) + "\n").c_str());
            break;
        case JSON_INT:
        {
            std::ostringstream sstr;
            sstr << value->int_value;
            ::OutputDebugStringA((sstr.str() + "\n").c_str());
            break;
        }
        case JSON_FLOAT:
        {
            std::ostringstream sstr;
            sstr << value->float_value;
            ::OutputDebugStringA((sstr.str() + "\n").c_str());
            break;
        }
        case JSON_BOOL:
            ::OutputDebugStringA(value->int_value ? "true\n" : "false\n");
            break;
    }
}
*/
bool MHX2Reader::Parse(json_value* pJson, IModel* pModel, IEType type)
{
    switch (pJson->type)
    {
        case JSON_NULL:
            break;

        case JSON_OBJECT:
        case JSON_ARRAY:
        {
            IEType itemType = IE_T_Unknown;

            if (pJson->name)
                itemType = NameToType(std::string(pJson->name, std::strlen(pJson->name)));

            std::string test = "";

            switch (itemType)
            {
                case IE_T_Bones:
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IBone> pBone(new IBone());
                        std::string error;

                        if (!pBone->Parse(it, error))
                            return false;

                        std::string test = "";
                    }

                    return true;

                default:
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, pModel, itemType))
                            return false;

                    return true;
            }

            return false;
        }

        case JSON_STRING:
            return true;

        case JSON_INT:
            return true;

        case JSON_FLOAT:
            return true;

        case JSON_BOOL:
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
MHX2Reader::IEType MHX2Reader::NameToType(const std::string& name) const
{
    if (name == "license")
        return IE_T_License;
    else
    if (name == "skeleton")
        return IE_T_Skeleton;
    else
    if (name == "bones")
        return IE_T_Bones;
    else
    if (name == "matrix")
        return IE_T_Matrix;
    else
    if (name == "materials")
        return IE_T_Materials;
    else
    if (name == "geometries")
        return IE_T_Geometries;
    else
    if (name == "mesh")
        return IE_T_Mesh;
    else
    if (name == "seed_mesh")
        return IE_T_SeedMesh;
    else
    if (name == "proxy_seed_mesh")
        return IE_T_ProxySeedMesh;
    else
    if (name == "proxy")
        return IE_T_Proxy;
    else
    if (name == "weights")
        return IE_T_Weights;

    return IE_T_Unknown;
}
//---------------------------------------------------------------------------
