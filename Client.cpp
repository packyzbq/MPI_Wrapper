//
// Created by zhaobq on 2016/12/1.
//

#include "Client.h"
#include <iostream>

using namespace std;

Client::Client(const Msg_handlerABC &mh, char * svc_name): MPI_Connect_Wrapper(mh), svc_name_(svc_name){

};

void* Client::recv_thread(void* ptr) {
    //TODO add return code
    cout << "[Client] on host: "<< hostname << ", receive thread start..." << endl;
    MPI_Connect_Wrapper::recv_thread(ptr);
    cout << "[Client] on host: "<< hostname << ", receive thread stop..." << endl;

    return 0;
}

bool Client::new_msg_come(ARGS *args) {
    MPI_Status stat;
    int flag;
    MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG, sc_comm_, &flag, &stat);
    if(flag){
        args->arg_stat = stat;
        args->datatype = analyz_type(stat.MPI_TAG);
        args->source_rank = stat.MPI_SOURCE;
        args->newcomm = sc_comm_;

        return true;
    }
    else
        return false;

}

void Client::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
    cout << "[Client]: send message..." << endl;
    MPI_Connect_Wrapper::send(buf, msgsize, dest, datatype, tag, comm);
    cout << "[Client]: send finish, send thread sleep..." << endl;
}

void Client::recv_handle(int tag, void *buf) {
    // TODO add conditions
    if(tag == MPI_Tags::MPI_BCAST_REQ){

    }
    else
        msg_handler.recv_commit(tag, buf);
}

void Client::run() {
    // TODO main thread for client
}