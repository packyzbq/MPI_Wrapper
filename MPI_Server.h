//
// Created by zhaobq on 2016/11/28.
//

#ifndef MPI_WRAPPER_MPI_SERVER_H
#define MPI_WRAPPER_MPI_SERVER_H

#include "MPI_Connect_Wrapper.h"


using namespace std;

class MPI_Server : public MPI_Connect_Wrapper{

public:
    MPI_Server(Msg_handlerABC mh1, char *svc_name)
            : MPI_Connect_Wrapper(mh1), svc_name_(svc_name){
        //TODO initialize server
    };

    virtual bool new_msg_come(ARGS * args);
    virtual void send(const void *buf, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);




private:
    char* svc_name_;
    char* hostname;
    map<int,MPI_Comm> client_comm_list;             //<wid : comm>
};


#endif //MPI_WRAPPER_MPI_SERVER_H
