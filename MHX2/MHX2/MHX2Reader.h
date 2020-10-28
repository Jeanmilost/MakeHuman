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

// retrograde engine
#include "Vector3.h"
#include "Matrix4x4.h"

/**
* MakeHuman .mhx2 file reader
*@author Jean-Milost Reymond
*/
class MHX2Reader
{
    public:
        /**
        * Generic item
        */
        class IItem
        {
            public:
                IItem();

                /**
                * Constructor
                *@param name - name
                *@param pParent - parent item
                *@param isArray - if true, the item contains an array
                */
                IItem(const std::string& name, IItem* pParent = nullptr, bool isArray = false);

                virtual ~IItem();

                /**
                * Gets the key
                *@return the key
                */
                virtual std::string GetKey() const;

                /**
                * Sets the key
                *@param key - the new key to set
                */
                virtual void SetKey(const std::string& name);

                /**
                * Gets the parent
                *@return the parent
                */
                virtual IItem* GetParent() const;

                /**
                * Sets the parent
                *@param pParent - the new parent to set
                */
                virtual void SetParent(IItem* pParent);

                /**
                * Gets if the item is an array
                *@return true if the item is an array, otherwise false
                */
                virtual bool GetIsArray() const;

                /**
                * Sets if the item is an array
                *@param value - if true, the item is an array
                */
                virtual void SetIsArray(bool value);

                /**
                * Adds a child
                *@param pChild - child to add
                *@note Don't try to delete the added item from outside, it will be deleted internally
                */
                virtual void AddChild(IItem* pChild);

                /**
                * Deletes a child
                *@param pChild - child to delete
                */
                virtual void DeleteChild(IItem* pChild);

                /**
                * Deletes a child at index
                *@param index - child index to delete
                */
                virtual void DeleteChildAt(std::size_t index);

                /**
                * Gets a child at index
                *@param index - child index to get
                *@return the child at index, nullptr if not found or on error
                */
                virtual IItem* GetChildAt(std::size_t index) const;

                /**
                * Gets the child count
                *@return the child count
                */
                virtual std::size_t GetChildCount() const;

            protected:
                typedef std::vector<IItem*> IItems;

                std::string m_Key;
                IItem*      m_pParent;
                IItems      m_Items;
                bool        m_IsArray;
        };

        /**
        * Single value
        */
        template <class T>
        class IValue : public IItem
        {
            public:
                IValue();

                /**
                * Constructor
                *@param value - value
                */
                IValue(T value);

                /**
                * Constructor
                *@param key - key
                *@param value - value
                */
                IValue(const std::string& key, T value);

                virtual ~IValue();

                /**
                * Gets the value
                *@return the value
                */
                virtual T GetValue() const;

                /**
                * Sets the value
                *@param value - the value to set
                */
                virtual void SetValue(T value);

            private:
                T m_Value;
        };

        /**
        * 3D vector
        */
        class IVector3 : public IItem
        {
            public:
                IVector3();

                /**
                * Constructor
                *@param key - key
                *@param value - value
                */
                IVector3(const std::string& key, const Vector3F& value);

                virtual ~IVector3();

                /**
                * Gets the value
                *@return the value
                */
                virtual Vector3F GetValue() const;

                /**
                * Sets the value
                *@param value - the value to set
                */
                virtual void SetValue(const Vector3F& value);

            private:
                Vector3F m_Value;
        };

        /**
        * 4x4 matrix
        */
        class IMatrix4x4 : public IItem
        {
            public:
                IMatrix4x4();

                /**
                * Constructor
                *@param key - key
                *@param value - value
                */
                IMatrix4x4(const std::string& name, const Matrix4x4F& value);

                virtual ~IMatrix4x4();

                /**
                * Gets the value
                *@return the value
                */
                virtual Matrix4x4F GetValue() const;

                /**
                * Sets the value
                *@param value - the value to set
                */
                virtual void SetValue(const Matrix4x4F& value);

            private:
                Matrix4x4F m_Value;
        };

        MHX2Reader();
        virtual ~MHX2Reader();

        /**
        * Opens a .mhx2 file
        *@param fileName - .mhx2 file to open
        *@return true on success, otherwise false
        */
        virtual bool Open(const std::string& fileName);

        /**
        * Reads a .mhx2 data
        *@param data - .mhx2 data to open
        *@return true on success, otherwise false
        */
        virtual bool Read(const std::string& data);

    private:
        IItem* m_pRoot;

        bool ReadLine(const std::string& data, const std::size_t startIndex, const std::size_t endIndex, IItem*& pParent);

        bool ReadKeyValue(const std::string& data,
                          const std::size_t  keyStart,
                          const std::size_t  keyEnd,
                          const std::size_t  valueStart,
                          const std::size_t  valueEnd,
                                IItem*&      pParent);
};

//---------------------------------------------------------------------------
// MHX2Reader::IValue
//---------------------------------------------------------------------------
template <class T>
MHX2Reader::IValue<T>::IValue() :
    IItem(),
    m_Value(T(0))
{}
//---------------------------------------------------------------------------
template <class T>
MHX2Reader::IValue<T>::IValue(T value) :
    IItem(),
    m_Value(value)
{}
//---------------------------------------------------------------------------
template <class T>
MHX2Reader::IValue<T>::IValue(const std::string& name, T value) :
    IItem(name),
    m_Value(value)
{}
//---------------------------------------------------------------------------
template <class T>
MHX2Reader::IValue<T>::~IValue()
{}
//---------------------------------------------------------------------------
template <class T>
T MHX2Reader::IValue<T>::GetValue() const
{
    return m_Value;
}
//---------------------------------------------------------------------------
template <class T>
void MHX2Reader::IValue<T>::SetValue(T value)
{
    m_Value = value;
}
//---------------------------------------------------------------------------
