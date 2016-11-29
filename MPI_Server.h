//
// Created by zhaobq on 2016/11/28.
//

#ifndef MPI_WRAPPER_MPI_SERVER_H
#define MPI_WRAPPER_MPI_SERVER_H

#include "MPI_Connect_Wrapper.h"

using namespace std;

class MPI_Server : public MPI_Connect_Wrapper{
public:

private:
    char* svc_name;
    map<int,MPI_Comm> client_comm_list;
};


#endif //MPI_WRAPPER_MPI_SERVER_H
