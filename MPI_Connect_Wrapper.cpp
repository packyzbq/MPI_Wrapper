//
// Created by zhaobq on 2016/11/28.
//
#include <cstdlib>
#include <iostream>
#include "MPI_Connect_Wrapper.h"

using namespace std;

virtual void MPI_Connect_Wrapper::recv_thread() {
    int msgsz;
    void* rb;

    pthread_t pid;
    pid = pthread_self();
    ARGS* args;
    MPI_Status stat,recv_st;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &w_size);

#ifdef DEBUG
    cout<<"Proc: "<< rank << ", Pid: " << pid << ", receive thread start...  "<<endl
#endif

    while(!recv_flag){
        if(new_msg_come(args)){
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
            MPI_Barrier(args->newcomm);
            msg_handler.recv_commit(rb);
        }
    }
}

void MPI_Connect_Wrapper::send_thread() {
    //发送函数，在平时挂起，使用 send唤醒 来发送信息
    pthread_t pid = pthread_self();
#ifdef DEBUG
    cout<< "Proc: "<< "Send thread start..., pid = " << pid << endl;
#endif
    pthread_mutex_lock(&send_mtx);
    while(!send_flag){

        pthread_cond_wait(&send_thread_cond, &send_mtx);
#ifdef DEBUG
        cout<< ""
#endif
        MPI_Send(sendmsg.buf_, sendmsg.msgsize_, sendmsg.datatype_, sendmsg.dest_, sendmsg.tag_, sendmsg.comm_);
    }
    pthread_mutex_unlock(&send_mtx);

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
virtual bool MPI_Connect_Wrapper::new_msg_come(ARGS * args) {
    //在子类实现，扫描子类的comm列表等
}


//通过tags 解析出数据类型
// TODO 添加新的tags时，可能需要改变程序
virtual MPI_Datatype MPI_Connect_Wrapper::analyz_type(int tags) {
    if(tags == 0 || tags % 2 == 0)
        return MPI_INT;
    else
        return MPI_CHAR;
}

virtual void MPI_Connect_Wrapper::set_recv_stop() {
    recv_flag = false;
}

void MPI_Connect_Wrapper::set_send_stop() {
    send_flag = false;
}