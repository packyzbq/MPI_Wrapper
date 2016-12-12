//
// Created by zhaobq on 2016/12/5.
//

#include "MPI_Server.h"

class test_msg_handler : public Msg_handlerABC{
public:
    virtual void recv_commit(int mpi_tags ,void* buffer){
        cout << "I get a msg, tag=" << mpi_tags <<endl;
    };
};

int main(){
    test_msg_handler* tmh;
    tmh = new test_msg_handler();
    MPI_Server server(tmh, "TEST");
    server.initial();

    server.stop();
}