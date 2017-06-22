#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sdf.h"
#ifdef PARALLEL
#include <mpi.h>
#endif


#define SDF_COMMON_INFO() do { \
if (!h->current_block || !h->current_block->done_header) { \
if (h->rank == h->rank_master) { \
fprintf(stderr, "*** ERROR ***\n"); \
fprintf(stderr, "SDF block header has not been read." \
" Ignoring call.\n"); \
} \
return 1; \
} \
b = h->current_block; \
if (b->done_info) return 0; \
h->current_location = b->block_start + h->block_header_length; \
b->done_info = 1; } while(0)

int sdf_read_header(sdf_file_t *h)
{
    int buflen;
    
    h->indent = 0;
    
    if (h->done_header) return 1;
    
    buflen = SDF_HEADER_LENGTH;
    h->buffer = malloc(buflen);
    
    h->current_location = h->start_location = 0;
    
    if (h->rank == h->rank_master) {
        sdf_seek(h);
        sdf_read_bytes(h, h->buffer, buflen);
    }
    sdf_broadcast(h, h->buffer, buflen);
    
    // If this isn't SDF_MAGIC then this isn't a SDF file;
    if (memcmp(h->buffer, SDF_MAGIC, 4) != 0) {
        sdf_close(h);
        return -1;
    }
    
    h->current_location += 4;
    
    SDF_READ_ENTRY_INT4(h->endianness);
    if (h->endianness == 0x0f0e0201) h->swap = 1;
    
    SDF_READ_ENTRY_INT4(h->file_version);
    if (h->file_version > SDF_VERSION) {
        sdf_close(h);
        return -1;
    }
    
    SDF_READ_ENTRY_INT4(h->file_revision);
    
    SDF_READ_ENTRY_ID(h->code_name);
    
    SDF_READ_ENTRY_INT8(h->first_block_location);
    
    SDF_READ_ENTRY_INT8(h->summary_location);
    
    SDF_READ_ENTRY_INT4(h->summary_size);
    
    SDF_READ_ENTRY_INT4(h->nblocks);
    
    SDF_READ_ENTRY_INT4(h->block_header_length);
    
    SDF_READ_ENTRY_INT4(h->step);
    
    SDF_READ_ENTRY_REAL8(h->time);
    
    SDF_READ_ENTRY_INT4(h->jobid1);
    
    SDF_READ_ENTRY_INT4(h->jobid2);
    
    SDF_READ_ENTRY_INT4(h->string_length);
    
    SDF_READ_ENTRY_INT4(h->code_io_version);
    
    SDF_READ_ENTRY_LOGICAL(h->restart_flag);
    
    SDF_READ_ENTRY_LOGICAL(h->other_domains);
    
    free(h->buffer);
    h->buffer = NULL;
    
    h->current_location = SDF_HEADER_LENGTH;
    h->done_header = 1;
    
    if (h->summary_size == 0) h->use_summary = 0;
    
    return 0;
}