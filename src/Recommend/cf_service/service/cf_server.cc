/* Author: Akshitha Sriraman
   Ph.D. Candidate at the University of Michigan - Ann Arbor*/

#include <stdlib.h>
#include <iostream>
#include <memory>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <thread>
#include <grpc++/grpc++.h>
#include "cf_service/service/helper_files/server_helper.h"
#include "cf_service/service/helper_files/timing.h"
#include "cf_service/service/helper_files/utils.h"
#include "../../protoc_files/cf.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using collaborative_filtering::UtilRequest;
using collaborative_filtering::UtilResponse;
using collaborative_filtering::CFRequest;
using collaborative_filtering::TimingDataInMicro;
using collaborative_filtering::CFResponse;
using collaborative_filtering::CFService;

using namespace mlpack;
using namespace mlpack::cf;

/* Make dataset a global, so that the dataset can be loaded
   even before the server starts running. */
Matrix dataset;

std::string ip_port = "";
std::mutex cf_mutex;
unsigned int cf_parallelism = 0;

int num_cores = 0, cf_server_number = 0, num_cf_servers = 0;
CF* cf_matrix;

void ProcessRequest(CFRequest &request,
        CFResponse* reply)
{
    /* If the index server is asking for util info,
       it means the time period has expired, so 
       the cf must read /proc/stat to provide user, system, io, and idle times.*/
    if(request.util_request().util_request())
    {
        uint64_t user_time = 0, system_time = 0, io_time = 0, idle_time = 0;
        GetCpuTimes(&user_time,
                &system_time,
                &io_time,
                &idle_time);
        reply->mutable_util_response()->set_user_time(user_time);
        reply->mutable_util_response()->set_system_time(system_time);
        reply->mutable_util_response()->set_io_time(io_time);
        reply->mutable_util_response()->set_idle_time(idle_time);
        reply->mutable_util_response()->set_util_present(true);
    }

    /* Simply copy request id into the reply - this was just a 
       piggyback message.*/
    reply->set_request_id(request.request_id());

    /* Get the current idle time and total time
       so as to calculate the CPU util when the cf is done.*/
    size_t idle_time_initial = 0, total_time_initial = 0, idle_time_final = 0, total_time_final = 0;
    //GetCpuTimes(&idle_time_initial, &total_time_initial);

    // Unpack received queries and point IDs
    uint32_t cf_server_id, shard_size;
    uint64_t start_time, end_time;
    start_time = GetTimeInMicro();
    Request user_item;
    UnpackCFServiceRequest(request,
            &user_item);

    end_time = GetTimeInMicro();
    reply->mutable_timing_data_in_micro()->set_unpack_cf_srv_req_time_in_micro((end_time - start_time));
    /* Next piggy back message - sent the received query back to the 
       index server. Helps to merge async responses.*/
    // Remove duplicate point IDs.
    //RemoveDuplicatePointIDs(point_ids_vec);

    // Dataset dimension must be equal to queries dimension.
#if 0
    dataset.ValidateDimensions(dataset.GetPointDimension(),
            queries.GetPointDimension());
#endif

    // Calculate the top K distances for all queries.
    start_time = GetTimeInMicro();
    float rating = 0.0;
    CalculateRating(user_item,
            cf_matrix,
            &rating);
    end_time = GetTimeInMicro();
    reply->mutable_timing_data_in_micro()->set_calculate_cf_srv_time_in_micro((end_time - start_time));
    // Difei: Added dispersion between each response
    start_time = GetTimeInMicro();
    end_time = GetTimeInMicro();
    int rand_num = rand() % 1000000;
    while (end_time - start_time < rand_num)
    {
    	end_time = GetTimeInMicro();
    }
    // Convert K-NN into form suitable for GRPC.
    start_time = GetTimeInMicro();
    PackCFServiceResponse(rating, 
            reply);
    end_time = GetTimeInMicro();
    reply->mutable_timing_data_in_micro()->set_pack_cf_srv_resp_time_in_micro((end_time - start_time));
    //GetCpuTimes(&idle_time_final, &total_time_final);
    const float idle_time_delta = idle_time_final - idle_time_initial;
    const float total_time_delta = total_time_final - total_time_initial;
    const float cpu_util = (100.0 * (1.0 - (idle_time_delta/total_time_delta)));
    reply->mutable_timing_data_in_micro()->set_cpu_util(cpu_util);
}

// Logic and data behind the server's behavior.
class ServiceImpl final : public CFService::Service 
{
    // There is no shutdown handling in this code
    Status CF(ServerContext* context, const CFRequest* request,
            CFResponse* reply) override 
    {
        // Difei
        if(request->util_request().util_request())
        {
            uint64_t user_time = 0, system_time = 0, io_time = 0, idle_time = 0;
            GetCpuTimes(&user_time,
                    &system_time,
                    &io_time,
                    &idle_time);
            reply->mutable_util_response()->set_user_time(user_time);
            reply->mutable_util_response()->set_system_time(system_time);
            reply->mutable_util_response()->set_io_time(io_time);
            reply->mutable_util_response()->set_idle_time(idle_time);
            reply->mutable_util_response()->set_util_present(true);
        }

        /* Simply copy request id into the reply - this was just a 
        piggyback message.*/
        reply->set_request_id(request->request_id());

        /* Get the current idle time and total time
        so as to calculate the CPU util when the cf is done.*/
        size_t idle_time_initial = 0, total_time_initial = 0, idle_time_final = 0, total_time_final = 0;
        //GetCpuTimes(&idle_time_initial, &total_time_initial);

        // Unpack received queries and point IDs
        uint32_t cf_server_id, shard_size;
        uint64_t start_time, end_time;
        start_time = GetTimeInMicro();
        Request user_item;
        UnpackCFServiceRequest(request,
                &user_item);

        end_time = GetTimeInMicro();
        reply->mutable_timing_data_in_micro()->set_unpack_cf_srv_req_time_in_micro((end_time - start_time));
        /* Next piggy back message - sent the received query back to the 
        index server. Helps to merge async responses.*/
        // Remove duplicate point IDs.
        //RemoveDuplicatePointIDs(point_ids_vec);

        // Dataset dimension must be equal to queries dimension.
    #if 0
        dataset.ValidateDimensions(dataset.GetPointDimension(),
                queries.GetPointDimension());
    #endif

        // Calculate the top K distances for all queries.
        start_time = GetTimeInMicro();
        float rating = 0.0;
        CalculateRating(user_item,
                cf_matrix,
                &rating);
        end_time = GetTimeInMicro();
        reply->mutable_timing_data_in_micro()->set_calculate_cf_srv_time_in_micro((end_time - start_time));
        // Difei: Added dispersion between each response
        start_time = GetTimeInMicro();
        end_time = GetTimeInMicro();
        int rand_num = rand() % 1000000;
        while (end_time - start_time < rand_num)
        {
            end_time = GetTimeInMicro();
        }
        // Convert K-NN into form suitable for GRPC.
        start_time = GetTimeInMicro();
        PackCFServiceResponse(rating, 
                reply);
        end_time = GetTimeInMicro();
        reply->mutable_timing_data_in_micro()->set_pack_cf_srv_resp_time_in_micro((end_time - start_time));
        //GetCpuTimes(&idle_time_final, &total_time_final);
        const float idle_time_delta = idle_time_final - idle_time_initial;
        const float total_time_delta = total_time_final - total_time_initial;
        const float cpu_util = (100.0 * (1.0 - (idle_time_delta/total_time_delta)));
        reply->mutable_timing_data_in_micro()->set_cpu_util(cpu_util);
                return Status::OK;
    }
};

void RunServer() {
    std::string server_address(ip_port);
    ServiceImpl service;
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    try
    {
        builder.AddListeningPort(server_address,
                grpc::InsecureServerCredentials());
    } catch(...) {
        CHECK(false, "ERROR: Enter a valid IP address follwed by port number - IP:Port number\n");
    }
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    // Proceed to the server's main loop.
    // Difei
    server->Wait();
}

int main(int argc, char** argv) {
    std::string dataset_file_name;
    if (argc == 7) {
        try
        {
            dataset_file_name = argv[1];
        }
        catch(...)
        {
            CHECK(false, "Enter a valid string for dataset file path\n");
        }
    } else {
        CHECK(false, "Format: ./<cf_server> <dataset file path> <IP address:Port Number> <Mode 1 - read dataset from text file OR Mode 2 - read dataset from binary file > <num of cores: -1 if you want all cores on the machine> <cf server number> <number of cf servers in the system>\n");
    }
    // Load the cf server IP
    ip_port = argv[2];
    int mode = atoi(argv[3]);
    num_cores = atoi(argv[4]);
    if ( (num_cores == -1) || (num_cores > GetNumProcs()) ) {
        num_cores = GetNumProcs();
    }
    cf_parallelism = num_cores;
    cf_server_number = atoi(argv[5]);
    num_cf_servers = atoi(argv[6]);

    CreateDatasetFromFile(dataset_file_name, &dataset);

    cf_matrix = new CF(dataset, amf::NMFALSFactorizer(), 5, 5);    
    // TODO:Difei
    // cf_matrix->Init();

    RunServer();
    return 0;
}
