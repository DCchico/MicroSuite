# MicroSuite
µSuite: A Benchmark Suite for Microservices

µSuite is a suite of OLDI services that are each composed of front-end, mid-tier, and leaf microservice tiers. μSuite includes four OLDI services that incorporate open-source software: a content-based high dimensional search for image similarity — HDSearch, a replication-based protocol router for scaling fault-tolerant key-value stores — Router, a service for performing set algebra on posting lists for document retrieval — Set Algebra, and a user-based item recommender system for predicting user ratings — Recommend.
µSuite was originally written to evaluate OS and network overheads faced by microservices. You can find more details about µSuite in our IISWC paper (http://akshithasriraman.eecs.umich.edu/pubs/IISWC2018-%CE%BCSuite-preprint.pdf).

# License & Copyright
µSuite is free software; you can redistribute it and/or modify it under the terms of the BSD License as published by the Open Source Initiative, revised version.

µSuite was originally written by Akshitha Sriraman at the University of Michigan, and per the the University of Michigan policy, the copyright of this original code remains with the Trustees of the University of Michigan.

If you use this software in your work, we request that you cite the µSuite paper ("μSuite: A Benchmark Suite for Microservices", Akshitha Sriraman and Thomas F. Wenisch, IEEE International Symposium on Workload Characterization, September 2018), and that you send us a citation of your work.

# Installation
To install µSuite, please follow these steps (works on Ubuntu):
Step 0. Preparation
apt-get update && apt-get install -y libssl-dev
apt-get update && apt-get install -y wget
wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.16.1/cmake-3.16.1-Linux-x86_64.sh
sh cmake-linux.sh -- --skip-license --prefix=/usr
rm cmake-linux.sh

Step 1. Install gRPC
apt-get install build-essential autoconf libtool pkg-config
apt-get install libgflags-dev
apt-get install clang-5.0 libc++-dev
git clone -b v1.31.1 https://github.com/grpc/grpc
cd grpc
git submodule update --init
mkdir -p cmake/build
cd cmake/build
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DgRPC_INSTALL=ON \
  -DgRPC_BUILD_TESTS=OFF \
  -DgRPC_SSL_PROVIDER=package \
  -DBUILD_SHARED_LIBS=ON \
  ../..
make -j4 install

Step 2. Install Protobuf 3.13.0
wget https://github.com/protocolbuffers/protobuf/releases/download/v3.13.0/protobuf-all-3.13.0.tar.gz
tar -xvzf protobuf-all-3.13.0.tar.gz
cd protobuf-3.13.0
./configure
make -j4
make check -j4
make install
ldconfig

Step 3. Install OpenSSL and Intel's MKL
sudo apt-get install openssl
sudo apt-get install libssl-dev
wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/13575/l_mkl_2019.0.117.tgz
tar -xzvf l_mkl_2019.0.117.tgz
cd l_mkl_*
./install.sh

Step 4. Install FLANN 
--degrade cmake below to 3.10 with apt autoremove
cd src/HDSearch/mid_tier_service
mkdir build
cd build
cmake ..
sudo make install
make

Step 5. Install MLPACK (2.2.5)
sudo apt-get install libmlpack-dev



Build Components Inside uSuite:

Step 0. Preparation
apt-get install libmemcached-dev libboost-all-dev

Step 1. Build HDSearch
Code Change: Add header file --> #include <grpc++/grpc++.h>
	a) src/HDSearch/bucket_service/service/helper_files/client_helper.h
	b) src/HDSearch/bucket_service/service/helper_files/server_helper.h

cd src/HDSearch/protoc_files
make ---> It's fine if you have errors, just make sure that the ".grpc." and "pb." files get created.
cd ../bucket_service/service
make
cd ../../mid_tier_service/service/
make
cd ../../load_generator/
make --> Open loop load generators are used for measuring latency and closed-loop load generators help measure throughput.
Step back to the MicroSuite parent directory.

Step 2. Build Router:
Code Change: Add header file --> #include <grpc++/grpc++.h>
	a) src/Router/lookup_service/service/helper_files/client_helper.h

cd src/Router/protoc_files
make ---> It's fine if you have errors, just make sure that the ".grpc." and "pb." files get created.
cd ../lookup_service/service
make
cd ../../mid_tier_service/service/
make
cd ../../load_generator/
make
Step back to the MicroSuite parent directory.

Step 3. Build Set Algebra:
Code Change: Add header file --> #include <grpc++/grpc++.h>
	a) src/SetAlgebra/intersection_service/service/helper_files/client_helper.h

cd src/SetAlgebra/protoc_files
make ---> It's fine if you have errors, just make sure that the ".grpc." and "pb." files get created.
cd ../intersection_service/service/
make
cd ../../union_service/service
make
cd ../../load_generator/
make
Step back to the MicroSuite parent directory.

Step 4. Build Recommend:
Code Change:
	a) src/Recommend/cf_service/service/helper_files/client_helper.h
		- Add header file --> #include <grpc++/grpc++.h>
	b) src/Recommend/cf_service/service/cf_server.cc
		- Comment "cf_matrix->Init();"

cd src/Recommend/protoc_files
make ---> It's fine if you have errors, just make sure that the ".grpc." and "pb." files get created.
cd ../cf_service/service/
make
cd ../../recommender_service/service/
make
cd ../../load_generator/
make
Step back to the MicroSuite parent directory.


# To Run MicroSuite
Note: Simply typing <./binary_file_name> for any of µSuite's microservices will tell you the exact arguments that you need to enter for that microservice.

(1) **To run HDSearch:**

*To run the leaf service:*


cd src/HDSearch/bucket_service/service

./bucket_server <dataset file path> <IP address:Port Number> <Mode 1 - read dataset from text file OR Mode 2 - read dataset from binary file <number of bucket server threads> <num of cores: -1 if you want all cores on the machine> <bucket server number> <number of bucket servers in the system>


Description of parameters:

(1) dataset file path: ~/MicroSuite/datasets/HDSearch/image_feature_vectors.dat -> Google Open Images Data Source

(2) Mode 1 - read dataset from text file OR Mode 2 - read dataset from binary file -> Input this as 2 since this data set is a binary file.

(3) number of bucket server threads -> Number of threads you want to launch to perform your bucket service (this is hardware dependent) e.g., 1 thread.

(4) num of cores -> Number of cores you want to use; make this the same as the number of threads you want to launch. e.g., 1

(5) bucket server number -> If you are launching multiple bucket servers, each bucket server needs to know its ID. e.g., If you have one bucket server, its ID or "bucket server number" is 1.

(6) number of bucket servers in the system -> this is the total number of bucket servers your set up has e.g., 1.

*To run the mid-tier service:*

cd ../../mid_tier_service/service/

./mid_tier_server <num_hash_tables> <hash_table_key_length> <num_multi_probe_levels> <number_of_bucket_servers> <file containing bucket server IPs> <dataset file path> <mode number: 1 - read dataset from text file, 2 - binary file> <index server IP address> <number of network poller threads> <number of dispatch threads> <number of async response threads> <get profile stats>

Description of parameters:

(1), (2), (3), i.e., num_hash_tables, hash_table_key_length, num_multi_probe_levels are all FLANN parameters. Please refer to the FLANN paper (https://www.cs.ubc.ca/research/flann/) for details on how they must be set. You could try: num_hash_tables = 1, hash_table_key_length = 13, num_multi_probe_levels = 1 but note that these parameters must be set depending on what you are trying to do.

(4) number_of_bucket_servers -> Number of bucket servers that the mid-tier must contact. e.g., 1

(5) file containing bucket server IPs -> Create a .txt file containing bucket server IPs and port number (for all bucket servers you launched in your system) in the following format:
``
127.0.0.1:50051
127.0.0.1:50052
.
.
``
and pass the file path as this argument.

(6) dataset file path -> ~/MicroSuite/datasets/HDSearch/image_feature_vectors.dat

(7) mode number: 1 - read dataset from text file, 2 - binary file -> Set as "2"

(8) index server IP address -> ID address and port number of the mid-tier server that you are trying to launch here e.g., 127.0.0.1:50050

(9) number of network poller threads -> Number of threads that must pick up requests from the front-end or load generator e.g., 1

(10) number of dispatch threads -> Number of worker threads that requests are dispatched to e.g., 4

(11) number of async response threads -> number of threads that must pick up responses that are sent by the leaves e.g., 4

(12) get profile stats -> If you want to turn on perf monitoring (this is not currently supported). Please input "0".

*To run the load generator:*

cd ../../load_generator/


Run ``load_generator_open_loop`` if you want to measure latency and ``load_generator_closed_loop`` if you want to measure throughput.


./load_generator_open_loop <queries file path> <K-NN result file path> <number_of_nearest_neighbors> <Time to run the program> <QPS> <IP to bind to> <timing file name> <QPS file name> <Util file name>


Description of parameters:

(1) queries file path -> ~/MicroSuite/datasets/HDSearch/image_feature_vectors.dat

(2) K-NN result file path -> file path that you want your result to be written to. The result does not get written in the current version since we only wanted to measure latency. You will have to uncomment a "Print" command in the load generator if you actually want the result.

(3) number_of_nearest_neighbors -> The "k" in k-NN that must be computed e.g., 1

(4) Time to run the program -> How long you want requests to be sent for in seconds e.g., 30 

(5) QPS -> The load that is offered to the systems e.g., 100 Queries Per Second.

(6) IP to bind to: IP address and port number of the mid-tier server that this load generator must send requests to e.g., 127.0.0.1:50050

(7) timing file name: This is for future support. For now, enter a dummy file name.

(8) QPS file name: This is for future support. For now, enter a dummy file name.

(9) Util file name: This is for future support. For now, enter a dummy file name.

Running the other MicroSuite services follow the same format as for HDSearch described above. The differences in format are listed below:

(2) **To run Router:**

A memcached server must be launched for the lookup_server to communicate with. Details on how to launch a memcached server can be found here: https://kyup.com/tutorials/install-use-memcache/

./lookup_server (leaf): parameter "Memcached port number to connect to" refers to the memcache server's port number that the lookup_server must communicate with.

./mid_tier_server : parameter "replication cnt" refers to the number of servers that you want in the replicated pool e.g., if you have 4 lookup servers, you may want _set_ requests to get routed to 3 servers in a replicated pool.

./load_generator_closed_loop: parameters "queries file path" -> ~/MicroSuite/datasets/Router/twitter_requests_query_set.dat, QPS -> a high number of outstanding requests in-flight to get saturation throughput, get ratio and set ratio -> are the raio of get and set requests e.g., 1:1 is entered as 1 1

(3) **To run Set Algebra:**

./intersection_server (leaf): parameter <path to dataset> -> ls ~/MicroSuite/datasets/SetAlgebra/wordIDs_mapped_to_posting_lists.txt (the available tar file must first be extracted on your machine). 

Note that you need to create shards of this entire data set depending on the number of leaf servers you have in your set up. e.g., shard0 must be fed as input to leaf server0

./load_generator_open_loop : parameter <queries file path> -> ~/MicroSuite/datasets/SetAlgebra/query_set.txt 
  
(3) **To run Recommend:**

./cf_server (leaf) : parameters: <dataset file path> -> user_to_movie_ratings.csv (Note that you need to create shards of this entire data set depending on the number of leaf servers you have in your set up. e.g., shard0 must be fed as input to leaf server0)
  
./load_generator_open_loop : parameter: <queries file path> -> ~/MicroSuite/datasets/Recommend/query_set.csv

# Issues
If you have issues with any of the third party software that MicroSuite uses, you will have to look up issues pertaining to that software; I may not be fully qualified to answer those questions.

Note that only the code for all four services is open source. You will have to create your own automation scripts if you need them.

If you have any other issues, please raise an issue in this github repository or email akshitha@umich.edu.

# Maintenance
Frequent code or data pushes to this repository are likely. Please pull from this repository for the latest update.
MicroSuite is developed and maintained by Akshitha Sriraman (http://akshithasriraman.eecs.umich.edu/index.html).
