//
// Created by zhaobq on 2016/12/1.
//

#ifndef MPI_WRAPPER_CLIENT_H
#define MPI_WRAPPER_CLIENT_H

#include "MPI_Connect_Wrapper.h"

class Client : public MPI_Connect_Wrapper{
public:
    Client(const Msg_handlerABC &mh, char* svc_name);

    virtual void run();
    virtual bool new_msg_come(ARGS * args);
    virtual void* recv_thread(void* ptr);
    virtual void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    void recv_bcast();

private:
    char* svc_name_;
    MPI_Comm sc_comm_;
    MPI_Comm bcast_comm_;



};


#endif //MPI_WRAPPER_CLIENT_H
