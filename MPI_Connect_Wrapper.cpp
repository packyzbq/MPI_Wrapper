//
// Created by zhaobq on 2016/11/28.
//
#include <cstdlib>
#include <iostream>
#include "MPI_Connect_Wrapper.h"

using namespace std;

virtual void* MPI_Connect_Wrapper::recv_thread() {
    int rank, size, newrk, msgsz;
    auto rb[1024];
    char *rp;
    char outstr[1024];
    int* rtn;
    ARGS* args;

    MPI_Status stat,status;

    cout << "recv_thread start..." <<endl;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while(!flag){
        if(new_msg_come(args)){
            MPI_Get_count(&(args->arg_stat), args->datatype, &msgsz);
            if()
        }
    }

}

virtual bool MPI_Connect_Wrapper::new_msg_come(ARGS * args) {

}


//通过tags 解析出数据类型
//TODO 添加新的tags时，可能需要改变程序
virtual MPI_Datatype MPI_Connect_Wrapper::analyz_type(MPI_Tags t) {
    if(t == 0 || t % 2 == 0)
        return MPI_INT;
    else
        return MPI_CHAR;
}
