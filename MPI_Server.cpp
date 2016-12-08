//
// Created by zhaobq on 2016/11/28.
//

#include "MPI_Server.h"



void MPI_Server::initial() {
    // init MPI env; open port+publish service ; start 3 main threads
    int provided;
    MPI_Init_thread(0,0,MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(hostname, &msglen);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    cout << "[Server]: Host: " << hostname << ",Proc: "<< myrank << ", Server initialize..." << endl;
    merr = MPI_Open_port(MPI_INFO_NULL, port);

    cout << "[Server]: Host: " << hostname << ",Proc: "<< myrank << ",Server opening port on <" << port <<">" << endl;

    merr = MPI_Publish_name(svc_name_, MPI_INFO_NULL, port);
    if(merr){
        errs++;
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server]: Error in publish_name :" << errmsg<<endl;
    }
    cout << "[Server]: publish service <" << svc_name_ << ">" << endl;
    MPI_Barrier(MPI_COMM_WORLD);

    //start recv thread
    cout << "[Server]: receive thread start..." << endl;
    pthread_create(&recv_t ,NULL, MPI_Connect_Wrapper::recv_thread, this);
    //recv_thread(this);

    //start send thread
    cout << "[Server]: send thread start..." << endl;
    pthread_create(&send_t, NULL, MPI_Connect_Wrapper::send_thread, this);

    //start accept thread
    cout << "[Server]: accept thread start..." << endl;
    pthread_create(&accept_thread, NULL, MPI_Server::accept_conn_thread, this);
}

void MPI_Server::stop() {
    cout << "[Server]: Ready to stop..." << endl;
    cout << "[Server]: Unpublish service name..." << endl;
    merr = MPI_Unpublish_name(svc_name_, MPI_INFO_NULL, port);
    if(merr){
        errs++;
        MPI_Error_string(merr, errmsg, &msglen);
        cout << "[Server]: Unpublish service name error: "<< errmsg << endl;
    }

    //stop threads
    set_accept_t_stop();
    set_recv_stop();
    set_send_stop();

    int ret;
    ret = pthread_cancel(accept_thread);
    cout <<"[Server]: cancel accept thread, exit code=" << ret << endl;

    ret = pthread_cancel(send_t);
    cout <<"[Server]: cancel accept thread, exit code=" << ret << endl;

    map<int ,MPI_Comm>::iterator iter;
    for(iter = client_comm_list.begin(); iter != client_comm_list.end(); iter++){
        MPI_Comm_disconnect(&(iter->second));
    }
}

bool MPI_Server::new_msg_come(ARGS *args) {

    MPI_Status stat;
    int flag = 0;
    map<int, MPI_Comm > ::iterator iter;

    for(iter = client_comm_list.begin(); iter != client_comm_list.end(); iter++){
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, iter->second , &flag, &stat);
        if(flag) {
            args = new ARGS();
            args->newcomm = iter->second;
            args->arg_stat = stat;
            args->datatype = analyz_type(stat.MPI_TAG);
            args->source_rank = stat.MPI_SOURCE;
            flag = 0;
            return true;
        }
    }
    return false;
}

void* MPI_Server::accept_conn_thread(void* ptr) {
    //TODO add return code
    pthread_t mypid = pthread_self();
    cout << "[Server] host: "<< ((MPI_Server*)ptr)->hostname <<", accept connection thread start..." << endl;
    while(!((MPI_Server*)ptr)->accept_conn_flag) {
        MPI_Comm newcomm;
        ((MPI_Server*)ptr)->merr = MPI_Comm_accept(((MPI_Server*)ptr)->port, MPI_INFO_NULL, 0, MPI_COMM_SELF, &newcomm);
        if(((MPI_Server*)ptr)->merr){
            MPI_Error_string(((MPI_Server*)ptr)->merr, ((MPI_Server*)ptr)->errmsg, &(((MPI_Server*)ptr)->msglen));
            cout << "[Server]: accept client error, msg: " << ((MPI_Server*)ptr)->errmsg << endl;
        }
        ((MPI_Server*)ptr)->client_comm_list.insert(pair<MPI_Comm, int>(newcomm, 0));

        //TODO receive worker MPI_REGISTEY tags and add to master, in recv_thread() function or ABC recv_commit() function
        cout << "[Server]:Host: " << ((MPI_Server*)ptr)->hostname << ",Proc: "<< ((MPI_Server*)ptr)->myrank << ", receive new connection..." << endl;
        //TODO add to bcast_comm/group

    }
    cout << "[Server] host: "<< ((MPI_Server*)ptr)->hostname << ", accept connection thread stop..." << endl;
    return 0;
}

void* MPI_Server::recv_thread(void* ptr) {
    cout << "[Server]: receive thread start..." << endl;

    pthread_create(&recv_t ,NULL, MPI_Connect_Wrapper::recv_thread, ptr);

    cout << "[Server]: receive thread stop..." << endl;

    return 0;
}

void MPI_Server::recv_handle(int tag, void* buf, MPI_Comm comm) {
    //TODO set different conditions

    switch(tag){
        case MPI_Tags::MPI_REGISTEY:
            client_comm_list[comm] = (*(int*)buf);
            break;
        case MPI_Tags::MPI_BCAST_ACK:
            break;
        default:
            msg_handler->recv_commit(tag, buf);
    }
}

void MPI_Server::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
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
    msg_handler->recv_commit(MPI_Tags::MPI_DISCONNECT, &w_uuid);
}

void MPI_Server::bcast(void *buf, int msgsz, MPI_Datatype datatype, int tags) {
// solve the bcast_comm and root sync problem --> no solutions yet
// use send loop to bcast,
// **warning**: can't guarantee transfer safety. msg may be discarded when msg is too big.
// use send loop to bcast
    int myrank;
    map<int, MPI_Comm>::iterator iter;
    for(iter = client_comm_list.begin(); iter != client_comm_list.end(); iter++){
        MPI_Comm_rank(iter->second, &myrank);
        MPI_Send(buf, msgsz, datatype, 1-myrank, tags, iter->second);
    }
    cout << "[Server]: bcast <" << tags << ">finish" << endl;
}

void MPI_Server::run() {
    // TODO Server work flow, add exception handle
    int ret;

    initial();

    //TODO add some work



    ret = pthread_join(accept_thread, NULL);
    cout << "[Server]: accept thread stop, exit code=" << ret << endl;
    ret = pthread_join(recv_t, NULL);
    cout << "[Server]: recv_thread stop, exit code=" << ret << endl;
    ret = pthread_join(send_t, NULL);
    cout << "[Server]: send_thread stop, exit code=" << ret << endl;

    MPI_Finalize();

}

bool MPI_Server::gen_client() {
    //TODO MPI_COMM_SPAWN -> client; add new client to bcast_comm/group
}

int MPI_Server::comm_list_size() {
    return client_comm_list.size();
}