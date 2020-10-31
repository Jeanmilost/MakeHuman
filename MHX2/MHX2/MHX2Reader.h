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

#pragma once

// std
#include <vector>
#include <string>

// libraries
#include "json.h"

// classes
#include "Color.h"
#include "Vector3.h"
#include "Matrix4x4.h"

/**
* MakeHuman .mhx2 file reader
*@author Jean-Milost Reymond
*/
class MHX2Reader
{
    public:
        typedef std::vector<bool>        IBoolValues;
        typedef std::vector<std::size_t> IIntValues;
        typedef std::vector<float>       IFloatValues;
        typedef std::vector<std::string> IStringValues;
        typedef std::vector<Vector3F*>   IVertices;

        /**
        * Item
        */
        struct IItem
        {
            IItem();
            virtual ~IItem();

            /**
            * Parses the vector data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] vector - vector to populate
            *@param[in, out] index - vector index
            *@param[out] error - last error
            *@return true on success, otherwise false
            */
            virtual bool ParseVector(json_value* pJson, Vector3F& vector, std::size_t& index, std::string& error) const;

            /**
            * Parses the matrix data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] matrix - matrix to populate
            *@param[in, out] x - matrix x index
            *@param[in, out] y - matrix y index
            *@param[out] error - last error
            *@return true on success, otherwise false
            */
            virtual bool ParseMatrix(json_value* pJson, Matrix4x4F& matrix, std::size_t& x, std::size_t& y, std::string& error) const;
        };

        /**
        * Bone
        */
        struct IBone : public IItem
        {
            std::string m_Name;
            std::string m_Parent;
            Vector3F    m_Head;
            Vector3F    m_Tail;
            float       m_Roll;
            Matrix4x4F  m_Matrix;

            IBone();
            virtual ~IBone();

            /**
            * Parses the bone data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] error - last error
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, std::string& error);
        };

        typedef std::vector<IBone*> IBones;

        /**
        * Skeleton
        */
        struct ISkeleton : public IItem
        {
            std::string m_Name;
            Vector3F    m_Offset;
            float       m_Scale;
            IBones      m_Bones;

            ISkeleton();
            virtual ~ISkeleton();

            /**
            * Parses the skeleton data from a json object
            *@param pJson - json object containing the data to parse
            *@param[out] error - last error
            *@return true on success, otherwise false
            */
            virtual bool Parse(json_value* pJson, std::string& error);
        };

        /**
        * Material
        */
        struct IMaterial : public IItem
        {
            std::string m_Name;
            std::string m_DiffuseTexture;
            ColorF      m_Ambient;
            ColorF      m_Diffuse;
            ColorF      m_Emissive;
            float       m_DiffuseMapIntensity;
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
            bool        m_SssEnabled;
        };

        typedef std::vector<IMaterial*> IMaterials;

        /**
        * License
        */
        struct ILicense : public IItem
        {
            std::string m_Author;
            std::string m_License;
            std::string m_Homepage;
        };

        /**
        * Face
        */
        struct IFace : public IItem
        {
            IIntValues m_Values;
        };

        typedef std::vector<IFace*> IFaces;

        /**
        * UV coord
        */
        struct IUVCoord : public IItem
        {
            float m_X;
            float m_Y;
        };

        typedef std::vector<IUVCoord*> IUVCoords;

        /**
        * Weight
        */
        struct IWeight : public IItem
        {
            std::string  m_Name;
            IFloatValues m_Values;
        };

        typedef std::vector<IWeight*> IWeights;

        /**
        * Fit
        */
        struct IFit : public IItem
        {
            IVertices m_Values;
        };

        typedef std::vector<IFit*> IFitting;

        /**
        * Mesh
        */
        struct IMesh : public IItem
        {
            IVertices   m_Vertices; // NEED DEL
            IFaces      m_Faces;    // NEED DEL
            IUVCoords   m_UVCoords; // NEED DEL
            IFaces      m_UVFaces;  // NEED DEL
            IWeights    m_Weights;  // NEED DEL
        };

        /**
        * Proxy
        */
        struct IProxy : public IItem
        {
            std::string   m_Name;
            std::string   m_Type;
            std::string   m_Uuid;
            std::string   m_Basemesh;
            IStringValues m_Tags;
            IBoolValues   m_DeleteVerts;
            IFitting      m_Fitting; // NEED DEL
            void*         m_pVertexBoneWeights; // NEED DEL
        };

        /**
        * Geometry
        */
        struct IGeometry : public IItem
        {
            std::string m_Name;
            std::string m_Uuid;
            std::string m_Material;
            ILicense    m_License;
            IMesh       m_Mesh;
            IMesh       m_SeedMesh;
            IMesh       m_ProxySeedMesh;
            IProxy      m_Proxy;
            Vector3F    m_Offset;
            float       m_Scale;
            bool        m_IsHuman;
            bool        m_IsSubdivided;
        };

        typedef std::vector<IGeometry*> IGeometries;

        /**
        * Model
        */
        struct IModel : public IItem
        {
            ISkeleton   m_Skeleton;
            IMaterials  m_Materials; // NEED DEL
            IGeometries m_Geometries; // NEED DEL
        };

        MHX2Reader();
        virtual ~MHX2Reader();

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

    private:
        /**
        * Item type
        */
        enum IEType
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

        IModel* m_pModel;

        /**
        * Parse the data
        *@param pJson - json value
        *@param pModel - mhx2 model
        *@param type - data type
        *@return true on success, otherwise false
        */
        bool Parse(json_value* pJson, IModel* pModel, IEType type);

        IEType NameToType(const std::string& name) const;
};
