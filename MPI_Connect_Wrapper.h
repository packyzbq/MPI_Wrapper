//
// Created by zhaobq on 2016/11/28.
//
#ifndef MPI_WRAPPER_MPI_CONNECT_WRAPPER_H
#define MPI_WRAPPER_MPI_CONNECT_WRAPPER_H

#include "msg_handlerABC.h"
#include "mpi.h"
#include "pthread.h"
#include "MPI_util.h"
#include <map>

struct SendMSG{ // 用于唤醒send进程后，send进程发送的内容
    void* buf_;
    int msgsize_;
    int dest_;
    int tag_;
    MPI_Datatype datatype_;
    MPI_Comm comm_;

    void init(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm){
        buf_ = buf;
        msgsize_ = msgsize;
        dest_ = dest;
        datatype_ = datatype;
        tag_ = tag;
        comm_ = comm;
    }
};

struct ARGS{    //用于 new_msg_come 向 recv传递参数
    MPI_Comm newcomm;
    int source_rank;
    MPI_Datatype datatype;
    MPI_Status arg_stat;
};


class MPI_Connect_Wrapper {
protected:
    Msg_handlerABC* msg_handler;
    pthread_cond_t recv_thread_cond, send_thread_cond;      //  用于挂起读/写线程时
    pthread_mutex_t recv_mtx, send_mtx;                     //  同上
    pthread_t recv_t, send_t;

    int myrank;
    int w_size;
    int errs = 0;
    int merr;
    int msglen;
    char errmsg[MPI_MAX_ERROR_STRING];
    char hostname[MPI_MAX_PROCESSOR_NAME];


public:
    MPI_Connect_Wrapper(Msg_handlerABC* mh):msg_handler(mh){
        recv_thread_cond = PTHREAD_COND_INITIALIZER;
        send_thread_cond = PTHREAD_COND_INITIALIZER;
        recv_mtx = PTHREAD_MUTEX_INITIALIZER;
        send_mtx = PTHREAD_MUTEX_INITIALIZER;

        MPI_Init(0,0);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        MPI_Comm_size(MPI_COMM_WORLD, &w_size);
        MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    };

    virtual ~MPI_Connect_Wrapper(){
        MPI_Finalize();
    };

    virtual void run(){};

    static void* recv_thread(void* ptr);
    static void* send_thread(void* ptr);

    virtual bool new_msg_come(ARGS * args);
    virtual MPI_Datatype analyz_type(int tags);

    virtual void send(void *buf, int msgsize, int dest, MPI_Datatype datatype, int tag, MPI_Comm comm);
    //virtual void err_handler();
    void set_recv_stop();
    void set_send_stop();

    virtual void recv_handle(int tag, void* buf, MPI_Comm comm){}; //

    bool recv_flag = false;
    bool send_flag = false;

    SendMSG sendmsg;


};


#endif //MPI_WRAPPER_MPI_CONNECT_WRAPPER_H
