//
// Created by zhaobq on 2016/11/28.
//

#ifndef MPI_WRAPPER_MPI_UTIL_H
#define MPI_WRAPPER_MPI_UTIL_H

//传输用的tags， 每个tags中包括了传输的数据类型
class MPI_Tags{
    static int MPI_RECV_INT         = 0;
    static int MPI_RECV_CHAR        = 1;
    static int MPI_SEND_INT         = 2;
    static int MPI_SEND_CHAR        = 3;
    static int MPI_BCAST_INT        = 4;
    static int MPI_BCAST_CHAR       = 5;
};

class MPI_Data_type{

};


#endif //MPI_WRAPPER_MPI_UTIL_H
