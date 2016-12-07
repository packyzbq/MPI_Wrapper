//
// Created by zhaobq on 2016/12/1.
//

#include "MPI_Client.h"
#include <iostream>

using namespace std;

MPI_Client::MPI_Client(const Msg_handlerABC &mh, char * svc_name): MPI_Connect_Wrapper(mh), svc_name_(svc_name){

};

void MPI_Client::initial() {
    cout << "[Client]: client initail..." << endl;

    send_flag = false;
    recv_flag = false;

    int provid;
    MPI_Init_thread(0,0, MPI_THREAD_MULTIPLE, &provid);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    MPI_Lookup_name(svc_name_, MPI_INFO_NULL, portname);

    MPI_Comm_connect(portname, MPI_INFO_NULL,0, MPI_COMM_SELF, &sc_comm_);
    cout << "[Clent]: client connect to server on port " << portname << endl;

    //pthread_create(&recv_t, NULL, MPI_Client::recv_thread, this);
    recv_thread(this);

    pthread_create(&send_t, NULL, MPI_Connect_Wrapper::send_thread, this);

}

void MPI_Client::stop() {
    cout << "[Client]: stop Client..." << endl;
    recv_flag = true;
    send_flag = true;
    pthread_cancel(send_t);
    MPI_Comm_disconnect(&sc_comm_);
    cout << "[Client]: disconnected..." << endl;
}

void* MPI_Client::recv_thread(void* ptr) {
    //TODO add return code
    cout << "[Client] on host: "<< hostname << ", receive thread start..." << endl;
    pthread_create(&recv_t, NULL, MPI_Connect_Wrapper::recv_thread, ptr);
    cout << "[Client] on host: "<< hostname << ", receive thread stop..." << endl;

    return 0;
}

bool MPI_Client::new_msg_come(ARGS *args) {
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

void MPI_Client::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
    cout << "[Client]: send message..." << endl;
    MPI_Connect_Wrapper::send(buf, msgsize, dest, datatype, tag, comm);
    cout << "[Client]: send finish, send thread sleep..." << endl;
}

void MPI_Client::recv_handle(int tag, void *buf) {
    // TODO add conditions
    if(tag == MPI_Tags::MPI_BCAST_REQ){

    }
    else
        msg_handler->recv_commit(tag, buf);
}

void MPI_Client::run() {
    // TODO main thread for client
    int ret;
    initial();

    ret = pthread_join(send_t, NULL);
    cout <<"[Client]: send thread stop, exit code=" << ret << endl;
    pthread_join(recv_t, NULL);
    cout <<"[Client]: recv thread stop, exit code=" << ret << endl;

    MPI_Finalize();
}