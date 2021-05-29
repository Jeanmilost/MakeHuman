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

#pragma once

// std
#include <vector>
#include <string>
#include <sstream>

// libraries
#include "json.h"

// classes
#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Vertex.h"
#include "Model.h"

/**
* MakeHuman .mhx2 file reader
*@author Jean-Milost Reymond
*/
class MHX2Model
{
    public:
        MHX2Model();
        virtual ~MHX2Model();

        /**
        * Opens a .mhx2 file
        *@param fileName - mhx2 file to open
        *@return true on success, otherwise false
        */
        virtual bool Open(const std::string& fileName);

        /**
        * Reads a mhx2 data
        *@param data - mhx2 data to open
        *@return true on success, otherwise false
        */
        virtual bool Read(const std::string& data);

        /**
        * Gets a ready-to-draw copy of the model
        *@param animSetIndex - animation set index
        *@param elapsedTime - elapsed time in milliseconds
        *@return a ready-to-draw copy of the model, nullptr on error
        */
        virtual Model* GetModel(int animSetIndex, double elapsedTime) const;

        /**
        * Changes the vertex format template
        *@param vertFormatTemplate - new vertex format template
        */
        virtual void SetVertFormatTemplate(const VertexFormat& vertFormatTemplate);

        /**
        * Changes the vertex culling template
        *@param vertCullingTemplate - new vertex culling template
        */
        virtual void SetVertCullingTemplate(const VertexCulling& vertCullingTemplate);

        /**
        * Changes the material template
        *@param materialTemplate - new material template
        */
        virtual void SetMaterial(const Material& materialTemplate);

        /**
        * Sets if only the pose should be rendered, without animation
        *@param value - if true, only the pose will be rendered
        *@note This function should be called before open the model
        */
        virtual void SetPoseOnly(bool value);

        /**
        * Sets the OnGetVertexColor callback
        *@param fOnGetVertexColor - callback function handle
        */
        void Set_OnGetVertexColor(VertexBuffer::ITfOnGetVertexColor fOnGetVertexColor);

        /**
        * Sets the OnLoadTexture callback
        *@param fOnLoadTexture - callback function handle
        */
        void Set_OnLoadTexture(Texture::ITfOnLoadTexture fOnLoadTexture);

    private:
        /**
        * Item type
        */
        enum class IEType
        {
            IE_T_Unknown = 0,
            IE_T_Model,
            IE_T_License,
            IE_T_Skeleton,
            IE_T_Bones,
            IE_T_Matrix,
            IE_T_Materials,
            IE_T_Geometries,
            IE_T_Mesh,
            IE_T_SeedMesh,
            IE_T_ProxySeedMesh,
            IE_T_Proxy,
            IE_T_Weights,
        };

        typedef std::vector<bool>        IBoolValues;
        typedef std::vector<std::size_t> IIntValues;
        typedef std::vector<float>       IFloatValues;
        typedef std::vector<std::string> IStringValues;
        typedef std::vector<Vector3F*>   IVerticeItems;

        /**
        * Logger
        */
        class ILogger
        {
            public:
                ILogger();
                virtual ~ILogger();

                /**
                * Clears the logger content
                */
                virtual void Clear();

                /**
                * Logs a simple message
                *@param pJson - the json object containing the data
                *@param message - message to log
                */
                virtual void Log(const std::string& message);

                /**
                * Logs a message with a value
                *@param pJson - the json object containing the data
                *@param message - message to log
                */
                template <class T>
                void Log(const std::string& message, T value);

                /**
                * Logs a simple json message
                *@param pJson - the json object containing the data
                *@param message - message to log
                */
                virtual void Log(json_value* pJson, const std::string& message);

                /**
                * Logs a json message with a value
                *@param pJson - the json object containing the data
                *@param message - message to log
                */
                template <class T>
                void Log(json_value* pJson, const std::string& message, T value);

            private:
                typedef std::vector<std::string> ILines;

                ILines m_Lines;
        };

        /**
        * Item
        */
        struct IItem
        {
            IItem();
            virtual ~IItem();

            /**
            * Parses the color data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] color - color to populate
            *@param[in, out] index - vector index
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool ParseColor(json_value* pJson, ColorF& color, std::size_t& index, ILogger& logger) const;

            /**
            * Parses the vector data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] vector - vector to populate
            *@param[in, out] index - vector index
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool ParseVector(json_value* pJson, Vector3F& vector, std::size_t& index, ILogger& logger) const;

            /**
            * Parses the matrix data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] matrix - matrix to populate
            *@param[in, out] x - matrix x index
            *@param[in, out] y - matrix y index
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool ParseMatrix(json_value* pJson, Matrix4x4F& matrix, std::size_t& x, std::size_t& y, ILogger& logger) const;
        };

        /**
        * Bone
        */
        struct IBoneItem : public IItem
        {
            std::string m_Name;
            std::string m_Parent;
            Vector3F    m_Head;
            Vector3F    m_Tail;
            float       m_Roll;
            Matrix4x4F  m_Matrix;

            IBoneItem();
            virtual ~IBoneItem();

            /**
            * Parses the bone data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IBoneItem*> IBoneItems;

        /**
        * Skeleton
        */
        struct ISkeletonItem : public IItem
        {
            std::string m_Name;
            Vector3F    m_Offset;
            float       m_Scale;
            IBoneItems  m_Bones;

            ISkeletonItem();
            virtual ~ISkeletonItem();

            /**
            * Parses the skeleton data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        /**
        * Material
        */
        struct IMaterialItem : public IItem
        {
            std::string m_Name;
            std::string m_DiffuseTexture;
            std::string m_NormalMapTexture;
            ColorF      m_Ambient;
            ColorF      m_Diffuse;
            ColorF      m_Specular;
            ColorF      m_Emissive;
            float       m_DiffuseMapIntensity;
            float       m_SpecularMapIntensity;
            float       m_TransparencyMapIntensity;
            float       m_Shininess;
            float       m_Opacity;
            float       m_Translucency;
            float       m_SssRScale;
            float       m_SssGScale;
            float       m_SssBScale;
            bool        m_Shadeless;
            bool        m_Wireframe;
            bool        m_Transparent;
            bool        m_AlphaToCoverage;
            bool        m_BackfaceCull;
            bool        m_Depthless;
            bool        m_CastShadows;
            bool        m_ReceiveShadows;
            bool        m_SssEnabled;

            IMaterialItem();
            virtual ~IMaterialItem();

            /**
            * Parses the material data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IMaterialItem*> IMaterialItems;

        /**
        * License
        */
        struct ILicenseItem : public IItem
        {
            std::string m_Author;
            std::string m_License;
            std::string m_Homepage;

            ILicenseItem();
            virtual ~ILicenseItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        /**
        * Face
        */
        struct IFaceItem : public IItem
        {
            IIntValues m_Values;

            IFaceItem();
            virtual ~IFaceItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IFaceItem*> IFaceItems;

        /**
        * UV coord
        */
        struct IUVCoordItem : public IItem
        {
            Vector2F    m_Value;
            std::size_t m_InternalIndex;

            IUVCoordItem();
            virtual ~IUVCoordItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IUVCoordItem*> IUVCoordItems;

        /**
        * Weight
        */
        struct IWeightItem : public IItem
        {
            std::size_t m_Index;
            std::size_t m_InternalIndex;
            float       m_Value;

            IWeightItem();
            virtual ~IWeightItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IWeightItem*>    IWeightItems;
        typedef std::map<std::size_t, float> IWeightTable;

        /**
        * Weight group
        */
        struct IWeightGroupItem : public IItem
        {
            std::string  m_Key;
            IWeightItems m_Weights;
            IWeightTable m_Table;

            IWeightGroupItem();
            virtual ~IWeightGroupItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IWeightGroupItem*> IWeightGroupItems;

        /**
        * Fit
        */
        struct IFitItem : public IItem
        {
            IVerticeItems m_Values;

            IFitItem();
            virtual ~IFitItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IFitItem*> IFitItems;

        /**
        * Mesh
        */
        struct IMeshItem : public IItem
        {
            IVerticeItems     m_Vertices;
            IFaceItems        m_Faces;
            IUVCoordItems     m_UVCoords;
            IFaceItems        m_UVFaces;
            IWeightGroupItems m_WeightGroups;

            IMeshItem();
            virtual ~IMeshItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        /**
        * Proxy
        */
        struct IProxyItem : public IItem
        {
            ILicenseItem  m_License;
            std::string   m_Name;
            std::string   m_Type;
            std::string   m_Uuid;
            std::string   m_Basemesh;
            IStringValues m_Tags;
            IBoolValues   m_DeleteVerts;
            IFitItems     m_Fitting;
            void*         m_pVertexBoneWeights;

            IProxyItem();
            virtual ~IProxyItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        /**
        * Geometry
        */
        struct IGeometryItem : public IItem
        {
            std::string  m_Name;
            std::string  m_Uuid;
            std::string  m_Material;
            ILicenseItem m_License;
            IMeshItem    m_Mesh;
            IMeshItem    m_SeedMesh;
            IMeshItem    m_ProxySeedMesh;
            IProxyItem   m_Proxy;
            Vector3F     m_Offset;
            float        m_Scale;
            bool         m_IsHuman;
            bool         m_IsSubdivided;

            IGeometryItem();
            virtual ~IGeometryItem();

            /**
            * Parses the license data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        typedef std::vector<IGeometryItem*> IGeometryItems;

        /**
        * Model
        */
        struct IModelItem : public IItem
        {
            std::string    m_Version;
            ISkeletonItem  m_Skeleton;
            IMaterialItems m_Materials;
            IGeometryItems m_Geometries;

            IModelItem();
            virtual ~IModelItem();

            /**
            * Parses the model data from a json object
            *@param pJson - json object containing the data to parse
            *@param[in, out] logger - logger
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, ILogger& logger);
        };

        /**
        * Bone details
        */
        struct IBoneDetails
        {
            Vector3F m_Head;
            Vector3F m_Tail;
            float    m_Roll;

            IBoneDetails();
            virtual ~IBoneDetails();
        };

        /**
        * Root bone details
        */
        struct IRootBoneDetails: public IBoneDetails
        {
            std::string m_Name;
            Vector3F    m_Offset;
            float       m_Scale;

            IRootBoneDetails();
            virtual ~IRootBoneDetails();
        };

        typedef std::vector<IBoneDetails*>                  IBonesDetails;
        typedef std::map<std::string, Model::ISkinWeights*> ISkinWeightsDict;

        /**
        * Animated bone cache dictionary
        */
        typedef std::map<const Model::IBone*, Matrix4x4F> IAnimBoneCacheDict;

        /**
        * Source vertex buffer cache
        */
        typedef std::vector<VertexBuffer::IData*> IVBCache;

        Model*                            m_pModel;
        IBonesDetails                     m_BoneDetails;
        VertexFormat                      m_VertFormatTemplate;
        VertexCulling                     m_VertCullingTemplate;
        Material                          m_MaterialTemplate;
        IAnimBoneCacheDict                m_AnimBoneCacheDict;
        IVBCache                          m_VBCache;
        ILogger                           m_Logger;
        bool                              m_PoseOnly;
        VertexBuffer::ITfOnGetVertexColor m_fOnGetVertexColor;
        Texture::ITfOnLoadTexture         m_fOnLoadTexture;

        /**
        * Builds the skeleton
        *@param skeletonItem - source skeleton item read from the file
        *@param pModel - model which will own the skeleton
        *@return true on success, otherwise false
        */
        bool BuildSkeleton(const ISkeletonItem& skeletonItem, Model* pModel);

        /**
        * Builds the geometry
        *@param pModelItem - source model item read from the file
        *@param pGeometryItem - source geometry item read from the file
        *@param pModel - target model for which the geometry should be built
        *@return true on success, otherwise false
        */
        bool BuildGeometry(const IModelItem* pModelItem, const IGeometryItem* pGeometryItem, Model* pModel);
};

//---------------------------------------------------------------------------
// MHX2Reader
//---------------------------------------------------------------------------
template <class T>
void MHX2Model::ILogger::Log(const std::string& message, T value)
{
    // log the message
    std::ostringstream sstr;
    sstr << message << " - " << value;

    m_Lines.push_back(sstr.str());
}
//---------------------------------------------------------------------------
template <class T>
void MHX2Model::ILogger::Log(json_value* pJson, const std::string& message, T value)
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
                sstr << message << " - " << value << " - json - key - " << key << " - type - " << pJson->type;
            else
                sstr << message << " - " << value << " - json - type - " << pJson->type;
        }
    }
    else
        // log the message
        sstr << message << " - " << value;

    m_Lines.push_back(sstr.str());
}
//---------------------------------------------------------------------------
