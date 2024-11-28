#include "ompi_config.h"
#include "ompi/mpiext/mpiext.h"
#include "ompi/mpi/c/bindings.h"
#include "opal/class/opal_hash_table.h"
#include <stdio.h>

#include "ompi/mpiext/fmu/c/mpiext_fmu_c.h"




static int fmu_init(void)
{
    //opal_hash_table_set_value_uint32( &FMU_HASH, 1, 2 );
    //printf("example mpiext init\n");
    return OMPI_SUCCESS;
}

static int fmu_fini(void)
{

    //printf("example mpiext fini\n");

    return OMPI_SUCCESS;
}

/*
 * Similar to Open MPI components, a well-known struct provides
 * function pointers to the extension's init/fini hooks.  The struct
 * must be a global symbol of the form ompi_mpiext_<ext_name> and be
 * of type ompi_mpiext_component_t.
 */
ompi_mpiext_component_t ompi_mpiext_fmu = {
    fmu_init,
    fmu_fini
};