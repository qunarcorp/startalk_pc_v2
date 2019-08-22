//
// Created by QITMAC000260 on 2019-05-08.
//

#include "VoiceHelper.h"
#include "wavreader.h"
#include "../Depends/amr/include/wrapper.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

namespace QTalk {

    namespace VoiceHelper{

        const int sizes[] = { 12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0 };

        bool amrToWav(const std::string& szAmrFileName, const std::string& wavFileName)
        {
#ifdef Q_NO_AMR
            char header[6];
            int n;
            FILE* in;
            void *amr;
            void *amrDecoder;

            in = std::fopen(szAmrFileName.data(), "rb");
            if (!in)
                return false;

            //check the file format
            n = fread(header, 1, 6, in);
            if (n != 6 || memcmp(header, "#!AMR\n", 6)) {
                fprintf(stderr, "Bad header\n");
                return false;
            }
            amr =Decoder_Interface_init();

            std::ostringstream wavData;

            while (true)
            {

                uint8_t buffer[500], littleendian[320], *ptr;
                int size, i;
                int16_t outbuffer[160];

                //default read frame to buffer
                /* Read the mode byte */
                n = fread(buffer, 1, 1, in);
                if (n <= 0)
                    break;

                /* Find the packet size */
                size = sizes[(buffer[0] >> 3) & 0x0f];
                n = fread(buffer + 1, 1, size, in);
                if (n != size)
                    break;

                /* Decode the packet */
                Decoder_Interface_Decode(amr, buffer, outbuffer, 0);

                /* Convert to little endian and write to wav */
                ptr = littleendian;
                for (i = 0; i < 160; i++) {
                    *ptr++ = (outbuffer[i] >> 0) & 0xff;
                    *ptr++ = (outbuffer[i] >> 8) & 0xff;
                }

                wavData << std::string((const char *)littleendian, 320);
            }

            fclose(in);
            Decoder_Interface_exit(amr);

            ofstream out;
            std::string data = wavData.str();
            out.open(wavFileName.c_str(), std::ios::out | std::ios::binary);
            if (out.is_open()) {
                out.write(data.c_str(), data.size());
            }
#endif
            return true;
        }

    }
}