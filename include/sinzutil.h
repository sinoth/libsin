#ifndef ZLIB_UTIL_H
#define ZLIB_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

#define CHUNK 16384

struct sinz {

    /* Compress from source to dest until EOF on source.
       def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
       allocated for processing, Z_STREAM_ERROR if an invalid compression
       level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
       version of the library linked do not match, or Z_ERRNO if there is
       an error reading or writing the files. */

    static int zDef( uint8_t *source, int source_size,
                     uint8_t *dest, int &dest_size, const int &comp_level ) {

            int ret, flush;
            unsigned have;
            int bytes_avail;
            z_stream strm;
            //char in[CHUNK];
            //char out[CHUNK];

            // allocate deflate state
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            ret = deflateInit(&strm, comp_level);
            if (ret != Z_OK)
                return ret;

            dest_size = 0;

            // compress until end of file
            do {
                if ( CHUNK <= source_size )
                     bytes_avail = CHUNK;
                else bytes_avail = source_size;

                strm.avail_in = bytes_avail;
                strm.next_in = source;

                flush = ( (bytes_avail <= CHUNK) ? Z_FINISH : Z_NO_FLUSH);

                // run deflate() on input until output buffer not full, finish
                // compression if all of source has been read in
                do {
                    strm.avail_out = CHUNK;
                    strm.next_out = dest;
                    ret = deflate(&strm, flush);    // no bad return value
                    assert(ret != Z_STREAM_ERROR);  // state not clobbered
                    have = CHUNK - strm.avail_out;
                    dest += have;
                    dest_size += have;
                } while (strm.avail_out == 0);
                assert(strm.avail_in == 0);     // all input will be used

                source += CHUNK;
                source_size -= CHUNK;

                // done when last data in file processed
            } while (flush != Z_FINISH);
            assert(ret == Z_STREAM_END);        // stream will be complete

            // clean up and return
            (void)deflateEnd(&strm);
            return Z_OK;
        }


    /* Decompress from source to dest until stream ends or EOF
       inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
       allocated for processing, Z_DATA_ERROR if the deflate data is
       invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
       the version of the library linked do not match, or Z_ERRNO if there
       is an error reading or writing the files. */
    static int zInf(uint8_t *source, int source_size, uint8_t *dest, int &dest_size)
        {
            int ret;
            unsigned have;
            z_stream strm;
            //char in[CHUNK];
            //char out[CHUNK];
            int bytes_avail;

            // allocate
            // inflate state
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;

            strm.avail_in = 0;
            strm.next_in = Z_NULL;

            ret = inflateInit(&strm);
            if (ret != Z_OK)
                return ret;

            dest_size = 0;

            // decompress until deflate stream ends or end of file
            do {

                if ( CHUNK <= source_size )
                     bytes_avail = CHUNK;
                else bytes_avail = source_size;

                strm.avail_in = bytes_avail;

                if (strm.avail_in == 0)
                    break;
                strm.next_in = source;

                // run inflate() on input until output buffer not full
                do {
                    strm.avail_out = CHUNK;
                    strm.next_out = dest;
                    ret = inflate(&strm, Z_NO_FLUSH);
                    assert(ret != Z_STREAM_ERROR);  // state not clobbered
                        switch (ret) {
                        case Z_NEED_DICT:
                            ret = Z_DATA_ERROR; // and fall through
                        case Z_DATA_ERROR:
                        case Z_MEM_ERROR:
                            (void)inflateEnd(&strm);
                            return ret;
                        }
                    have = CHUNK - strm.avail_out;
                    dest += have;
                    dest_size += have;

                } while (strm.avail_out == 0);

                source += CHUNK;
                source_size -= CHUNK;

                // done when inflate() says it's done
            } while (ret != Z_STREAM_END);

            // clean up and return
            (void)inflateEnd(&strm);
            return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
        }

    // report a zlib or i/o error
    static void zerr(int ret)
        {
            fputs("zpipe: ", stderr);
            switch (ret) {
            case Z_ERRNO:
                if (ferror(stdin))
                    fputs("error reading stdin\n", stderr);
                if (ferror(stdout))
                    fputs("error writing stdout\n", stderr);
                break;
            case Z_STREAM_ERROR:
                fputs("invalid compression level\n", stderr);
                break;
            case Z_DATA_ERROR:
                fputs("invalid or incomplete deflate data\n", stderr);
                break;
            case Z_MEM_ERROR:
                fputs("out of memory\n", stderr);
                break;
            case Z_VERSION_ERROR:
                fputs("zlib version mismatch!\n", stderr);
            }
        }

};

#endif
