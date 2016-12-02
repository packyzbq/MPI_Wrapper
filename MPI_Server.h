//
// Created by zhaobq on 2016/11/28.
//

#ifndef MPI_WRAPPER_MPI_SERVER_H
#define MPI_WRAPPER_MPI_SERVER_H

#include "MPI_Connect_Wrapper.h"
#include <iostream>

using namespace std;


class MPI_Server : public MPI_Connect_Wrapper{

public:
    MPI_Server(Msg_handlerABC mh, char *svc_name) : MPI_Connect_Wrapper(mh), svc_name_(svc_name) {};

    ~MPI_Server(){};

    void run();                 //启动server各种线程
    void initial();
    virtual bool new_msg_come(ARGS * args);
    virtual void* recv_thread(void* ptr);
    virtual void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    void* accept_conn_thread(void* ptr);
    bool gen_client();
    bool remove_client(int w_uuid);
    void bcast(void *buf, int msgsz, MPI_Datatype datatype, MPI_Comm comm);
    void set_accept_t_stop(){accept_conn_flag = true;};




private:
    char svc_name_[100];
    map<int,MPI_Comm> client_comm_list;             //<wid : comm>
    char port[MPI_MAX_PORT_NAME];
    bool accept_conn_flag = false;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Group bcast_group;

    pthread_t accept_thread;
};


#endif //MPI_WRAPPER_MPI_SERVER_H
