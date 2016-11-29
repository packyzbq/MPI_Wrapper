//
// Created by zhaobq on 2016/11/28.
//
#ifndef MPI_WRAPPER_MPI_CONNECT_WRAPPER_H
#define MPI_WRAPPER_MPI_CONNECT_WRAPPER_H

#include "msg_handlerABC.h"
#include "mpi.h"
#include "pthread.h"
#include "MPI_util.h"
#include <map>

class MPI_Connect_Wrapper {
private:
    Msg_handlerABC msg_handler;


public:
    virtual void* recv_thread();
    virtual bool new_msg_come(ARGS * args);
    virtual MPI_Datatype analyz_type(MPI_Tags t);
    virtual void send_thread();
    virtual void send(const void *buf, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    virtual void err_handler();
    void set_recv_stop(bool flag);

    bool flag = false;
    struct ARGS{    //用于 new_msg_come 向 recv传递参数
        MPI_Comm newcomm;
        int myrank;
        MPI_Datatype datatype;
        MPI_Status arg_stat;
    };
};


#endif //MPI_WRAPPER_MPI_CONNECT_WRAPPER_H
