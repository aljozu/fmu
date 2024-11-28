# Example program
example: example.c
	mpicc example.c -o example -g

CLEANFILES = example

# We must set these #defines so that the inner OMPI MPI prototype
# header files do the Right Thing.
AM_CPPFLAGS = -DOMPI_PROFILE_LAYER=0 -DOMPI_COMPILING_FORTRAN_WRAPPERS=1

# Convenience libtool library that will be slurped up into libmpi.la.
noinst_LTLIBRARIES = libmpiext_fmu_c.la

# This is where the top-level header file (that is included in
# <mpi-ext.h>) must be installed.
ompidir = $(ompiincludedir)/mpiext/

# This is the header file that is installed.
ompi_HEADERS = mpiext_fmu_c.h

# Sources for the convenience libtool library.  Other than the one
# header file, all source files in the extension have no file naming
# conventions.
libmpiext_fmu_c_la_SOURCES = \
        $(ompi_HEADERS) \
        mpiext_fmu_write.c \
        mpiext_fmu_read.c \
        mpiext_fmu_module.c \
        mpiext_fmu_alltoall.c
libmpiext_fmu_c_la_LDFLAGS = -module -avoid-version

EXTRA_DIST = example.c
