#ifndef ZLIB_UTIL_H
#define ZLIB_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

// chunks don't make sense when the input is already in memory
//#define CHUNK 128
//#define CHUNK 16384
//#define CHUNK 2097152

struct sinz {

    /* Compress from source to dest until EOF on source.
       def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
       allocated for processing, Z_STREAM_ERROR if an invalid compression
       level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
       version of the library linked do not match, or Z_ERRNO if there is
       an error reading or writing the files. */

    static int zDef( uint8_t *source, int source_size,
                     uint8_t *dest, int &dest_size, const int comp_level=6) {

            int ret;
            int orig_dest_size;
            z_stream strm;

            // allocate deflate state
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;

            ret = deflateInit(&strm, comp_level);
            if (ret != Z_OK) {
                printf("zLib: error on deflateInit\n");
                return ret;
            }

            strm.avail_in = source_size;
            strm.next_in = source;

            if ( dest_size == 0 ) {
                orig_dest_size = source_size;
            	strm.avail_out = source_size;
            } else {
                orig_dest_size = dest_size;
                strm.avail_out = dest_size;
            }
            strm.next_out = dest;

            dest_size = 0;

            ret = deflate(&strm, Z_FINISH); // no bad return value
            assert(ret != Z_STREAM_ERROR);  // state not clobbered

            assert(strm.avail_in == 0);     // all input will be used
            assert(ret == Z_STREAM_END);    // stream will be complete

            dest_size = orig_dest_size - strm.avail_out;

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
            int expected_dest_size = dest_size;
            z_stream strm;

            // inflate state
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;

            ret = inflateInit(&strm);
            if (ret != Z_OK) {
                printf("zLib: error on inflateInit\n");
                return ret;
            }

            strm.avail_in = source_size;
            strm.next_in = source;

            strm.avail_out = dest_size;
            strm.next_out = dest;

            dest_size = 0;

            ret = inflate(&strm, Z_FINISH);
            assert(ret != Z_STREAM_ERROR);  // state not clobbered
            if ( ret != Z_STREAM_END ) {
                printf("zLib: not enough space to inflate data! (%d)\n", ret);
                return Z_DATA_ERROR;
            }
            dest_size = expected_dest_size - strm.avail_out;

            // clean up and return
            (void)inflateEnd(&strm);

            if ( dest_size != expected_dest_size ) {
                printf("zLib: data size (%d) does not match expected size (%d)\n", dest_size, expected_dest_size);
                return Z_DATA_ERROR;
            } else
                return Z_OK;
        }

};

#endif
