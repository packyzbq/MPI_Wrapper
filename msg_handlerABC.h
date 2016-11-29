//
// Created by zhaobq on 2016/11/28.
//

#ifndef MPI_WRAPPER_MSG_HANDLERABC_H
#define MPI_WRAPPER_MSG_HANDLERABC_H

#endif //MPI_WRAPPER_MSG_HANDLERABC_H

class Msg_handlerABC{
public:
    virtual void recv_commit(int mpi_tags ,void* buffer)=0;

};