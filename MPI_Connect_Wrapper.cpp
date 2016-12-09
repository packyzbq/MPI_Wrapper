//
// Created by zhaobq on 2016/11/28.
//
#include <cstdlib>
#include <iostream>
#include "MPI_Connect_Wrapper.h"

#define DEBUG
using namespace std;

void* MPI_Connect_Wrapper::recv_thread(void *ptr) {
    int msgsz;
    void* rb;

    ((MPI_Connect_Wrapper*)ptr)->recv_flag = false;

    pthread_t pid;
    pid = pthread_self();
    ARGS* args;
    MPI_Status stat,recv_st;

    MPI_Comm_rank(MPI_COMM_WORLD, &(((MPI_Connect_Wrapper*)ptr)->myrank));
    MPI_Comm_size(MPI_COMM_WORLD, &(((MPI_Connect_Wrapper*)ptr)->w_size));

#ifdef DEBUG
    cout<<"<thread_recv>: Proc: "<< ((MPI_Connect_Wrapper*)ptr)->myrank << ", Pid: " << pid << ", receive thread start...  "<<endl;
#endif
    // TODO add exception handler -> OR add return code
    while(!((MPI_Connect_Wrapper*)ptr)->recv_flag){
        if(((MPI_Connect_Wrapper*)ptr)->new_msg_come(args)){
#ifdef DEBUG
            cout <<"<thread_recv>: detect a new message" << endl;
#endif
            MPI_Get_count(&(args->arg_stat), args->datatype, &msgsz);
            switch (args->datatype)
            {
                case MPI_INT:
                    rb = new int[msgsz];
                    break;
                case MPI_CHAR:
                    rb = new char[msgsz];
                    break;
                default:
                    break;
            }
            MPI_Recv(rb, msgsz, args->datatype, args->arg_stat.MPI_SOURCE, args->arg_stat.MPI_TAG, args->newcomm, &recv_st);
#ifdef DEBUG
            cout << "<thread_recv>: receive a message <-- <" << rb << ">" << endl;
#endif
            MPI_Barrier(args->newcomm);
#ifdef DEBUG
            cout << "<thread_recv>: handled by recv_handler" << endl;
#endif
            ((MPI_Connect_Wrapper*)ptr)->recv_handle(args->arg_stat.MPI_TAG, rb, args->newcomm);

        }
    }

    return 0;
}

void* MPI_Connect_Wrapper::send_thread(void* ptr) {
    //TODO add return code
    ((MPI_Connect_Wrapper*)ptr)->send_flag = false;
    //发送函数，在平时挂起，使用 send唤醒 来发送信息
    pthread_t pid = pthread_self();
#ifdef DEBUG
    cout<< "Proc: "<< "Send thread start..., pid = " << pid << endl;
#endif
    pthread_mutex_lock(&(((MPI_Connect_Wrapper*)ptr)->send_mtx));
    while(!((MPI_Connect_Wrapper*)ptr)->send_flag){

        pthread_cond_wait(&(((MPI_Connect_Wrapper*)ptr)->send_thread_cond), &(((MPI_Connect_Wrapper*)ptr)->send_mtx));
#ifdef DEBUG
        cout << "Send restart..." << endl;
#endif
        MPI_Send(((MPI_Connect_Wrapper*)ptr)->sendmsg.buf_, ((MPI_Connect_Wrapper*)ptr)->sendmsg.msgsize_,
                 ((MPI_Connect_Wrapper*)ptr)->sendmsg.datatype_, ((MPI_Connect_Wrapper*)ptr)->sendmsg.dest_,
                 ((MPI_Connect_Wrapper*)ptr)->sendmsg.tag_, ((MPI_Connect_Wrapper*)ptr)->sendmsg.comm_);
#ifdef DEBUG
        cout << "Send finish..." << endl;
#endif
    }
    pthread_mutex_unlock(&(((MPI_Connect_Wrapper*)ptr)->send_mtx));

    return 0;

}

void MPI_Connect_Wrapper::send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm) {
    //唤醒send_thread， 发送信息

    pthread_mutex_lock(&send_mtx);
    sendmsg.init(buf, msgsize, dest, datatype, tag, comm);
    pthread_cond_signal(&send_thread_cond);
    pthread_mutex_unlock(&send_mtx);

}

//探测是否有新的点对点信息到来
//@return bool args
bool MPI_Connect_Wrapper::new_msg_come(ARGS * args) {
    //在子类实现，扫描子类的comm列表等
}


//通过tags 解析出数据类型
// TODO 添加新的tags时，可能需要改变程序
MPI_Datatype MPI_Connect_Wrapper::analyz_type(int tags) {
    if (tags >= 10)
        return MPI_PACKED;
    else if(tags == 0 || tags % 2 == 0)
        return MPI_INT;
    else
        return MPI_CHAR;
}

void MPI_Connect_Wrapper::set_recv_stop() {
    recv_flag = true;
}

void MPI_Connect_Wrapper::set_send_stop() {
    send_flag = true;
}