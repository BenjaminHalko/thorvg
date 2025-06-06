/*
 * Copyright (c) 2021 - 2025 the ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// jpgd.h - C++ class for JPEG decompression.
// Public domain, Rich Geldreich <richgel99@gmail.com>
#ifndef _TVG_JPGD_H_
#define _TVG_JPGD_H_

#include <exception>
#include <string>

class jpeg_decoder;

class JpegDecoderException : public std::exception {
private:
    jpgd_status status;
    std::string message;

public:
    explicit JpegDecoderException(jpgd_status status) : status(status) {
        switch (status) {
            case JPGD_BAD_DHT_COUNTS: message = "Bad DHT counts"; break;
            case JPGD_BAD_DHT_INDEX: message = "Bad DHT index"; break;
            case JPGD_BAD_DHT_MARKER: message = "Bad DHT marker"; break;
            case JPGD_BAD_DQT_MARKER: message = "Bad DQT marker"; break;
            case JPGD_BAD_DQT_TABLE: message = "Bad DQT table"; break;
            case JPGD_BAD_PRECISION: message = "Bad precision"; break;
            case JPGD_BAD_HEIGHT: message = "Bad height"; break;
            case JPGD_BAD_WIDTH: message = "Bad width"; break;
            case JPGD_TOO_MANY_COMPONENTS: message = "Too many components"; break;
            case JPGD_BAD_SOF_LENGTH: message = "Bad SOF length"; break;
            case JPGD_BAD_VARIABLE_MARKER: message = "Bad variable marker"; break;
            case JPGD_BAD_DRI_LENGTH: message = "Bad DRI length"; break;
            case JPGD_BAD_SOS_LENGTH: message = "Bad SOS length"; break;
            case JPGD_BAD_SOS_COMP_ID: message = "Bad SOS component ID"; break;
            case JPGD_W_EXTRA_BYTES_BEFORE_MARKER: message = "Extra bytes before marker"; break;
            case JPGD_NO_ARITHMETIC_SUPPORT: message = "No arithmetic coding support"; break;
            case JPGD_UNEXPECTED_MARKER: message = "Unexpected marker"; break;
            case JPGD_NOT_JPEG: message = "Not a JPEG file"; break;
            case JPGD_UNSUPPORTED_MARKER: message = "Unsupported marker"; break;
            case JPGD_BAD_DQT_LENGTH: message = "Bad DQT length"; break;
            case JPGD_TOO_MANY_BLOCKS: message = "Too many blocks"; break;
            case JPGD_UNDEFINED_QUANT_TABLE: message = "Undefined quantization table"; break;
            case JPGD_UNDEFINED_HUFF_TABLE: message = "Undefined Huffman table"; break;
            case JPGD_NOT_SINGLE_SCAN: message = "Not a single scan"; break;
            case JPGD_UNSUPPORTED_COLORSPACE: message = "Unsupported colorspace"; break;
            case JPGD_UNSUPPORTED_SAMP_FACTORS: message = "Unsupported sampling factors"; break;
            case JPGD_DECODE_ERROR: message = "Decode error"; break;
            case JPGD_BAD_RESTART_MARKER: message = "Bad restart marker"; break;
            case JPGD_ASSERTION_ERROR: message = "Assertion error"; break;
            case JPGD_BAD_SOS_SPECTRAL: message = "Bad SOS spectral"; break;
            case JPGD_BAD_SOS_SUCCESSIVE: message = "Bad SOS successive"; break;
            case JPGD_STREAM_READ: message = "Stream read error"; break;
            case JPGD_NOTENOUGHMEM: message = "Not enough memory"; break;
            default: message = "Unknown error"; break;
        }
    }

    const char* what() const noexcept override {
        return message.c_str();
    }

    jpgd_status getStatus() const noexcept {
        return status;
    }
};

jpeg_decoder* jpgdHeader(const char* data, int size, int* width, int* height);
jpeg_decoder* jpgdHeader(const char* filename, int* width, int* height);
unsigned char* jpgdDecompress(jpeg_decoder* decoder);
void jpgdDelete(jpeg_decoder* decoder);

#endif //_TVG_JPGD_H_
