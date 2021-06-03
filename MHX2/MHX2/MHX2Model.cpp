/****************************************************************************
 * ==> MHX2Model -----------------------------------------------------------*
 ****************************************************************************
 * Description : MakeHuman .mhx2 model                                      *
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

#include "MHX2Model.h"

//---------------------------------------------------------------------------
// MHX2Model::ILogger
//---------------------------------------------------------------------------
MHX2Model::ILogger::ILogger()
{}
//---------------------------------------------------------------------------
MHX2Model::ILogger::~ILogger()
{}
//---------------------------------------------------------------------------
void MHX2Model::ILogger::Clear()
{
    m_Lines.clear();
}
//---------------------------------------------------------------------------
void MHX2Model::ILogger::Log(const std::string& message)
{
    m_Lines.push_back(message);
}
//---------------------------------------------------------------------------
void MHX2Model::ILogger::Log(json_value* pJson, const std::string& message)
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
 // MHX2Model::IItem
 //---------------------------------------------------------------------------
MHX2Model::IItem::IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::IItem::~IItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IItem::ParseColor(json_value* pJson, ColorF& color, std::size_t& index, ILogger& logger) const
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
bool MHX2Model::IItem::ParseVector(json_value* pJson, Vector3F& vector, std::size_t& index, ILogger& logger) const
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
bool MHX2Model::IItem::ParseMatrix(json_value* pJson, Matrix4x4F& matrix, std::size_t& x, std::size_t& y, ILogger& logger) const
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
            matrix.m_Table[x][y] = float(pJson->int_value);
            ++x;
            return true;

        case JSON_FLOAT:
            // read the next value
            matrix.m_Table[x][y] = pJson->float_value;
            ++x;
            return true;
    }

    logger.Log(pJson, "Parse matrix - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Model::IBoneItem
//---------------------------------------------------------------------------
MHX2Model::IBoneItem::IBoneItem() :
    IItem(),
    m_Roll(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Model::IBoneItem::~IBoneItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IBoneItem::Parse(json_value* pJson, ILogger& logger)
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
// MHX2Model::ISkeletonItem
//---------------------------------------------------------------------------
MHX2Model::ISkeletonItem::ISkeletonItem() :
    IItem(),
    m_Scale(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Model::ISkeletonItem::~ISkeletonItem()
{
    const std::size_t count = m_Bones.size();

    // iterate through bones and delete them
    for (std::size_t i = 0; i < count; ++i)
        delete m_Bones[i];
}
//---------------------------------------------------------------------------
bool MHX2Model::ISkeletonItem::Parse(json_value* pJson, ILogger& logger)
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
                        std::unique_ptr<IBoneItem> pBone(new IBoneItem());

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
// MHX2Model::IMaterialItem
//---------------------------------------------------------------------------
MHX2Model::IMaterialItem::IMaterialItem() :
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
MHX2Model::IMaterialItem::~IMaterialItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IMaterialItem::Parse(json_value* pJson, ILogger& logger)
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
// MHX2Model::ILicenseItem
//---------------------------------------------------------------------------
MHX2Model::ILicenseItem::ILicenseItem() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::ILicenseItem::~ILicenseItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::ILicenseItem::Parse(json_value* pJson, ILogger& logger)
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
// MHX2Model::IFaceItem
//---------------------------------------------------------------------------
MHX2Model::IFaceItem::IFaceItem() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::IFaceItem::~IFaceItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IFaceItem::Parse(json_value* pJson, ILogger& logger)
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
            m_Values.push_back(pJson->int_value);
            return true;

        case JSON_FLOAT:
            m_Values.push_back((std::size_t)pJson->float_value);
            return true;
    }

    logger.Log(pJson, "Parse face - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Model::IUVCoordItem
//---------------------------------------------------------------------------
MHX2Model::IUVCoordItem::IUVCoordItem() :
    IItem(),
    m_InternalIndex(0)
{}
//---------------------------------------------------------------------------
MHX2Model::IUVCoordItem::~IUVCoordItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IUVCoordItem::Parse(json_value* pJson, ILogger& logger)
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
                case 0: m_Value.m_X = float(pJson->int_value); break;
                case 1: m_Value.m_Y = float(pJson->int_value); break;

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
                case 0: m_Value.m_X = pJson->float_value; break;
                case 1: m_Value.m_Y = pJson->float_value; break;

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
// MHX2Model::IWeightItem
//---------------------------------------------------------------------------
MHX2Model::IWeightItem::IWeightItem() :
    IItem(),
    m_Index(0),
    m_InternalIndex(0),
    m_Value(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Model::IWeightItem::~IWeightItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IWeightItem::Parse(json_value* pJson, ILogger& logger)
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
// MHX2Model::IWeightGroupItem
//---------------------------------------------------------------------------
MHX2Model::IWeightGroupItem::IWeightGroupItem() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::IWeightGroupItem::~IWeightGroupItem()
{
    const std::size_t weightCount = m_Weights.size();

    // iterate through weights and delete them
    for (std::size_t i = 0; i < weightCount; ++i)
        delete m_Weights[i];
}
//---------------------------------------------------------------------------
bool MHX2Model::IWeightGroupItem::Parse(json_value* pJson, ILogger& logger)
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
                std::unique_ptr<IWeightItem> pWeight(new IWeightItem());

                if (!pWeight->Parse(it, logger))
                    return false;

                // build the weight table. NOTE it's assumed that a weight is defined only once per bone influence group
                m_Table[pWeight->m_Index] = pWeight->m_Value;

                // add the weight to the bone influence group list
                m_Weights.push_back(pWeight.get());
                pWeight.release();
            }

            return true;
    }

    logger.Log(pJson, "Parse face - unknown type");
    return false;
}
//---------------------------------------------------------------------------
// MHX2Model::IFitItem
//---------------------------------------------------------------------------
MHX2Model::IFitItem::IFitItem() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::IFitItem::~IFitItem()
{
    const std::size_t count = m_Values.size();

    // iterate through fits and delete them
    for (std::size_t i = 0; i < count; ++i)
        delete m_Values[i];
}
//---------------------------------------------------------------------------
bool MHX2Model::IFitItem::Parse(json_value* pJson, ILogger& logger)
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
// MHX2Model::IMeshItem
//---------------------------------------------------------------------------
MHX2Model::IMeshItem::IMeshItem() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::IMeshItem::~IMeshItem()
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
bool MHX2Model::IMeshItem::Parse(json_value* pJson, ILogger& logger)
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
                        std::unique_ptr<IFaceItem> pFace(new IFaceItem());

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
                        std::unique_ptr<IUVCoordItem> pUvCoord(new IUVCoordItem());

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
                        std::unique_ptr<IFaceItem> pUvFace(new IFaceItem());

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
                        std::unique_ptr<IWeightGroupItem> pWeightGroup(new IWeightGroupItem());

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
// MHX2Model::IProxyItem
//---------------------------------------------------------------------------
MHX2Model::IProxyItem::IProxyItem() :
    IItem(),
    m_pVertexBoneWeights(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Model::IProxyItem::~IProxyItem()
{
    const std::size_t fitCount = m_Fitting.size();

    // iterate through fits and delete them
    for (std::size_t i = 0; i < fitCount; ++i)
        delete m_Fitting[i];

    if (m_pVertexBoneWeights)
        delete m_pVertexBoneWeights;
}
//---------------------------------------------------------------------------
bool MHX2Model::IProxyItem::Parse(json_value* pJson, ILogger& logger)
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
                        std::unique_ptr<IFitItem> pFit(new IFitItem());

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
// MHX2Model::IGeometryItem
//---------------------------------------------------------------------------
MHX2Model::IGeometryItem::IGeometryItem() :
    IItem(),
    m_Scale(1.0f),
    m_IsHuman(true),
    m_IsSubdivided(false)
{}
//---------------------------------------------------------------------------
MHX2Model::IGeometryItem::~IGeometryItem()
{}
//---------------------------------------------------------------------------
bool MHX2Model::IGeometryItem::Parse(json_value* pJson, ILogger& logger)
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
// MHX2Model::IModelItem
//---------------------------------------------------------------------------
MHX2Model::IModelItem::IModelItem() :
    IItem()
{}
//---------------------------------------------------------------------------
MHX2Model::IModelItem::~IModelItem()
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
bool MHX2Model::IModelItem::Parse(json_value* pJson, ILogger& logger)
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
                        std::unique_ptr<IMaterialItem> pMaterial(new IMaterialItem());

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
                        std::unique_ptr<IGeometryItem> pGeometry(new IGeometryItem());

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
// MHX2Model
//---------------------------------------------------------------------------
MHX2Model::MHX2Model() :
    m_pModel(nullptr),
    m_PoseOnly(false),
    m_fOnGetVertexColor(nullptr),
    m_fOnLoadTexture(nullptr)
{
    // configure the default vertex format
    m_VertFormatTemplate.m_Format = (VertexFormat::IEFormat)((unsigned)VertexFormat::IEFormat::IE_VF_Colors |
                                                             (unsigned)VertexFormat::IEFormat::IE_VF_TexCoords);

    // configure the default vertex culling
    m_VertCullingTemplate.m_Type = VertexCulling::IECullingType::IE_CT_Back;
    m_VertCullingTemplate.m_Face = VertexCulling::IECullingFace::IE_CF_CCW;

    // configure the default material
    m_MaterialTemplate.m_Color = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
}
//---------------------------------------------------------------------------
MHX2Model::~MHX2Model()
{
    if (m_pModel)
        delete m_pModel;
}
//---------------------------------------------------------------------------
bool MHX2Model::Open(const std::string& fileName)
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
            // add read data to output
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
bool MHX2Model::Read(const std::string& data)
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

    // create the mhx2 model item
    std::unique_ptr<IModelItem> pModelItem(new IModelItem());

    // parse the model
    if (!pModelItem->Parse(pJson, m_Logger))
        return false;

    // create the mhx2 model
    std::unique_ptr<Model> pModel(new Model());

    // build the model skeleton
    if (!BuildSkeleton(pModelItem->m_Skeleton, pModel.get()))
        return false;

    const std::size_t geometryCount = pModelItem->m_Geometries.size();

    // build the model geometries
    for (std::size_t i = 0; i < geometryCount; ++i)
        if (!BuildGeometry(pModelItem.get(), pModelItem->m_Geometries[i], pModel.get()))
            return false;

    // to show only the pose without animation
    pModel->m_PoseOnly = m_PoseOnly;

    m_pModel = pModel.release();
    return true;
}
//---------------------------------------------------------------------------
Model* MHX2Model::GetModel(int animSetIndex, double elapsedTime) const
{
    // no model?
    if (!m_pModel)
        return nullptr;

    // if mesh has no skeleton, perform a simple draw
    if (!m_pModel->m_pSkeleton)
        return m_pModel;

    // clear the animation matrix cache
    const_cast<IAnimBoneCacheDict&>(m_AnimBoneCacheDict).clear();

    const std::size_t meshCount = m_pModel->m_Mesh.size();

    // iterate through model meshes
    for (std::size_t i = 0; i < meshCount; ++i)
    {
        // get model mesh
        Mesh* pMesh = m_pModel->m_Mesh[i];

        // found it?
        if (!pMesh)
            continue;

        // normally each mesh should contain only one vertex buffer
        if (pMesh->m_VB.size() != 1)
            // unsupported if not (because cannot know which texture should be binded. If a such model
            // exists, a custom version of this function should also be written for it)
            continue;

        // malformed deformers?
        if (meshCount != m_pModel->m_Deformers.size())
            return nullptr;

        const std::size_t weightCount = m_pModel->m_Deformers[i]->m_SkinWeights.size();

        // mesh contains skin weights?
        if (!weightCount)
            return nullptr;

        // clear the previous vertex buffer vertices in order to rebuild them
        for (std::size_t j = 0; j < pMesh->m_VB[0]->m_Data.size(); j += pMesh->m_VB[0]->m_Format.m_Stride)
        {
            pMesh->m_VB[0]->m_Data[j]     = 0.0f;
            pMesh->m_VB[0]->m_Data[j + 1] = 0.0f;
            pMesh->m_VB[0]->m_Data[j + 2] = 0.0f;
        }

        // iterate through mesh skin weights
        for (std::size_t j = 0; j < weightCount; ++j)
        {
            Matrix4x4F boneMatrix;

            // get the bone matrix
            if (m_pModel->m_PoseOnly)
                // in mhx2 files, the bones matrix are pre-calculated, so don't call the pModel->GetBoneMatrix() function
                boneMatrix = m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_pBone->m_Matrix;
            /*
            else
                GetBoneAnimMatrix(m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_pBone,
                                  m_pModel->m_AnimationSet[animSetIndex],
                                  std::fmod(elapsedTime, (double)m_pModel->m_AnimationSet[animSetIndex]->m_MaxValue / 46186158000.0),
                                  Matrix4x4F::Identity(),
                                  boneMatrix);
            */

            // get the final matrix after bones transform
            const Matrix4x4F finalMatrix = m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_Matrix.Multiply(boneMatrix);

            // get the weight influence count
            const std::size_t weightInfluenceCount = m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_WeightInfluences.size();

            // apply the bone and its skin weights to each vertices
            for (std::size_t k = 0; k < weightInfluenceCount; ++k)
            {
                // get the vertex index count
                const std::size_t vertexIndexCount =
                        m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_WeightInfluences[k]->m_VertexIndex.size();

                // iterate through weights influences vertex indices
                for (std::size_t l = 0; l < vertexIndexCount; ++l)
                {
                    // get the next vertex to which the next skin weight should be applied
                    const std::size_t iX = m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_WeightInfluences[k]->m_VertexIndex[l];
                    const std::size_t iY = m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_WeightInfluences[k]->m_VertexIndex[l] + 1;
                    const std::size_t iZ = m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_WeightInfluences[k]->m_VertexIndex[l] + 2;

                    Vector3F inputVertex;

                    // get input vertex
                    inputVertex.m_X = (*m_VBCache[i])[iX];
                    inputVertex.m_Y = (*m_VBCache[i])[iY];
                    inputVertex.m_Z = (*m_VBCache[i])[iZ];

                    // apply bone transformation to vertex
                    const Vector3F outputVertex = finalMatrix.Transform(inputVertex);

                    // apply the skin weights and calculate the final output vertex
                    pMesh->m_VB[0]->m_Data[iX] += (outputVertex.m_X * (float)m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_Weights[k]);
                    pMesh->m_VB[0]->m_Data[iY] += (outputVertex.m_Y * (float)m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_Weights[k]);
                    pMesh->m_VB[0]->m_Data[iZ] += (outputVertex.m_Z * (float)m_pModel->m_Deformers[i]->m_SkinWeights[j]->m_Weights[k]);
                }
            }
        }
    }

    return m_pModel;
}
//---------------------------------------------------------------------------
void MHX2Model::SetVertFormatTemplate(const VertexFormat& vertFormatTemplate)
{
    m_VertFormatTemplate = vertFormatTemplate;
}
//---------------------------------------------------------------------------
void MHX2Model::SetVertCullingTemplate(const VertexCulling& vertCullingTemplate)
{
    m_VertCullingTemplate = vertCullingTemplate;
}
//---------------------------------------------------------------------------
void MHX2Model::SetMaterial(const Material& materialTemplate)
{
    m_MaterialTemplate = materialTemplate;
}
//---------------------------------------------------------------------------
void MHX2Model::SetPoseOnly(bool value)
{
    m_PoseOnly = value;
}
//---------------------------------------------------------------------------
void MHX2Model::Set_OnGetVertexColor(VertexBuffer::ITfOnGetVertexColor fOnGetVertexColor)
{
    m_fOnGetVertexColor = fOnGetVertexColor;
}
//---------------------------------------------------------------------------
void MHX2Model::Set_OnLoadTexture(Texture::ITfOnLoadTexture fOnLoadTexture)
{
    m_fOnLoadTexture = fOnLoadTexture;
}
//---------------------------------------------------------------------------
bool MHX2Model::BuildSkeleton(const ISkeletonItem& skeletonItem, Model* pModel)
{
    if (!pModel)
        return false;

    // get the bone count
    const std::size_t boneCount = skeletonItem.m_Bones.size();

    // iterate through the bones and build the skeleton
    for (std::size_t i = 0; i < boneCount; ++i)
    {
        Model::IBone* pParent = nullptr;

        // link the parent bone
        if (!skeletonItem.m_Bones[i]->m_Parent.empty() && pModel->m_pSkeleton)
            pParent = pModel->FindBone(pModel->m_pSkeleton, skeletonItem.m_Bones[i]->m_Parent);

        // is the root bone?
        if (!pParent)
        {
            // create the bone and populate it
            std::unique_ptr<Model::IBone> pSkeleton(new Model::IBone());
            pSkeleton->m_Name   = skeletonItem.m_Bones[i]->m_Name;
            pSkeleton->m_Matrix = skeletonItem.m_Bones[i]->m_Matrix;

            // define this bone as the skeleton root bone
            pModel->m_pSkeleton = pSkeleton.release();
        }
        else
        {
            // create the bone and populate it
            std::unique_ptr<Model::IBone> pBone(new Model::IBone());
            pBone->m_pParent = pParent;
            pBone->m_Name    = skeletonItem.m_Bones[i]->m_Name;
            pBone->m_Matrix  = skeletonItem.m_Bones[i]->m_Matrix;

            // add this bone to the parent children bones
            pBone->m_pParent->m_Children.push_back(pBone.get());
            pBone.release();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool MHX2Model::BuildGeometry(const IModelItem* pModelItem, const IGeometryItem* pGeometryItem, Model* pModel)
{
    if (!pModelItem)
        return false;

    if (!pGeometryItem)
        return false;

    if (!pModel)
        return false;

    std::unique_ptr<Mesh>         pMesh(new Mesh());
    std::unique_ptr<VertexBuffer> pVB(new VertexBuffer());

    // apply the user wished vertex format
    pVB->m_Format = m_VertFormatTemplate;

    // apply the user wished vertex culling
    pVB->m_Culling = m_VertCullingTemplate;

    // apply the user wished material
    pVB->m_Material = m_MaterialTemplate;

    // set the vertex format type
    pVB->m_Format.m_Type = VertexFormat::IEType::IE_VT_Triangles;

    // calculate the stride
    pVB->m_Format.CalculateStride();

    const std::size_t faceCount = pGeometryItem->m_Mesh.m_Faces.size();

    // can load the texture?
    if (m_fOnLoadTexture)
    {
        IMaterialItem*    pMaterial     = nullptr;
        const std::size_t materialCount = pModelItem->m_Materials.size();

        // search the material matching with the mesh
        for (std::size_t i = 0; i < materialCount; ++i)
            if (pModelItem->m_Materials[i]->m_Name == pGeometryItem->m_Material)
            {
                pMaterial = pModelItem->m_Materials[i];
                break;
            }

        // found a material?
        if (pMaterial)
        {
            // load the texture
            pVB->m_Material.m_pTexture = m_fOnLoadTexture(pMaterial->m_DiffuseTexture, pMaterial->m_Transparent);

            // set material transparency
            pVB->m_Material.m_Transparent = pMaterial->m_Transparent;
        }
    }

    std::unique_ptr<Model::IDeformers> pDeformers(new Model::IDeformers());
    IIndexToInflDict                   indexToInfl;
    float                              determinant;

    const std::size_t weightsGroupCount = pGeometryItem->m_Mesh.m_WeightGroups.size();

    // create the mesh weights containers
    for (std::size_t i = 0; i < weightsGroupCount; ++i)
    {
        // populate the skin weights. The weight matrix is the inverse of the global bone matrix, as explained in this document:
        // https://veeenu.github.io/blog/implementing-skeletal-animation/
        std::unique_ptr<Model::ISkinWeights> pSkinWeights(new Model::ISkinWeights());
        pSkinWeights->m_BoneName = pGeometryItem->m_Mesh.m_WeightGroups[i]->m_Key;
        pSkinWeights->m_pBone    = pModel->FindBone(pModel->m_pSkeleton, pGeometryItem->m_Mesh.m_WeightGroups[i]->m_Key);
        pSkinWeights->m_Matrix   = pSkinWeights->m_pBone->m_Matrix.Inverse(determinant);

        // read the vertex indices and the weights from the source file
        for (IWeightTable::iterator it = pGeometryItem->m_Mesh.m_WeightGroups[i]->m_Table.begin();
                it != pGeometryItem->m_Mesh.m_WeightGroups[i]->m_Table.end(); ++it)
        {
            // create a new weight influence, and set the vertex index it references
            std::unique_ptr<Model::IWeightInfluence> pWeightInfluence(new Model::IWeightInfluence());
            pWeightInfluence->m_Index = it->first;
            pSkinWeights->m_WeightInfluences.push_back(pWeightInfluence.get());

            // search for existing index in the dictionary
            IIndexToInflDict::iterator itInfl = indexToInfl.find(pWeightInfluence->m_Index);

            // found it?
            if (itInfl == indexToInfl.end())
            {
                // create a new weight influence reference
                Model::IWeightInfluences weightInfluences;
                weightInfluences.push_back(pWeightInfluence.get());

                // add the index to the dictionary
                indexToInfl[pWeightInfluence->m_Index] = weightInfluences;
            }
            else
                // add the weight influence to the index
                itInfl->second.push_back(pWeightInfluence.get());

            pWeightInfluence.release();

            // add the weight matching with the vertex
            pSkinWeights->m_Weights.push_back(it->second);
        }

        pDeformers->m_SkinWeights.push_back(pSkinWeights.get());
        pSkinWeights.release();
    }

    // iterate through the faces to build
    for (std::size_t i = 0; i < faceCount; ++i)
    {
        const IFaceItem*  pFace      = pGeometryItem->m_Mesh.m_Faces[i];
        const IFaceItem*  pUVFace    = pGeometryItem->m_Mesh.m_UVFaces[i];
        const std::size_t valueCount = pFace->m_Values.size();

        // iterate through the face vertices
        for (std::size_t j = 0; j < valueCount - 2; ++j)
            for (unsigned char k = 0; k < 3; ++k)
            {
                const std::size_t index       = !k ? 0 : j + k;
                const std::size_t vertexIndex = pVB->m_Data.size();
                const std::size_t faceIndex   = pFace->m_Values[index];
                const std::size_t uvIndex     = pUVFace->m_Values[index];
                const Vector3F    normal;

                AddWeightInfluence(&indexToInfl, faceIndex, pVB.get());

                // add the vertex to the buffer
                pVB->Add(pGeometryItem->m_Mesh.m_Vertices[faceIndex],
                        &normal,
                        &pGeometryItem->m_Mesh.m_UVCoords[uvIndex]->m_Value,
                         0,
                         m_fOnGetVertexColor);
            }
    }

    // cache the vertex buffer
    std::unique_ptr<VertexBuffer::IData> pVBData(new VertexBuffer::IData());
    *pVBData = pVB->m_Data;
    m_VBCache.push_back(pVBData.get());
    pVBData.release();

    // add the vertex buffer to the mesh
    pMesh->m_VB.push_back(pVB.get());
    pVB.release();

    // add the mesh to the model
    pModel->m_Mesh.push_back(pMesh.get());
    pMesh.release();

    // add the deformers to the model
    pModel->m_Deformers.push_back(pDeformers.get());
    pDeformers.release();

    return true;
}
//---------------------------------------------------------------------------
void MHX2Model::AddWeightInfluence(const IIndexToInflDict* pIndexToInfl, std::size_t indice, VertexBuffer* pModelVB) const
{
    if (!pIndexToInfl)
        return;

    IIndexToInflDict::const_iterator it = pIndexToInfl->find(indice);

    if (it != pIndexToInfl->end())
    {
        const std::size_t weightInflCount = it->second.size();

        for (std::size_t j = 0; j < weightInflCount; ++j)
            it->second[j]->m_VertexIndex.push_back(pModelVB->m_Data.size());
    }
}
//---------------------------------------------------------------------------
