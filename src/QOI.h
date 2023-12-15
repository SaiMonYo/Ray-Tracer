#pragma once

#include <iostream>
#include <fstream>
#include "Vector.h"


// byte headers for QOI file
const int QOI_OP_RUN   = 0xc0;
const int QOI_OP_INDEX = 0x00;
const int QOI_OP_DIFF  = 0x40;
const int QOI_OP_LUMA  = 0x80;
const int QOI_OP_RGB   = 0xfe;
const int QOI_OP_RGBA  = 0xff;


inline int gen_key(Vector3 colour){
    return ((int)(colour.x * 3 + colour.y * 5 + colour.z * 7) % 64);
}

struct QOIWriter{
    std::ofstream& filestream;
    Vector3 previous_colour = Vector3(0);
    Vector3 lookup[64] = {};
    int run_length = 0;

    QOIWriter(std::ofstream& file, int width, int height) : filestream(file){
        // init attributes and write magic numbers and headers to file
        write32(0x716f6966);
        write32(width);
        write32(height);
        filestream << (unsigned char) 3;
        filestream << (unsigned char) 1;
    }

    // write 32 bit number to the file
    void write32(long value){
        filestream << (unsigned char) ((value & 0xff000000) >> 24);
        filestream << (unsigned char) ((value & 0x00ff0000) >> 16);
        filestream << (unsigned char) ((value & 0x0000ff00) >> 8);
        filestream << (unsigned char) ((value & 0x000000ff));
    }


    void write_pixel(Vector3 pixel){
        // clamp pixel values between 0-255
        pixel = Vector3::apply(pixel, [](float a){return floorf(a);});
        pixel = Vector3::apply(pixel, [](float a){return fmax(0, fmin(a, 255));});

        // run length encoding
        if (pixel == previous_colour){
            run_length++;
            if (run_length == 62){
                filestream << (unsigned char) (QOI_OP_RUN | (run_length - 1));
                run_length = 0;
            }
        }
        else{
            // write previous run to the file
            if (run_length > 0){
                filestream << (unsigned char) (QOI_OP_RUN | (run_length - 1));
                run_length = 0;
            }

            // look to see if weve recently had the pixel colour
            int key = gen_key(pixel);
            if (pixel == lookup[key]){
                filestream << (unsigned char) (QOI_OP_INDEX | key);
            }
            else{
                // put the pixel into the buffer
                lookup[key] = pixel;
                // calculate the difference in colours
                Vector3 diff = pixel - previous_colour;
                int dr_dg = diff.x - diff.y;
                int db_dg = diff.z - diff.y;

                if ((-2 <= diff.x && diff.x <= 1) && (-2 <= diff.y && diff.y <= 1) && (-2 <= diff.z && diff.z <= 1)){
                    filestream << (unsigned char) (QOI_OP_DIFF | (((int)diff.x + 2)) << 4 | (((int)diff.y + 2) << 2) | ((int)diff.z + 2));
                }
                // larger differeence of -32 to 31 in green channel and -8 to 7 in red and blue channels
                else if (-32 <= diff.y && diff.y <= 31 && -8 <= dr_dg && dr_dg <= 7 && -8 <= db_dg && db_dg <= 7){
                    filestream << (unsigned char) (QOI_OP_LUMA | ((int)diff.y + 32));
                    filestream << (unsigned char) (((dr_dg + 8) << 4) | (db_dg + 8));
                }
                else{
                    // no encoding possible so write RGB header and RGB values
                    filestream << (unsigned char) QOI_OP_RGB;
                    filestream << (unsigned char) (int)pixel.x;
                    filestream << (unsigned char) (int)pixel.y;
                    filestream << (unsigned char) (int)pixel.z;
                }
            }
        }
        previous_colour = pixel;
    }

    void finish_run(){
        // write any remaining run length
        if (run_length > 0){
            filestream << (unsigned char) (QOI_OP_RUN | (run_length - 1));
        }
    }
};


struct QOIReader{
    std::ifstream& filestream;
    Vector3 previous_colour = Vector3(0);
    Vector3 lookup[64] = {};
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colour_space;

    uint32_t read32(){
        uint32_t value = 0;
        value |= filestream.get() << 24;
        value |= filestream.get() << 16;
        value |= filestream.get() << 8;
        value |= filestream.get();
        return value;
    }

    QOIReader(std::ifstream& file) : filestream(file){
        std::string header;
        for (int i = 0; i < 4; i++){
            header += filestream.get();
        }
        if (header != "qoif"){
            std::cerr << "Invalid QOI file" << std::endl;
            return;
        }
        // read in the magic numbers and headers
        width = read32();
        height = read32();
        channels = filestream.get();
        colour_space = filestream.get();
    }

    void read_all(std::vector<Vector3>& pixels){
        // go through all the file
        while (!filestream.eof()){
            // get the current byte
            uint8_t byte = filestream.get();
            // no encoding
            if (byte == QOI_OP_RGB){
                Vector3 pixel;
                pixel.x = filestream.get();
                pixel.y = filestream.get();
                pixel.z = filestream.get();
                pixels.push_back(pixel / 255.0);
                // add current pixel to lookup
                // and set previous colour to it
                previous_colour = pixel;
                int key = gen_key(pixel);
                lookup[key] = pixel;
            }
            // run of same pixels
            else if ((byte & 0xc0) == QOI_OP_RUN){
                int run_length = (byte & 0x3f) + 1;
                // add that many pixels to the vector
                for (int i = 0; i < run_length; i++){
                    pixels.push_back(previous_colour / 255.0);
                }
            }
            // seen pixel in the buffer
            else if ((byte & 0xc0) == QOI_OP_INDEX){
                int key = byte & 0x3f;
                Vector3 pixel = lookup[key];
                pixels.push_back(pixel / 255.0);
                previous_colour = pixel;
            }
            // small difference in r, g, b values
            else if ((byte & 0xc0) == QOI_OP_DIFF){
                int dr = ((byte & 0x30) >> 4) - 2;
                int dg = ((byte & 0x0c) >> 2) - 2;
                int db = (byte & 0x03) - 2;
                // should stay under 255 but mod just incase
                previous_colour.x = int(previous_colour.x + dr) % 256;
                previous_colour.y = int(previous_colour.y + dg) % 256;
                previous_colour.z = int(previous_colour.z + db) % 256;
                pixels.push_back(previous_colour / 255.0);
                int key = gen_key(previous_colour);
                lookup[key] = previous_colour;
            }
            // larger difference in Green channel
            else if ((byte & 0xc0) == QOI_OP_LUMA){
                // get green difference
                int dg = (byte & 0x3f) - 32;
                // get next bit for R, B channel diffs
                byte = filestream.get();
                int dr_dg = ((byte & 0xf0) >> 4) - 8;
                int db_dg = (byte & 0x0f) - 8;
                int dr = dr_dg + dg;
                int db = db_dg + dg;
                previous_colour.x = int(previous_colour.x + dr) % 256;
                previous_colour.y = int(previous_colour.y + dg) % 256;
                previous_colour.z = int(previous_colour.z + db) % 256;
                pixels.push_back(previous_colour / 255.0);
                int key = gen_key(previous_colour);
                lookup[key] = previous_colour;
            }
            // unkown encoding / error in program
            else{
                std::cerr << std::hex << (int)byte << std::endl;
            }
        }
    }
};