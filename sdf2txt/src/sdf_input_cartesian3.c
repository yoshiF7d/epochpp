#include "sdf2.h"
#include <stdlib.h>

//#define SDF_COMMON_MESH_LENGTH (4 + 8 + SDF_ID_LENGTH + 4 * b->ndims)

#define SDF_COMMON_MESH_INFO() do { \
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


int sdf_read_plain_mesh_info(sdf_file_t *h)
{
    sdf_block_t *b;
    int i;
    uint32_t dims_in[SDF_MAXDIMS];
    uint32_t *dims_ptr = dims_in;

    // Metadata is
    // - mults     REAL(r8), DIMENSION(ndims)
    // - labels    CHARACTER(id_length), DIMENSION(ndims)
    // - units     CHARACTER(id_length), DIMENSION(ndims)
    // - geometry  INTEGER(i4)
    // - minval    REAL(r8), DIMENSION(ndims)
    // - maxval    REAL(r8), DIMENSION(ndims)
    // - dims      INTEGER(i4), DIMENSION(ndims)

    SDF_COMMON_MESH_INFO();

    SDF_READ_ENTRY_ARRAY_REAL8(b->dim_mults, b->ndims);

    SDF_READ_ENTRY_ARRAY_ID(b->dim_labels, b->ndims);

    SDF_READ_ENTRY_ARRAY_ID(b->dim_units, b->ndims);

    SDF_READ_ENTRY_INT4(b->geometry);

    SDF_READ_ENTRY_ARRAY_REAL8(b->extents, 2*b->ndims);

    SDF_READ_ENTRY_ARRAY_INT4(dims_ptr, b->ndims);
    for (i = 0; i < b->ndims; i++) b->dims[i] = dims_in[i];

    b->stagger = SDF_STAGGER_VERTEX;
    for (i = 0; i < b->ndims; i++) b->const_value[i] = 1;

    return 0;
}



int sdf_read_plain_variable_info(sdf_file_t *h)
{
    sdf_block_t *b;
    int i;
    uint32_t dims_in[SDF_MAXDIMS];
    uint32_t *dims_ptr = dims_in;

    // Metadata is
    // - mult      REAL(r8)
    // - units     CHARACTER(id_length)
    // - meshid    CHARACTER(id_length)
    // - dims      INTEGER(i4), DIMENSION(ndims)
    // - stagger   INTEGER(i4)

    SDF_COMMON_MESH_INFO();

    SDF_READ_ENTRY_REAL8(b->mult);

    SDF_READ_ENTRY_ID(b->units);

    SDF_READ_ENTRY_ID(b->mesh_id);

    SDF_READ_ENTRY_ARRAY_INT4(dims_ptr, b->ndims);
    for (i = 0; i < b->ndims; i++) b->dims[i] = dims_in[i];

    SDF_READ_ENTRY_INT4(b->stagger);
    for (i = 0; i < b->ndims; i++) b->const_value[i] = (b->stagger & 1<<i);

#ifdef PARALLEL
    // Calculate per block parallel factorisation
    // This will be fixed up later once we have the whole block list.
    sdf_factor(h);
#else
    b->nelements_local = 1;
    for (i=0; i < b->ndims; i++) {
        b->local_dims[i] = (int)b->dims[i];
        b->nelements_local *= b->dims[i];
    }
    for (i=b->ndims; i < 3; i++) b->local_dims[i] = 1;
#endif

    return 0;
}



static int sdf_create_1d_distribution(sdf_file_t *h, int global, int local,
        int start)
{
#ifdef PARALLEL
    sdf_block_t *b = h->current_block;
    int lengths[3];
    MPI_Aint disp[3];
    MPI_Datatype types[3];

    lengths[0] = 1;
    lengths[1] = local;
    lengths[2] = 1;
    disp[0] = 0;
    disp[1] = start * SDF_TYPE_SIZES[b->datatype];
    disp[2] = global * SDF_TYPE_SIZES[b->datatype];
    types[0] = MPI_LB;
    types[1] = b->mpitype;
    types[2] = MPI_UB;

    MPI_Type_create_struct(3, lengths, disp, types, &b->distribution);
    MPI_Type_commit(&b->distribution);
#endif
    return 0;
}



static int sdf_plain_mesh_distribution(sdf_file_t *h)
{
#ifdef PARALLEL
    sdf_block_t *b = h->current_block;
    int n;
    int sizes[SDF_MAXDIMS];

    for (n=0; n < b->ndims; n++) {
        b->dims[n] -= 2 * b->ng;
        b->local_dims[n] -= 2 * b->ng;
        sizes[n] = (int)b->dims[n];
    }

    // Get starts for creating subarray
    sdf_factor(h);

    MPI_Type_create_subarray(b->ndims, sizes, b->local_dims, b->starts,
        MPI_ORDER_FORTRAN, b->mpitype, &b->distribution);
    MPI_Type_commit(&b->distribution);

    b->nelements_local = 1;
    for (n=0; n < b->ndims; n++) {
        b->dims[n] += 2 * b->ng;
        b->local_dims[n] += 2 * b->ng;
        b->nelements_local *= b->local_dims[n];
    }
#endif

    return 0;
}



static int sdf_free_distribution(sdf_file_t *h)
{
#ifdef PARALLEL
    sdf_block_t *b = h->current_block;

    if (b->ng) return 0;

    MPI_Type_free(&b->distribution);
#endif
    return 0;
}



static int sdf_helper_read_array_halo(sdf_file_t *h, void **var_in)
{
	fprintf(stderr,"sdf_read_array_halo\n");
    sdf_block_t *b = h->current_block;
    char **var_ptr = (char**)var_in;
    char *var = *var_ptr;
    char convert;
    int count, sz;
#ifdef PARALLEL
    MPI_Datatype distribution, facetype;
    int subsizes[SDF_MAXDIMS];
    int face[SDF_MAXDIMS];
    int i, tag;
    uint64_t offset;
    char *p1, *p2;
#else
    char *vptr;
    int i, j, k;
    int nx, ny, nz;
    int j0, j1, k0, k1;
    int subsize;
#endif

    count = b->nelements_local;

    if (*var_ptr) free(*var_ptr);
    sz = SDF_TYPE_SIZES[b->datatype];
    *var_ptr = var = malloc(count * sz);

#ifdef PARALLEL
    for (i=0; i < b->ndims; i++) {
        subsizes[i] = b->local_dims[i] - 2 * b->ng;
        b->starts[i] = b->ng;
    }
    for (i=b->ndims; i < SDF_MAXDIMS; i++)
        subsizes[i] = 1;

    MPI_Type_create_subarray(b->ndims, b->local_dims, subsizes, b->starts,
        MPI_ORDER_FORTRAN, b->mpitype, &distribution);

    MPI_Type_commit(&distribution);

    MPI_File_set_view(h->filehandle, h->current_location, b->mpitype,
            b->distribution, "native", MPI_INFO_NULL);
    MPI_File_read_all(h->filehandle, var, 1, distribution,
            MPI_STATUS_IGNORE);
    MPI_File_set_view(h->filehandle, 0, MPI_BYTE, MPI_BYTE, "native",
            MPI_INFO_NULL);

    MPI_Type_free(&distribution);

    // Swap ghostcell faces
    for (i=0; i < b->ndims; i++) {
        face[i] = b->local_dims[i] - 2 * b->ng;
        b->starts[i] = b->ng;
    }

    tag = 1;
    offset = sz;
    for (i=0; i < b->ndims; i++) {
        face[i] = b->ng;
        b->starts[i] = 0;

        MPI_Type_create_subarray(b->ndims, b->local_dims, face, b->starts,
            MPI_ORDER_FORTRAN, b->mpitype, &facetype);
        MPI_Type_commit(&facetype);

        p1 = (char*)b->data + b->ng * offset;
        p2 = (char*)b->data + (b->local_dims[i] - b->ng) * offset;
        MPI_Sendrecv(p1, 1, facetype, b->proc_min[i], tag, p2, 1, facetype,
            b->proc_max[i], tag, h->comm, MPI_STATUS_IGNORE);
        tag++;

        p1 = (char*)b->data + (b->local_dims[i] - 2 * b->ng) * offset;
        p2 = b->data;
        MPI_Sendrecv(p1, 1, facetype, b->proc_max[i], tag, p2, 1, facetype,
            b->proc_min[i], tag, h->comm, MPI_STATUS_IGNORE);
        tag++;

        MPI_Type_free(&facetype);

        face[i] = b->local_dims[i];
        offset *= b->local_dims[i];
    }
#else
    j0 = k0 = 0;
    j1 = k1 = ny = nz = 1;

    i = b->ng;
    nx = b->local_dims[0];
    subsize = nx - 2 * i;

    if (b->ndims > 1) {
        ny = b->local_dims[1];
        j0 = b->ng;
        j1 = ny - b->ng;
    }
    if (b->ndims > 2) {
        nz = b->local_dims[2];
        k0 = b->ng;
        k1 = nz - b->ng;
    }

    for (k = k0; k < k1; k++) {
    for (j = j0; j < j1; j++) {
        vptr = var + (i + nx * (j + ny * k)) * sz;
        fseeko(h->filehandle, h->current_location, SEEK_SET);
        if (!fread(vptr, sz, subsize, h->filehandle)) return 1;
        h->current_location += subsize * sz;
    }}
#endif

    if (h->use_float && b->datatype == SDF_DATATYPE_REAL8)
        convert = 1;
    else
        convert = 0;

    if (convert) {
        int i;
        float *r4;
        double *old_var, *r8;
        r8 = old_var = (double*)var;
        r4 = (float*)(*var_ptr);
        if (!r4) {
            *var_ptr = malloc(count * sizeof(float));
            r4 = (float*)(*var_ptr);
        }
        for (i=0; i < count; i++)
            *r4++ = (float)(*r8++);
        if (!h->mmap) free(old_var);
        b->datatype_out = SDF_DATATYPE_REAL4;
#ifdef PARALLEL
        b->mpitype_out = MPI_FLOAT;
#endif
    }

    return 0;
}



static int sdf_helper_read_array(sdf_file_t *h, void **var_in, int count)
{
    sdf_block_t *b = h->current_block;
    char **var_ptr = (char**)var_in;
    char *var = *var_ptr;
    char convert;
    int sz;
	
	fprintf(stderr,"sdf_helper_read_array\n");	
	if (b->ng) return sdf_helper_read_array_halo(h, var_in);

    if (h->mmap) {
        *var_ptr = h->mmap + h->current_location;
        return 0;
    }

    sz = SDF_TYPE_SIZES[b->datatype];
    if (h->use_float && b->datatype == SDF_DATATYPE_REAL8) {
        convert = 1;
        *var_ptr = var = malloc(count * sz);
    } else {
        convert = 0;
        if (!var) *var_ptr = var = malloc(count * sz);
    }

#ifdef PARALLEL
    MPI_File_set_view(h->filehandle, h->current_location, b->mpitype,
            b->distribution, "native", MPI_INFO_NULL);
    MPI_File_read_all(h->filehandle, var, count, b->mpitype,
            MPI_STATUS_IGNORE);
    MPI_File_set_view(h->filehandle, 0, MPI_BYTE, MPI_BYTE, "native",
            MPI_INFO_NULL);
#else
    fseeko(h->filehandle, h->current_location, SEEK_SET);
    if (!fread(var, sz, count, h->filehandle)) return 1;
#endif
    if (h->swap) {
        if (b->datatype == SDF_DATATYPE_INTEGER4
                || b->datatype == SDF_DATATYPE_REAL4) {
            int i;
            uint32_t *v = (uint32_t*)*var_ptr;
            for (i=0; i < count; i++) {
                _SDF_BYTE_SWAP32(*v);
                v++;
            }
        } else if (b->datatype == SDF_DATATYPE_INTEGER8
                || b->datatype == SDF_DATATYPE_REAL8) {
            int i;
            uint64_t *v = (uint64_t*)*var_ptr;
            for (i=0; i < count; i++) {
                _SDF_BYTE_SWAP64(*v);
                v++;
            }
        }
    }

    if (convert) {
        int i;
        float *r4;
        double *old_var, *r8;
        r8 = old_var = (double*)var;
        r4 = (float*)(*var_ptr);
        if (!r4) {
            *var_ptr = malloc(count * sizeof(float));
            r4 = (float*)(*var_ptr);
        }
        for (i=0; i < count; i++)
            *r4++ = (float)(*r8++);
        if (!h->mmap) free(old_var);
        b->datatype_out = SDF_DATATYPE_REAL4;
#ifdef PARALLEL
        b->mpitype_out = MPI_FLOAT;
#endif
    }

    return 0;
}



int sdf_read_plain_mesh(sdf_file_t *h)
{
	fprintf(stderr,"sdf_read_plain_mesh\n");
    sdf_block_t *b = h->current_block;
    int n;

    if (b->done_data) return 0;
    if (!b->done_info) sdf_read_blocklist(h);

    sdf_factor(h);

    h->current_location = b->data_location;

    if (!b->grids) b->grids = calloc(3, sizeof(float*));

    if (h->print) {
        h->indent = 0;
        SDF_DPRNT("\n");
        SDF_DPRNT("b->name: %s ", b->name);
        for (n=0; n<b->ndims; n++) SDF_DPRNT("%i ",b->local_dims[n]);
        SDF_DPRNT("\n");
        h->indent = 2;
    }
    for (n = 0; n < 3; n++) {
        if (b->ndims > n) {
#ifdef PARALLEL
            sdf_create_1d_distribution(h, (int)b->dims[n], b->local_dims[n],
                    b->starts[n]);
#endif
            sdf_helper_read_array(h, &b->grids[n], b->local_dims[n]);
            sdf_free_distribution(h);
            if (h->print) {
                SDF_DPRNT("%s: ", b->dim_labels[n]);
                SDF_DPRNTar(b->grids[n], b->local_dims[n]);
            }
            h->current_location = h->current_location
                    + SDF_TYPE_SIZES[b->datatype] * b->dims[n];
        } else {
            b->grids[n] = calloc(1, SDF_TYPE_SIZES[b->datatype]);
        }
    }

    b->done_data = 1;

    return 0;
}



int sdf_read_lagran_mesh(sdf_file_t *h)
{
    sdf_block_t *b = h->current_block;
    int n;
    uint64_t nelements = 1;
	
	fprintf(stderr,"sdf_read_lagran_mesh\n");
    if (b->done_data) return 0;
    if (!b->done_info) sdf_read_blocklist(h);

    sdf_factor(h);

    h->current_location = b->data_location;

    if (!b->grids) b->grids = calloc(3, sizeof(float*));

    if (h->print) {
        h->indent = 0;
        SDF_DPRNT("\n");
        SDF_DPRNT("b->name: %s ", b->name);
        for (n=0; n<b->ndims; n++) SDF_DPRNT("%i ",b->local_dims[n]);
        SDF_DPRNT("\n");
        h->indent = 2;
    }

    sdf_plain_mesh_distribution(h);

    for (n = 0; n < b->ndims; n++) nelements *= b->dims[n];

    for (n = 0; n < 3; n++) {
        if (b->ndims > n) {
            sdf_helper_read_array(h, &b->grids[n], b->nelements_local);
            sdf_convert_array_to_float(h, &b->grids[n], b->nelements_local);
            if (h->print) {
                SDF_DPRNT("%s: ", b->dim_labels[n]);
                SDF_DPRNTar(b->grids[n], b->nelements_local);
            }
            h->current_location = h->current_location
                    + SDF_TYPE_SIZES[b->datatype] * nelements;
        } else {
            b->grids[n] = calloc(1, SDF_TYPE_SIZES[b->datatype]);
        }
    }

    sdf_free_distribution(h);

    b->done_data = 1;

    return 0;
}



int sdf_read_plain_variable(sdf_file_t *h)
{
	Data data;
	//fprintf(stderr,"sdf_read_plain_variable\n");
    sdf_block_t *b = h->current_block;
	int i,j,n,row,col;
	double *double_array;
	float *float_array;

    if (b->done_data) return 0;
    if (!b->done_info) sdf_read_plain_variable_info(h);

    h->current_location = b->data_location;

    sdf_plain_mesh_distribution(h);

    sdf_helper_read_array(h, &b->data, b->nelements_local);

    sdf_free_distribution(h);
	
	if(b->datatype_out == SDF_DATATYPE_REAL8){
		double_array = b->data;
		col = b->local_dims[0];
		n = b->nelements_local;
		row = n/col;
		data = Data_create(row,col);
		for(i=0;i<n;i++){
			Data_set(data,i/col,i%col,double_array[i]);
		}
		Data_output(data,fileout,p_float);
		Data_delete(data);
	}else if(b->datatype_out == SDF_DATATYPE_REAL4){
		float_array = b->data;
		col = b->local_dims[0];
		n = b->nelements_local;
		row = n/col;
		data = Data_create(row,col);
		for(i=0;i<n;i++){
			Data_set(data,i/col,i%col,float_array[i]);
		}
		Data_output(data,fileout,p_float);
		Data_delete(data);
	}
    b->done_data = 1;

    return 0;
}
