//
// Created by zhaobq on 2016/12/1.
//

#ifndef MPI_WRAPPER_CLIENT_H
#define MPI_WRAPPER_CLIENT_H

#include "MPI_Connect_Wrapper.h"

class MPI_Client : public MPI_Connect_Wrapper{
public:
    MPI_Client(Msg_handlerABC *mh, char* svc_name, char* port);

    virtual void run();
    void initial();
    void stop();
    virtual bool new_msg_come(ARGS * args);
    virtual void* recv_thread(void* ptr);
    virtual void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    virtual void recv_handle(int tag, void* buf);
    //TODO void recv_bcast();

private:
    char* svc_name_;
    char portname[MPI_MAX_PORT_NAME];

    int wid=0;
    int dest_rank;

    MPI_Comm sc_comm_;
    //MPI_Comm bcast_comm_;

    //pthread_t recv_pid, send_pid;



};


#endif //MPI_WRAPPER_CLIENT_H
