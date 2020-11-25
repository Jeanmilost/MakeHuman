/****************************************************************************
 * ==> PngTextureHelper ----------------------------------------------------*
 ****************************************************************************
 * Description : PNG texture helper                                         *
 * Developer   : Jean-Milost Reymond                                        *
 ****************************************************************************
 * MIT License - x model reader                                             *
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

#include "PngTextureHelper.h"

// std
#include <memory>

//------------------------------------------------------------------------------
bool PngTextureHelper::OpenBitmapData(const std::string& fileName,
                                            std::size_t& width,
                                            std::size_t& height,
                                            std::size_t& format,
                                            std::size_t& length,
                                            void*&       pPixels)
{
    // no file name?
    if (fileName.empty())
        return false;

    png_image image;

    std::memset(&image, 0, (sizeof image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image, fileName.c_str()) != 0)
    {
        // set a simple sRGB format with a non-associated alpha channel as png file format to read
        image.format = PNG_FORMAT_BGR;

        // allocate enough memory to hold the image in this format
        png_bytep pBuffer = (png_bytep)std::malloc(PNG_IMAGE_SIZE(image));

        if (pBuffer && png_image_finish_read(&image, NULL, pBuffer, 0, NULL) != 0)
            return LoadBitmapData(image, pBuffer, width, height, format, length, pPixels);
        else
        if (!pBuffer)
            png_image_free(&image);
        else
            std::free(pBuffer);
    }

    return false;
}
//------------------------------------------------------------------------------
bool PngTextureHelper::LoadBitmapData(const png_image&   image,
                                      const png_bytep    pBuffer,
                                            std::size_t& width,
                                            std::size_t& height,
                                            std::size_t& format,
                                            std::size_t& length,
                                            void*&       pPixels)
{
    width   = image.width;
    height  = image.height;
    format  = 24;// image.format * 8;
    length  = PNG_IMAGE_SIZE(image);
    pPixels = NULL;

    unsigned char* pBitmapData = NULL;

    try
    {
        const unsigned bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
        const unsigned bitmapSize = bytesPerRow * height;

        pBitmapData = new unsigned char[bitmapSize];

        // read bitmap data
        std::memcpy(pBitmapData, pBuffer, bitmapSize);

        // search for bitmap format
        switch (format)
        {
            case 24:
            {
                // calculate pixels array length
                length = (width * height * 3);

                unsigned char* pPixelArray = new unsigned char[length];

                // get bitmap data in right format
                for (unsigned y = 0; y < height; ++y)
                    for (unsigned x = 0; x < width; ++x)
                        for (unsigned char c = 0; c < 3; ++c)
                            pPixelArray[3 * (width * y + x) + c] =
                                    pBitmapData[(bytesPerRow * ((height - 1) - y)) + (3 * x) + (2 - c)];

                pPixels = pPixelArray;
                break;
            }

            default: throw new std::exception("Unknown format");
        }
    }
    catch (...)
    {
        // clear memory
        delete[](unsigned char*)pPixels;
        delete[] pBitmapData;

        throw;
    }

    // clear memory
    delete[] pBitmapData;

    return true;
}
//---------------------------------------------------------------------------
