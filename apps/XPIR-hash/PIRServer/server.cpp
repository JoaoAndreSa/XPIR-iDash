/**
    XPIR-hash
    server.cpp
    Purpose: SERVER main
    @author Joao Sa
    @version 1.0 01/07/16
*/

#include "Pipeline/PIRServerPipeline.hpp"
#include "Sequential/PIRServerSequential.hpp"

int main(int argc, char* argv[]){
	Socket::errorExit((Constants::port > 65535) || (Constants::port < 2000),"Please choose a port number between 2000 - 65535");

	Socket socket(0);

	//Create an asio::io_service and a thread_group (through pool in essence)
	boost::asio::io_service ioService;
	/**
		TODO: 	 For threadpool to actually make sense we need to have the database previously stored on the server.
		WARNING: Multiple threads executing at the same time would lead to concurrency problems due to overwriting files
	*/
	boost::thread_group threadpool;

	/**
 		This will start the ioService processing loop. All tasks assigned with ioService.post() will start executing. 
 	*/
	boost::asio::io_service::work work(ioService);

 	//This will add n_threads to the thread pool.
	for(int i=0;i<Constants::n_threads;i++){
		threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
	}

    while (1){
        std::cout << "Listening" << "\n";

        //This is where client connects. svr will hang in this mode until client connects
        socket.acceptConnection();

        if(Constants::pipeline){	//if PIPELINE execution
        	PIRServerPipeline s(socket);

        	//This will assign tasks to the thread pool.
      		ioService.post(boost::bind(&PIRServerPipeline::job,s));	//bind thread to object s (sequential)c
		}
		else{						//if SEQUENTIAL execution
			PIRServerSequential s(socket);

		    //This will assign tasks to the thread pool.
		    ioService.post(boost::bind(&PIRServerSequential::job,s));	//bind thread to object s (sequential)c
		}
    }

	/**clear
		This will stop the ioService processing loop. Any tasks you add behind this point will not execute.
	*/
	ioService.stop();

	/**
		Will wait until all the threads in the thread pool are finished with their assigned tasks and 'join' them.
		Just assume the threads inside the threadpool will be destroyed by this method.
	*/
	threadpool.join_all();

    return 0;
}