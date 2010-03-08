// code used from website:
// http://www.networkdls.com/Software.Asp?Review=22
// license is LGPL v3

#ifndef _CCRC32_H
#define _CCRC32_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

class CCRC32{

    public:
        CCRC32(void) { this->Initialize(); }
        ~CCRC32(void) {}

        void Initialize(void) {
                //0x04C11DB7 is the official polynomial used by PKZip, WinZip and Ethernet.
                unsigned long ulPolynomial = 0x04C11DB7;
                //memset(&this->ulTable, 0, sizeof(this->ulTable));

                // 256 values representing ASCII character codes.
                for(int iCodes = 0; iCodes <= 0xFF; iCodes++)
                {
                    this->ulTable[iCodes] = this->Reflect(iCodes, 8) << 24;

                    for(int iPos = 0; iPos < 8; iPos++)
                    {
                        this->ulTable[iCodes] = (this->ulTable[iCodes] << 1)
                            ^ ((this->ulTable[iCodes] & (1 << 31)) ? ulPolynomial : 0);
                    }

                    this->ulTable[iCodes] = this->Reflect(this->ulTable[iCodes], 32);
                }
            }

        bool FileCRC(const char *sFileName, unsigned long *ulOutCRC) {
                return this->FileCRC(sFileName, ulOutCRC, 1048576);
            }

        bool FileCRC(const char *sFileName, unsigned long *ulOutCRC, unsigned long ulBufferSize) {
                *(unsigned long *)ulOutCRC = 0xffffffff; //Initilaize the CRC.

                FILE *fSource = NULL;
                unsigned char *sBuf = NULL;
                int iBytesRead = 0;

                if((fSource = fopen(sFileName, "rb")) == NULL)
                {
                    return false; //Failed to open file for read access.
                }

                if(!(sBuf = (unsigned char *)malloc(ulBufferSize))) //Allocate memory for file buffering.
                {
                    fclose(fSource);
                    return false; //Out of memory.
                }

                while((iBytesRead = fread(sBuf, sizeof(char), ulBufferSize, fSource)))
                {
                    this->PartialCRC(ulOutCRC, sBuf, iBytesRead);
                }

                free(sBuf);
                fclose(fSource);

                *(unsigned long *)ulOutCRC ^= 0xffffffff; //Finalize the CRC.

                return true;
            }

        unsigned long FullCRC(const unsigned char *sData, unsigned long ulDataLength) {
                unsigned long ulCRC = 0xffffffff; //Initilaize the CRC.
                this->PartialCRC(&ulCRC, sData, ulDataLength);
                return(ulCRC ^ 0xffffffff); //Finalize the CRC and return.
            }

        void FullCRC(const unsigned char *sData, unsigned long ulDataLength, unsigned long *ulOutCRC) {
                *(unsigned long *)ulOutCRC = 0xffffffff; //Initilaize the CRC.
                this->PartialCRC(ulOutCRC, sData, ulDataLength);
                *(unsigned long *)ulOutCRC ^= 0xffffffff; //Finalize the CRC.
        }

        void PartialCRC(unsigned long *ulCRC, const unsigned char *sData, unsigned long ulDataLength) {
                while(ulDataLength--)
                {
                    //If your compiler complains about the following line, try changing each
                    //    occurrence of *ulCRC with "((unsigned long)*ulCRC)" or "*(unsigned long *)ulCRC".

                     *(unsigned long *)ulCRC =
                        ((*(unsigned long *)ulCRC) >> 8) ^ this->ulTable[((*(unsigned long *)ulCRC) & 0xFF) ^ *sData++];
                }
            }

    private:
        unsigned long Reflect(unsigned long ulReflect, const char cChar) {
                unsigned long ulValue = 0;

                // Swap bit 0 for bit 7, bit 1 For bit 6, etc....
                for(int iPos = 1; iPos < (cChar + 1); iPos++)
                {
                    if(ulReflect & 1)
                    {
                        ulValue |= (1 << (cChar - iPos));
                    }
                    ulReflect >>= 1;
                }

                return ulValue;
            }

        unsigned long ulTable[256]; // CRC lookup table array.
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
