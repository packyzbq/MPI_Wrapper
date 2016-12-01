//
// Created by zhaobq on 2016/11/28.
//

#include "MPI_Server.h"


MPI_Server::~MPI_Server(){
    //TODO  disconnect; unpublish service name; finalize; set read_thread、send_thread、accept_thread stop
}

void MPI_Server::initial() {
    // TODO initialize mpi env with multi thread ; start send/recv/accept thread ; set err_handler

    MPI_Get_processor_name(hostname, &msglen);
    cout << "Host: " << hostname << ",Proc: "<< myrank << ", Server initialize..." << endl;
    merr = MPI_Open_port(MPI_INFO_NULL, port);

    cout << "Host: " << hostname << ",Proc: "<< myrank << ",Server opening port on <" << port <<">" << endl;

    merr = MPI_Publish_name(svc_name_, MPI_INFO_NULL, port);
    if(merr){
        errs++;
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "Error in publish_name :" << errmsg<<endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

virtual bool MPI_Server::new_msg_come(ARGS *args) {

    MPI_Status stat;
    int flag;
    map<int, MPI_Comm > ::iterator iter;

    for(iter = client_comm_list.begin(); iter != client_comm_list.end(); iter++){
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, iter->second , &flag,&stat);
        if(flag)
        {
            args->newcomm = iter->second;
            args->arg_stat = stat;
            args->datatype = analyz_type(stat.MPI_TAG);
            args->source_rank = stat.MPI_SOURCE;
        }
        return true;
    }
    return false;
}

void MPI_Server::accept_conn_thread() {
    pthread_t mypid = pthread_self();
    cout << "[Server] host: "<< hostname <<", accept connection thread start..." << endl;
    while(!accept_conn_flag) {
        MPI_Comm newcomm;
        merr = MPI_Comm_accept(port, MPI_INFO_NULL, 0, MPI_COMM_SELF, &newcomm);
        client_comm_list.insert(pair<int, MPI_Comm>(0, newcomm));

        //TODO receive worker MPI_REGISTEY tags and add to master, in recv_thread() function or ABC recv_commit() function
        cout << "Host: " << hostname << ",Proc: "<< myrank << ", receive new connection...";
        //TODO add to bcast_comm/group

    }
    cout << "[Server] host: "<< hostname << ", accept connection thread stop..." << endl;
}

virtual void MPI_Server::recv_thread() {
    cout << "[Server]: receive thread start..." << endl;

    MPI_Connect_Wrapper::recv_thread();

    cout << "[Server]: receive thread stop..." << endl;
}

virtual void MPI_Server::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
    cout << "[Server]: send message..." << endl;
    MPI_Connect_Wrapper::send(buf, msgsize, dest, datatype, tag, comm);
    cout << "[Server]: send finish, send thread sleep..." << endl;
}


bool MPI_Server::remove_client(int w_uuid) {
    int my_newrank;
    MPI_Comm_rank(client_comm_list[w_uuid],&my_newrank);
    send(NULL, 0, 2-my_newrank, MPI_INT, MPI_Tags::MPI_DISCONNECT, client_comm_list[w_uuid]);
    MPI_Comm_disconnect(&client_comm_list[w_uuid]);
    cout <<"[Server]: remove worker "<< w_uuid << endl;

    client_comm_list.erase(w_uuid);
    //告知master 已经移除worker
    msg_handler.recv_commit(MPI_Tags::MPI_DISCONNECT, &w_uuid);
}

void MPI_Server::bcast(void *buf, int msgsz, MPI_Datatype datatype, MPI_Comm comm) {
//TODO solve the bcast_comm and root sync problem

}

void MPI_Server::run() {
    // TODO main thread for server
}

bool MPI_Server::gen_client() {
    //TODO MPI_COMM_SPAWN -> client; add new client to bcast_comm/group
}