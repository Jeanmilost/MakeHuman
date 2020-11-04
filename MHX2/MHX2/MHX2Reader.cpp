/****************************************************************************
 * ==> MHX2Reader ----------------------------------------------------------*
 ****************************************************************************
 * Description : MakeHuman .mhx2 file reader                                *
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

#include "MHX2Reader.h"

 //---------------------------------------------------------------------------
 // MHX2Reader::ILogger
 //---------------------------------------------------------------------------
MHX2Reader::ILogger::ILogger()
{}
//---------------------------------------------------------------------------
MHX2Reader::ILogger::~ILogger()
{}
//---------------------------------------------------------------------------
void MHX2Reader::ILogger::Clear()
{
    m_Lines.clear();
}
//---------------------------------------------------------------------------
void MHX2Reader::ILogger::Log(const std::string& message)
{
    m_Lines.push_back(message);
}
//---------------------------------------------------------------------------
void MHX2Reader::ILogger::Log(json_value* pJson, const std::string& message)
{
    std::ostringstream sstr;

    // is json defined?
    if (pJson)
    {
        // json contains a name
        if (pJson->name)
        {
            // get the data key
            const std::string key = pJson->name;

            // log the message
            if (!key.empty())
                sstr << message << " - json - key - " << key << " - type - " << pJson->type;
            else
                sstr << message << " - json - type - " << pJson->type;
        }
    }
    else
        // log the message
        sstr << message;

    m_Lines.push_back(sstr.str());
}
//---------------------------------------------------------------------------
 // MHX2Reader::IItem
 //---------------------------------------------------------------------------
MHX2Reader::IItem::IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IItem::~IItem()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IItem::ParseColor(json_value* pJson, ColorF& color, std::size_t& index, ILogger& logger) const
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse color - json data source is missing");
        return false;
    }

    // is index out of bounds?
    if (index >= 4)
    {
        logger.Log(pJson, "Parse color - index is out of bounds", index);
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // read color values
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!ParseColor(it, color, index, logger))
                    return false;

            return true;

        case JSON_INT:
            // read the next value. May be an int if value is 0 or 1
            switch (index)
            {
                case 0:  color.m_R = float(pJson->int_value); break;
                case 1:  color.m_G = float(pJson->int_value); break;
                case 2:  color.m_B = float(pJson->int_value); break;
                case 3:  color.m_A = float(pJson->int_value); break;

                default:
                    logger.Log(pJson, "Parse color - index is out of bounds", index);
                    return false;
            }

            ++index;
            return true;

        case JSON_FLOAT:
            // read the next value
            switch (index)
            {
                case 0:  color.m_R = pJson->float_value; break;
                case 1:  color.m_G = pJson->float_value; break;
                case 2:  color.m_B = pJson->float_value; break;
                case 3:  color.m_A = pJson->float_value; break;

                default:
                    logger.Log(pJson, "Parse color - index is out of bounds", index);
                    return false;
            }

            ++index;
            return true;
    }

    logger.Log(pJson, "Parse color - unknown type");
    return false;
}
//---------------------------------------------------------------------------
bool MHX2Reader::IItem::ParseVector(json_value* pJson, Vector3F& vector, std::size_t& index, ILogger& logger) const
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse vector - json data source is missing");
        return false;
    }

    // is index out of bounds?
    if (index >= 3)
    {
        logger.Log(pJson, "Parse vector - index is out of bounds", index);
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // read vector values
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!ParseVector(it, vector, index, logger))
                    return false;

            return true;

        case JSON_INT:
            // read the next value. May be an int if value is 0 or 1
            switch (index)
            {
                case 0:  vector.m_X = float(pJson->int_value); break;
                case 1:  vector.m_Y = float(pJson->int_value); break;
                case 2:  vector.m_Z = float(pJson->int_value); break;

                default:
                    logger.Log(pJson, "Parse vector - index is out of bounds", index);
                    return false;
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

                default:
                    logger.Log(pJson, "Parse vector - index is out of bounds", index);
                    return false;
            }

            ++index;
            return true;
    }

    logger.Log(pJson, "Parse vector - unknown type");
    return false;
}
//---------------------------------------------------------------------------
bool MHX2Reader::IItem::ParseMatrix(json_value* pJson, Matrix4x4F& matrix, std::size_t& x, std::size_t& y, ILogger& logger) const
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse matrix - json data source is missing");
        return false;
    }

    // is x index out of bounds?
    if (x >= 4)
    {
        logger.Log(pJson, "Parse matrix - x index is out of bounds", x);
        return false;
    }

    // is y index out of bounds?
    if (y >= 4)
    {
        logger.Log(pJson, "Parse matrix - y index is out of bounds", y);
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // read matrix line
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!ParseMatrix(it, matrix, x, y, logger))
                    return false;

            // go to next line
            x = 0;
            ++y;
            return true;

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

    logger.Log(pJson, "Parse matrix - unknown type");
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
bool MHX2Reader::IBone::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse bone - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (!pJson->name)
            {
                // if no name, assume that it's the bone class itself, so iterate through children
                for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    if (!Parse(it, logger))
                        return false;

                return true;
            }
            else
            if (std::strcmp(pJson->name, "head") == 0)
            {
                std::size_t index = 0;
                return ParseVector(pJson, m_Head, index, logger);
            }
            else
            if (std::strcmp(pJson->name, "tail") == 0)
            {
                std::size_t index = 0;
                return ParseVector(pJson, m_Tail, index, logger);
            }
            else
            if (std::strcmp(pJson->name, "matrix") == 0)
            {
                std::size_t x = 0;
                std::size_t y = 0;

                return IItem::ParseMatrix(pJson, m_Matrix, x, y, logger);
            }

            logger.Log(pJson, "Parse bone - unknown value");
            return true;

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

            logger.Log(pJson, "Parse bone - unknown value");
            return true;

        case JSON_INT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "roll") == 0)
                {
                    m_Roll = float(pJson->int_value);
                    return true;
                }

            logger.Log(pJson, "Parse bone - unknown value");
            return true;

        case JSON_FLOAT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "roll") == 0)
                {
                    m_Roll = pJson->float_value;
                    return true;
                }

            logger.Log(pJson, "Parse bone - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse bone - unknown type");
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
bool MHX2Reader::ISkeleton::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse skeleton - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (pJson->name)
                if (std::strcmp(pJson->name, "skeleton") == 0)
                {
                    // skeleton class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "offset") == 0)
                {
                    std::size_t index = 0;
                    return ParseVector(pJson, m_Offset, index, logger);
                }
                else
                if (std::strcmp(pJson->name, "bones") == 0)
                {
                    // bone array, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IBone> pBone(new IBone());

                        if (!pBone->Parse(it, logger))
                            return false;

                        m_Bones.push_back(pBone.get());
                        pBone.release();
                    }

                    return true;
                }

            logger.Log(pJson, "Parse skeleton - unknown value");
            return true;

        case JSON_STRING:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "name") == 0)
                {
                    m_Name = pJson->string_value;
                    return true;
                }

            logger.Log(pJson, "Parse skeleton - unknown value");
            return true;

        case JSON_INT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "scale") == 0)
                {
                    m_Scale = float(pJson->int_value);
                    return true;
                }

            logger.Log(pJson, "Parse skeleton - unknown value");
            return true;

        case JSON_FLOAT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "scale") == 0)
                {
                    m_Scale = pJson->float_value;
                    return true;
                }

            logger.Log(pJson, "Parse skeleton - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse bone - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IMaterial
//---------------------------------------------------------------------------
MHX2Reader::IMaterial::IMaterial() :
    IItem(),
    m_Ambient(ColorF(1.0f, 1.0f, 1.0f, 1.0f)),
    m_Diffuse(ColorF(1.0f, 1.0f, 1.0f, 1.0f)),
    m_Emissive(ColorF(1.0f, 1.0f, 1.0f, 1.0f)),
    m_DiffuseMapIntensity(1.0f),
    m_SpecularMapIntensity(1.0f),
    m_TransparencyMapIntensity(1.0f),
    m_Shininess(0.0f),
    m_Opacity(1.0f),
    m_Translucency(0.0f),
    m_SssRScale(1.0f),
    m_SssGScale(1.0f),
    m_SssBScale(1.0f),
    m_Shadeless(false),
    m_Wireframe(false),
    m_Transparent(false),
    m_AlphaToCoverage(false),
    m_BackfaceCull(false),
    m_Depthless(false),
    m_CastShadows(false),
    m_ReceiveShadows(false),
    m_SssEnabled(false)
{}
//---------------------------------------------------------------------------
MHX2Reader::IMaterial::~IMaterial()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IMaterial::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse material - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (!pJson->name)
            {
                // if no name, assume that it's the material class itself, so iterate through children
                for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    if (!Parse(it, logger))
                        return false;

                return true;
            }
            else
            if (std::strcmp(pJson->name, "diffuse_color") == 0)
            {
                std::size_t index = 0;
                return ParseColor(pJson, m_Diffuse, index, logger);
            }
            else
            if (std::strcmp(pJson->name, "specular_color") == 0)
            {
                std::size_t index = 0;
                return ParseColor(pJson, m_Specular, index, logger);
            }
            else
            if (std::strcmp(pJson->name, "emissive_color") == 0)
            {
                std::size_t index = 0;
                return ParseColor(pJson, m_Emissive, index, logger);
            }
            else
            if (std::strcmp(pJson->name, "ambient_color") == 0)
            {
                std::size_t index = 0;
                return ParseColor(pJson, m_Ambient, index, logger);
            }

            logger.Log(pJson, "Parse material - unknown value");
            return true;

        case JSON_STRING:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "name") == 0)
                {
                    m_Name = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "diffuse_texture") == 0)
                {
                    m_DiffuseTexture = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "normal_map_texture") == 0)
                {
                    m_NormalMapTexture = pJson->string_value;
                    return true;
                }

            logger.Log(pJson, "Parse material - unknown value");
            return true;

        case JSON_INT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "diffuse_map_intensity") == 0)
                {
                    m_DiffuseMapIntensity = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "specular_map_intensity") == 0)
                {
                    m_SpecularMapIntensity = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "transparency_map_intensity") == 0)
                {
                    m_TransparencyMapIntensity = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "shininess") == 0)
                {
                    m_Shininess = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "opacity") == 0)
                {
                    m_Opacity = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "translucency") == 0)
                {
                    m_Translucency = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssRScale") == 0)
                {
                    m_SssRScale = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssGScale") == 0)
                {
                    m_SssGScale = float(pJson->int_value);
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssBScale") == 0)
                {
                    m_SssBScale = float(pJson->int_value);
                    return true;
                }

            logger.Log(pJson, "Parse material - unknown value");
            return true;

        case JSON_FLOAT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "diffuse_map_intensity") == 0)
                {
                    m_DiffuseMapIntensity = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "specular_map_intensity") == 0)
                {
                    m_SpecularMapIntensity = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "transparency_map_intensity") == 0)
                {
                    m_TransparencyMapIntensity = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "shininess") == 0)
                {
                    m_Shininess = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "opacity") == 0)
                {
                    m_Opacity = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "translucency") == 0)
                {
                    m_Translucency = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssRScale") == 0)
                {
                    m_SssRScale = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssGScale") == 0)
                {
                    m_SssGScale = pJson->float_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssBScale") == 0)
                {
                    m_SssBScale = pJson->float_value;
                    return true;
                }

            logger.Log(pJson, "Parse material - unknown value");
            return true;

        case JSON_BOOL:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "shadeless") == 0)
                {
                    m_Shadeless = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "wireframe") == 0)
                {
                    m_Wireframe = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "transparent") == 0)
                {
                    m_Transparent = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "alphaToCoverage") == 0)
                {
                    m_AlphaToCoverage = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "backfaceCull") == 0)
                {
                    m_BackfaceCull = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "depthless") == 0)
                {
                    m_Depthless = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "castShadows") == 0)
                {
                    m_CastShadows = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "receiveShadows") == 0)
                {
                    m_ReceiveShadows = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "sssEnabled") == 0)
                {
                    m_SssEnabled = pJson->int_value;
                    return true;
                }

            logger.Log(pJson, "Parse material - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse material - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::ILicense
//---------------------------------------------------------------------------
MHX2Reader::ILicense::ILicense() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::ILicense::~ILicense()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::ILicense::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse license - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (pJson->name)
                if (std::strcmp(pJson->name, "license") == 0)
                {
                    // license class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }

            logger.Log(pJson, "Parse license - unknown value");
            return true;

        case JSON_STRING:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "author") == 0)
                {
                    m_Author = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "license") == 0)
                {
                    m_License = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "homepage") == 0)
                {
                    m_Homepage = pJson->string_value;
                    return true;
                }

            logger.Log(pJson, "Parse license - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse license - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IFace
//---------------------------------------------------------------------------
MHX2Reader::IFace::IFace() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IFace::~IFace()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IFace::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse face - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // iterate through children, each of them contains a face value
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!Parse(it, logger))
                    return false;

            return true;

        case JSON_INT:
            m_Values.push_back(float(pJson->int_value));
            return true;

        case JSON_FLOAT:
            m_Values.push_back(pJson->float_value);
            return true;
    }

    logger.Log(pJson, "Parse face - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IUVCoord
//---------------------------------------------------------------------------
MHX2Reader::IUVCoord::IUVCoord() :
    IItem(),
    m_X(0.0f),
    m_Y(0.0f),
    m_InternalIndex(0)
{}
//---------------------------------------------------------------------------
MHX2Reader::IUVCoord::~IUVCoord()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IUVCoord::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse uv coords - json data source is missing");
        return false;
    }

    // is index out of bounds?
    if (m_InternalIndex >= 2)
    {
        logger.Log(pJson, "Parse uv coords - index is out of bounds", m_InternalIndex);
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // iterate through children, each of them contains an uv coodrinate value
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!Parse(it, logger))
                    return false;

            return true;

        case JSON_INT:
            // read the value
            switch (m_InternalIndex)
            {
                case 0: m_X = float(pJson->int_value); break;
                case 1: m_Y = float(pJson->int_value); break;

                default:
                    logger.Log(pJson, "Parse uv coords - index is out of bounds", m_InternalIndex);
                    return false;
            }

            ++m_InternalIndex;
            return true;

        case JSON_FLOAT:
            // read the value
            switch (m_InternalIndex)
            {
                case 0: m_X = pJson->float_value; break;
                case 1: m_Y = pJson->float_value; break;

                default:
                    logger.Log(pJson, "Parse uv coords - index is out of bounds", m_InternalIndex);
                    return false;
            }

            ++m_InternalIndex;
            return true;
    }

    logger.Log(pJson, "Parse uv coords - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IWeight
//---------------------------------------------------------------------------
MHX2Reader::IWeight::IWeight() :
    IItem(),
    m_Index(0),
    m_InternalIndex(0),
    m_Value(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Reader::IWeight::~IWeight()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IWeight::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse weight - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // iterate through children, each of them contains a weight
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!Parse(it, logger))
                    return false;

            return true;

        case JSON_INT:
            switch (m_InternalIndex)
            {
                case 0: m_Index =       pJson->int_value;  break;
                case 1: m_Value = float(pJson->int_value); break;

                default:
                    logger.Log(pJson, "Parse weight - index is out of bounds", m_InternalIndex);
                    return false;
            }

            ++m_InternalIndex;
            return true;

        case JSON_FLOAT:
            switch (m_InternalIndex)
            {
                case 0: m_Index = std::size_t(pJson->float_value); break;
                case 1: m_Value =             pJson->float_value;  break;

                default:
                    logger.Log(pJson, "Parse weight - index is out of bounds", m_InternalIndex);
                    return false;
            }

            ++m_InternalIndex;
            return true;
    }

    logger.Log(pJson, "Parse weight - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IWeightGroup
//---------------------------------------------------------------------------
MHX2Reader::IWeightGroup::IWeightGroup() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IWeightGroup::~IWeightGroup()
{
    const std::size_t weightCount = m_Weights.size();

    // iterate through weights and delete them
    for (std::size_t i = 0; i < weightCount; ++i)
        delete m_Weights[i];
}
//---------------------------------------------------------------------------
bool MHX2Reader::IWeightGroup::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse weight group - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // keep the name, it's the key to retrieve the linked bone
            if (pJson->name)
                m_Key = pJson->name;

            // iterate through children, each of them contains a weight group
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
            {
                std::unique_ptr<IWeight> pWeight(new IWeight());

                if (!pWeight->Parse(it, logger))
                    return false;

                m_Weights.push_back(pWeight.get());
                pWeight.release();
            }

            return true;
    }

    logger.Log(pJson, "Parse face - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IFit
//---------------------------------------------------------------------------
MHX2Reader::IFit::IFit() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IFit::~IFit()
{
    const std::size_t count = m_Values.size();

    // iterate through fits and delete them
    for (std::size_t i = 0; i < count; ++i)
        delete m_Values[i];
}
//---------------------------------------------------------------------------
bool MHX2Reader::IFit::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse fit - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // read the generic children
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
            {
                std::unique_ptr<Vector3F> pVertex(new Vector3F());
                std::size_t index = 0;

                // read the vertex
                if (!ParseVector(it, *pVertex.get(), index, logger))
                    return false;

                m_Values.push_back(pVertex.get());
                pVertex.release();
            }

            return true;
    }

    logger.Log(pJson, "Parse proxy - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IMesh
//---------------------------------------------------------------------------
MHX2Reader::IMesh::IMesh() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IMesh::~IMesh()
{
    const std::size_t vertCount = m_Vertices.size();

    // iterate through vertices and delete them
    for (std::size_t i = 0; i < vertCount; ++i)
        delete m_Vertices[i];

    const std::size_t faceCount = m_Faces.size();

    // iterate through faces and delete them
    for (std::size_t i = 0; i < faceCount; ++i)
        delete m_Faces[i];

    const std::size_t uvCount = m_UVCoords.size();

    // iterate through uv coordinates and delete them
    for (std::size_t i = 0; i < uvCount; ++i)
        delete m_UVCoords[i];

    const std::size_t uvFaceCount = m_UVFaces.size();

    // iterate through uv faces and delete them
    for (std::size_t i = 0; i < uvFaceCount; ++i)
        delete m_UVFaces[i];

    const std::size_t weightGroupCount = m_WeightGroups.size();

    // iterate through weight groups and delete them
    for (std::size_t i = 0; i < weightGroupCount; ++i)
        delete m_WeightGroups[i];
}
//---------------------------------------------------------------------------
bool MHX2Reader::IMesh::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse mesh - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (pJson->name)
                if (std::strcmp(pJson->name, "mesh") == 0)
                {
                    // mesh class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "seed_mesh") == 0)
                {
                    // seed mesh class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "proxy_seed_mesh") == 0)
                {
                    // proxy seed mesh class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "vertices") == 0)
                {
                    // vertices, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<Vector3F> pVertex(new Vector3F());
                        std::size_t index = 0;

                        if (!ParseVector(it, *(pVertex.get()), index, logger))
                            return false;

                        m_Vertices.push_back(pVertex.get());
                        pVertex.release();
                    }

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "faces") == 0)
                {
                    // faces, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IFace> pFace(new IFace());

                        if (!pFace->Parse(it, logger))
                            return false;

                        m_Faces.push_back(pFace.get());
                        pFace.release();
                    }

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "uv_coordinates") == 0)
                {
                    // uv coordinates, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IUVCoord> pUvCoord(new IUVCoord());

                        if (!pUvCoord->Parse(it, logger))
                            return false;

                        m_UVCoords.push_back(pUvCoord.get());
                        pUvCoord.release();
                    }

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "uv_faces") == 0)
                {
                    // uv faces, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IFace> pUvFace(new IFace());

                        if (!pUvFace->Parse(it, logger))
                            return false;

                        m_UVFaces.push_back(pUvFace.get());
                        pUvFace.release();
                    }

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "weights") == 0)
                {
                    // weight groups, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IWeightGroup> pWeightGroup(new IWeightGroup());

                        if (!pWeightGroup->Parse(it, logger))
                            return false;

                        m_WeightGroups.push_back(pWeightGroup.get());
                        pWeightGroup.release();
                    }

                    return true;
                }

            logger.Log(pJson, "Parse mesh - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse mesh - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IProxy
//---------------------------------------------------------------------------
MHX2Reader::IProxy::IProxy() :
    IItem(),
    m_pVertexBoneWeights(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Reader::IProxy::~IProxy()
{
    const std::size_t fitCount = m_Fitting.size();

    // iterate through fits and delete them
    for (std::size_t i = 0; i < fitCount; ++i)
        delete m_Fitting[i];

    if (m_pVertexBoneWeights)
        delete m_pVertexBoneWeights;
}
//---------------------------------------------------------------------------
bool MHX2Reader::IProxy::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse proxy - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (pJson->name)
                if (std::strcmp(pJson->name, "proxy") == 0)
                {
                    // mesh class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "license") == 0)
                    return m_License.Parse(pJson, logger);
                else
                if (std::strcmp(pJson->name, "tags") == 0)
                {
                    // mesh class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "fitting") == 0)
                {
                    // iterate through fits and read each of them
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IFit> pFit(new IFit());

                        // read the fit
                        if (!pFit->Parse(it, logger))
                            return false;

                        m_Fitting.push_back(pFit.get());
                        pFit.release();
                    }

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "delete_verts") == 0)
                {
                    // mesh class, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                        if (!Parse(it, logger))
                            return false;

                    return true;
                }

            // read the generic children
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!Parse(it, logger))
                    return false;

            logger.Log(pJson, "Parse proxy - unknown value");
            return true;

        case JSON_STRING:
            // read the value
            if (!pJson->name)
            {
                // no name, assume it's a tag
                m_Tags.push_back(pJson->string_value);
                return true;
            }
            else
            if (std::strcmp(pJson->name, "name") == 0)
            {
                m_Name = pJson->string_value;
                return true;
            }
            else
            if (std::strcmp(pJson->name, "type") == 0)
            {
                m_Type = pJson->string_value;
                return true;
            }
            else
            if (std::strcmp(pJson->name, "uuid") == 0)
            {
                m_Uuid = pJson->string_value;
                return true;
            }
            else
            if (std::strcmp(pJson->name, "basemesh") == 0)
            {
                m_Basemesh = pJson->string_value;
                return true;
            }

            logger.Log(pJson, "Parse proxy - unknown value");
            return true;

        case JSON_BOOL:
            // read the value
            if (!pJson->name)
            {
                // no name, assume it's a delete vertex value
                m_DeleteVerts.push_back(pJson->int_value);
                return true;
            }

            logger.Log(pJson, "Parse proxy - unknown value");
            return true;

        case JSON_NULL:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "vertex_bone_weights") == 0)
                {
                    m_pVertexBoneWeights = nullptr;
                    return true;
                }

            logger.Log(pJson, "Parse proxy - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse proxy - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IGeometry
//---------------------------------------------------------------------------
MHX2Reader::IGeometry::IGeometry() :
    IItem(),
    m_Scale(1.0f),
    m_IsHuman(true),
    m_IsSubdivided(false)
{}
//---------------------------------------------------------------------------
MHX2Reader::IGeometry::~IGeometry()
{}
//---------------------------------------------------------------------------
bool MHX2Reader::IGeometry::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse geometry - json data source is missing");
        return false;
    }

    // dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (!pJson->name)
            {
                // if no name, assume that it's the geometry class itself, so iterate through children
                for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    if (!Parse(it, logger))
                        return false;

                return true;
            }
            else
            if (std::strcmp(pJson->name, "license") == 0)
                return m_License.Parse(pJson, logger);
            else
            if (std::strcmp(pJson->name, "offset") == 0)
            {
                std::size_t index = 0;
                return ParseVector(pJson, m_Offset, index, logger);
            }
            else
            if (std::strcmp(pJson->name, "mesh") == 0)
                return m_Mesh.Parse(pJson, logger);
            else
            if (std::strcmp(pJson->name, "seed_mesh") == 0)
                return m_SeedMesh.Parse(pJson, logger);
            else
            if (std::strcmp(pJson->name, "proxy_seed_mesh") == 0)
                return m_ProxySeedMesh.Parse(pJson, logger);
            else
            if (std::strcmp(pJson->name, "proxy") == 0)
                return m_Proxy.Parse(pJson, logger);

            logger.Log(pJson, "Parse geometry - unknown value");
            return true;

        case JSON_STRING:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "name") == 0)
                {
                    m_Name = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "uuid") == 0)
                {
                    m_Uuid = pJson->string_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "material") == 0)
                {
                    m_Material = pJson->string_value;
                    return true;
                }

            logger.Log(pJson, "Parse geometry - unknown value");
            return true;

        case JSON_INT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "scale") == 0)
                {
                    m_Scale = float(pJson->int_value);
                    return true;
                }

            logger.Log(pJson, "Parse geometry - unknown value");
            return true;

        case JSON_FLOAT:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "scale") == 0)
                {
                    m_Scale = pJson->float_value;
                    return true;
                }

            logger.Log(pJson, "Parse geometry - unknown value");
            return true;

        case JSON_BOOL:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "issubdivided") == 0)
                {
                    m_IsSubdivided = pJson->int_value;
                    return true;
                }
                else
                if (std::strcmp(pJson->name, "human") == 0)
                {
                    m_IsHuman = pJson->int_value;
                    return true;
                }

            logger.Log(pJson, "Parse geometry - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse geometry - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Reader::IModel
//---------------------------------------------------------------------------
MHX2Reader::IModel::IModel() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Reader::IModel::~IModel()
{
    const std::size_t materialCount = m_Materials.size();

    // iterate through bones materials delete them
    for (std::size_t i = 0; i < materialCount; ++i)
        delete m_Materials[i];

    const std::size_t geometryCount = m_Geometries.size();

    // iterate through geometries and delete them
    for (std::size_t i = 0; i < geometryCount; ++i)
        delete m_Geometries[i];
}
//---------------------------------------------------------------------------
bool MHX2Reader::IModel::Parse(json_value* pJson, ILogger& logger)
{
    // no source data?
    if (!pJson)
    {
        logger.Log("Parse model - json data source is missing");
        return false;
    }

    // Dispatch json type
    switch (pJson->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
            // search for name
            if (pJson->name)
                if (std::strcmp(pJson->name, "skeleton") == 0)
                {
                    std::string error;

                    if (!m_Skeleton.Parse(pJson, logger))
                        return false;

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "materials") == 0)
                {
                    // material array, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IMaterial> pMaterial(new IMaterial());

                        if (!pMaterial->Parse(it, logger))
                            return false;

                        m_Materials.push_back(pMaterial.get());
                        pMaterial.release();
                    }

                    return true;
                }
                else
                if (std::strcmp(pJson->name, "geometries") == 0)
                {
                    // geometry array, iterate through children
                    for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                    {
                        std::unique_ptr<IGeometry> pGeometry(new IGeometry());

                        if (!pGeometry->Parse(it, logger))
                            return false;

                        m_Geometries.push_back(pGeometry.get());
                        pGeometry.release();
                    }

                    return true;
                }

            // read the generic children
            for (json_value* it = pJson->first_child; it; it = it->next_sibling)
                if (!Parse(it, logger))
                    return false;

            return true;

        case JSON_STRING:
            // read the value
            if (pJson->name)
                if (std::strcmp(pJson->name, "mhx2_version") == 0)
                {
                    m_Version = pJson->string_value;
                    return true;
                }

            logger.Log(pJson, "Parse model - unknown value");
            return true;
    }

    logger.Log(pJson, "Parse model - unknown type");
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

    // clear the previous log
    m_Logger.Clear();

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

    return m_pModel->Parse(pJson, m_Logger);
}
//---------------------------------------------------------------------------
