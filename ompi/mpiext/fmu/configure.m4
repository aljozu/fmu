AC_DEFUN([OMPI_MPIEXT_fmu_CONFIG], [
    AC_CONFIG_FILES([ompi/mpiext/fmu/Makefile])
    AC_CONFIG_FILES([ompi/mpiext/fmu/c/Makefile])

    # This example can always build, so we just execute $1 if it was
    # requested.
    AS_IF([test "$ENABLE_fmu" = "1" || \
           test "$ENABLE_EXT_ALL" = "1"],
          [$1],
          [$2])
])

AC_DEFUN([OMPI_MPIEXT_fmu_NEED_INIT], [1])