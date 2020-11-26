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
 // MHX2Model::IBone
 //---------------------------------------------------------------------------
MHX2Model::IBone::IBone() :
    m_pParent(nullptr),
    m_Roll(0.0f)
{}
//---------------------------------------------------------------------------
MHX2Model::IBone::~IBone()
{
    const std::size_t count = m_Children.size();

    for (std::size_t i = 0; i < count; ++i)
        delete m_Children[i];
}
//---------------------------------------------------------------------------
// MHX2Model::ISkeleton
//---------------------------------------------------------------------------
MHX2Model::ISkeleton::ISkeleton() :
    m_Scale(1.0f),
    m_pRoot(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Model::ISkeleton::~ISkeleton()
{
    if (m_pRoot)
        delete m_pRoot;
}
//---------------------------------------------------------------------------
// MHX2Model::IWeight
//---------------------------------------------------------------------------
MHX2Model::IWeight::IWeight() :
    m_Index(0),
    m_VertexIndex(0),
    m_Value(1.0f)
{}
//---------------------------------------------------------------------------
MHX2Model::IWeight::~IWeight()
{}
//---------------------------------------------------------------------------
// MHX2Model::IBonedWeights
//---------------------------------------------------------------------------
MHX2Model::IBonedWeights::IBonedWeights() :
    m_pBone(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Model::IBonedWeights::~IBonedWeights()
{
    const std::size_t count = m_Weights.size();

    for (std::size_t i = 0; i < count; ++i)
        delete m_Weights[i];
}
//---------------------------------------------------------------------------
// MHX2Model::IModel
//---------------------------------------------------------------------------
MHX2Model::IModel::IModel() :
    m_pSkeleton(nullptr)
{}
//---------------------------------------------------------------------------
MHX2Model::IModel::~IModel()
{
    const std::size_t count = m_Meshes.size();

    for (std::size_t i = 0; i < count; ++i)
        delete m_Meshes[i];
}
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
    m_PoseOnly(true),
    m_fOnGetVertexColor(nullptr),
    m_fOnLoadTexture(nullptr)
{
    // configure the default vertex format
    m_VertFormatTemplate.m_Format = (VertexFormat::IEFormat)(VertexFormat::IE_VF_Colors | VertexFormat::IE_VF_TexCoords);

    // configure the default vertex culling
    m_VertCullingTemplate.m_Type = VertexCulling::IE_CT_Back;
    m_VertCullingTemplate.m_Face = VertexCulling::IE_CF_CCW;

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
    std::unique_ptr<IModel>    pModel(new IModel());
    std::unique_ptr<ISkeleton> pSkeleton(new ISkeleton());

    // build the model skeleton
    if (!BuildSkeleton(pModelItem->m_Skeleton, pSkeleton.get()))
        return false;

    pModel->m_pSkeleton = pSkeleton.release();

    const std::size_t geometryCount = pModelItem->m_Geometries.size();

    // build the model geometries
    for (std::size_t i = 0; i < geometryCount; ++i)
        if (!BuildGeometry(pModelItem.get(), pModelItem->m_Geometries[i], pModel.get()))
            return false;

    m_pModel = pModel.release();
    return true;
}
//---------------------------------------------------------------------------
MHX2Model::IModel* MHX2Model::GetModel() const
{
    return m_pModel;
}
//---------------------------------------------------------------------------
void MHX2Model::GetBoneMatrix(const IBone* pBone, const Matrix4x4F& initialMatrix, Matrix4x4F& matrix) const
{
    // no bone?
    if (!pBone)
        return;

    // set the output matrix as identity
    matrix = Matrix4x4F::Identity();

    // iterate through bones
    while (pBone)
    {
        // get the previously stacked matrix as base to calculate the new one
        const Matrix4x4F localMatrix = matrix;

        // stack the previously calculated matrix with the current bone one
        matrix = localMatrix.Multiply(pBone->m_Matrix);

        // go to parent bone
        pBone = pBone->m_pParent;
    }

    // initial matrix provided?
    if (!initialMatrix.IsIdentity())
    {
        // get the previously stacked matrix as base to calculate the new one
        const Matrix4x4F localMatrix = matrix;

        // stack the previously calculated matrix with the initial one
        matrix = localMatrix.Multiply(initialMatrix);
    }
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
void MHX2Model::Set_OnGetVertexColor(ITfOnGetVertexColor fOnGetVertexColor)
{
    m_fOnGetVertexColor = fOnGetVertexColor;
}
//---------------------------------------------------------------------------
void MHX2Model::Set_OnLoadTexture(ITfOnLoadTexture fOnLoadTexture)
{
    m_fOnLoadTexture = fOnLoadTexture;
}
//---------------------------------------------------------------------------
bool MHX2Model::BuildSkeleton(const ISkeletonItem& skeletonItem, ISkeleton* pSkeleton)
{
    if (!pSkeleton)
        return false;

    // populate the skeleton object
    pSkeleton->m_Name   = skeletonItem.m_Name;
    pSkeleton->m_Scale  = skeletonItem.m_Scale;
    pSkeleton->m_Offset = skeletonItem.m_Offset;

    // get the bone count
    const std::size_t boneCount = skeletonItem.m_Bones.size();

    // iterate through the bones and build the skeleton
    for (std::size_t i = 0; i < boneCount; ++i)
    {
        // create the bone and populate it
        std::unique_ptr<IBone> pBone(new IBone());
        pBone->m_Name   = skeletonItem.m_Bones[i]->m_Name;
        pBone->m_Head   = skeletonItem.m_Bones[i]->m_Head;
        pBone->m_Tail   = skeletonItem.m_Bones[i]->m_Tail;
        pBone->m_Roll   = skeletonItem.m_Bones[i]->m_Roll;
        pBone->m_Matrix = skeletonItem.m_Bones[i]->m_Matrix;

        // link the parent bone
        if (!skeletonItem.m_Bones[i]->m_Parent.empty())
            pBone->m_pParent = GetBone(skeletonItem.m_Bones[i]->m_Parent, pSkeleton->m_pRoot);

        // is the root bone?
        if (!pBone->m_pParent)
        {
            // only one root bone may exist, if another was previously assigned it's an error
            if (!pSkeleton->m_pRoot)
                pSkeleton->m_pRoot = pBone.release();
            else
                return false;
        }
        else
        {
            // add this bone to the parent children bones
            pBone->m_pParent->m_Children.push_back(pBone.get());
            pBone.release();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
MHX2Model::IBone* MHX2Model::GetBone(const std::string& name, IBone* pBone) const
{
    // no parent bone?
    if (!pBone)
        return nullptr;

    // found the bone to get?
    if (pBone->m_Name == name)
        return pBone;

    // get children count
    const std::size_t count = pBone->m_Children.size();

    // Search for the bone in children
    for (std::size_t i = 0; i < count; ++i)
    {
        IBone* pChild = GetBone(name, pBone->m_Children[i]);

        if (pChild)
            return pChild;
    }

    return nullptr;
}
//---------------------------------------------------------------------------
bool MHX2Model::BuildGeometry(const IModelItem* pModelItem, const IGeometryItem* pGeometryItem, IModel* pModel)
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
    pVB->m_Format.m_Type = VertexFormat::IE_VT_Triangles;

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

    const std::size_t weightsGroupCount = pGeometryItem->m_Mesh.m_WeightGroups.size();

    // create the mesh weights containers
    if (!m_PoseOnly)
        for (std::size_t i = 0; i < weightsGroupCount; ++i)
        {
            std::unique_ptr<IBonedWeights> pBonedWeights(new IBonedWeights());
            pBonedWeights->m_pBone = GetBone(pGeometryItem->m_Mesh.m_WeightGroups[i]->m_Key, pModel->m_pSkeleton->m_pRoot);
            pModel->m_Weights.push_back(pBonedWeights.get());
            pBonedWeights.release();
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

                // add the vertex to the buffer
                VertexBufferAdd(pGeometryItem->m_Mesh.m_Vertices[pFace->m_Values[index]],
                               &Vector3F(),
                               &pGeometryItem->m_Mesh.m_UVCoords[pUVFace->m_Values[index]]->m_Value,
                                0,
                                m_fOnGetVertexColor,
                                pVB.get());

                // do build the weights
                if (m_PoseOnly)
                    continue;

                // iterate through the weight groups
                for (std::size_t l = 0; l < weightsGroupCount; ++l)
                {
                    const std::size_t weightCount = pGeometryItem->m_Mesh.m_WeightGroups[l]->m_Weights.size();

                    // search if the vertex is linked to a weight
                    for (std::size_t m = 0; m < weightCount; ++m)
                        if (pGeometryItem->m_Mesh.m_WeightGroups[l]->m_Weights[m]->m_Index == pFace->m_Values[index])
                        {
                            // add the weight to the mesh
                            std::unique_ptr<IWeight> pWeight(new IWeight());
                            pWeight->m_Index       = (vertexIndex / pVB->m_Format.m_Stride);
                            pWeight->m_VertexIndex =  vertexIndex;
                            pWeight->m_Value       =  pGeometryItem->m_Mesh.m_WeightGroups[l]->m_Weights[m]->m_Value;
                            pModel->m_Weights[l]->m_Weights.push_back(pWeight.get());
                            pWeight.release();
                            break;
                        }
                }
            }
    }

    // add the vertex buffer to the mesh
    pMesh->m_VB.push_back(pVB.get());
    pVB.release();

    // add the mesh to the model
    pModel->m_Meshes.push_back(pMesh.get());
    pMesh.release();

    return true;
}
//---------------------------------------------------------------------------
bool MHX2Model::VertexBufferAdd(const Vector3F*           pVertex,
                                const Vector3F*           pNormal,
                                const Vector2F*           pUV,
                                      std::size_t         groupIndex,
                                const ITfOnGetVertexColor fOnGetVertexColor,
                                      VertexBuffer*       pVB) const
{
    // no vertex buffer to add to?
    if (!pVB)
        return false;

    // the stride should be already calculated
    if (!pVB->m_Format.m_Stride)
        return false;

    // keep the current offset
    std::size_t offset = pVB->m_Data.size();

    // allocate space for new vertex
    pVB->m_Data.resize(pVB->m_Data.size() + pVB->m_Format.m_Stride);

    // source vertex exists?
    if (!pVertex)
    {
        // cannot add a nonexistent vertex, fill with empty data in this case
        pVB->m_Data[offset]     = 0.0f;
        pVB->m_Data[offset + 1] = 0.0f;
        pVB->m_Data[offset + 2] = 0.0f;
    }
    else
    {
        // copy vertex from source
        pVB->m_Data[offset]     = pVertex->m_X;
        pVB->m_Data[offset + 1] = pVertex->m_Y;
        pVB->m_Data[offset + 2] = pVertex->m_Z;
    }

    offset += 3;

    // vertex has a normal?
    if (pVB->m_Format.m_Format & VertexFormat::IE_VF_Normals)
    {
        // source normal exists?
        if (!pNormal)
        {
            // cannot add a nonexistent normal, fill with empty data in this case
            pVB->m_Data[offset]     = 0.0f;
            pVB->m_Data[offset + 1] = 0.0f;
            pVB->m_Data[offset + 2] = 0.0f;
        }
        else
        {
            // copy normal from source
            pVB->m_Data[offset]     = pNormal->m_X;
            pVB->m_Data[offset + 1] = pNormal->m_Y;
            pVB->m_Data[offset + 2] = pNormal->m_Z;
        }

        offset += 3;
    }

    // vertex has UV texture coordinates?
    if (pVB->m_Format.m_Format & VertexFormat::IE_VF_TexCoords)
    {
        // source texture coordinates exists?
        if (!pUV)
        {
            // cannot add nonexistent texture coordinates, fill with empty data in this case
            pVB->m_Data[offset] = 0.0f;
            pVB->m_Data[offset + 1] = 0.0f;
        }
        else
        {
            // copy texture coordinates from source
            pVB->m_Data[offset] = pUV->m_X;
            pVB->m_Data[offset + 1] = pUV->m_Y;
        }

        offset += 2;
    }

    // vertex has color?
    if (pVB->m_Format.m_Format & VertexFormat::IE_VF_Colors)
    {
        ColorF color;

        // get the vertex color
        if (fOnGetVertexColor)
            color = fOnGetVertexColor(pVB, pNormal, groupIndex);
        else
            color = pVB->m_Material.m_Color;

        // set color data
        pVB->m_Data[offset]     = color.m_R;
        pVB->m_Data[offset + 1] = color.m_G;
        pVB->m_Data[offset + 2] = color.m_B;
        pVB->m_Data[offset + 3] = color.m_A;
    }

    return true;
}
//---------------------------------------------------------------------------
