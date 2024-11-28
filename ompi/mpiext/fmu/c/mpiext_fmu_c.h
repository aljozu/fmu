OMPI_DECLSPEC int MPI_Fmu_Write(void *data, int count, MPI_Datatype datatype, int tag, int fmu_tag, MPI_Comm comm);
OMPI_DECLSPEC int MPI_Fmu_Read(void *data, int count, int dest, MPI_Datatype datatype, int tag, int fmu_tag, MPI_Comm comm);
OMPI_DECLSPEC int MPI_Fmu_Alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

