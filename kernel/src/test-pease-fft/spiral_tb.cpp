#include "host.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }
    
    clock_t htod, dtoh, comp; 

    /*====================================================CL===============================================================*/

    std::string binaryFile = argv[1];
    cl_int err;
    cl::Context context;
    cl::Kernel krnl1, krnl2;
    cl::CommandQueue q;
    
    auto devices = get_xil_devices();
    auto fileBuf = read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, 0, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            std::cout << "Setting CUs up..." << std::endl; 
            OCL_CHECK(err, krnl1 = cl::Kernel(program, "spiral_FFT", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }
    /*====================================================HOST BUFFERS===============================================================*/
    
    ///double *inputFFT;
    //double *outputFFT;
    std::vector<double, aligned_allocator<double> > a(2048);
    std::vector<double, aligned_allocator<double> > b(2048);
for (int i = 0; i < 471; i++) {
        a[i] = 16; 
    }
    for (int i = 471; i < 816; i++) {
        a[i] = 0; 
    }
    for (int i = 816; i < 1495; i++) {
        a[i] = -16; 
    }
    for (int i = 1495; i < 1840; i++) {
        a[i] = -0; 
    }
    for (int i = 1840; i < 2048; i++) {
        a[i] = 16; 
    }

   // if (posix_memalign((void**) &inputFFT, 2048, sizeof(double))) throw std::bad_alloc();
  //  if (posix_memalign((void**) &outputFFT, 2048, sizeof(double))) throw std::bad_alloc();
   
    /*====================================================Setting up kernel I/O===============================================================*/

    /* INPUT BUFFERS */
    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(double) * 2048, a.data(), &err)); 
    

    /* OUTPUT BUFFERS */
    OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(double) * 2048, b.data(), &err)); 

    /* SETTING INPUT PARAMETERS */
    OCL_CHECK(err, err = krnl1.setArg(0, buffer_a));

    /* SETTING OUTPUT PARAMETERS */
    OCL_CHECK(err, err = krnl1.setArg(1, buffer_b));
    

    /*====================================================KERNEL===============================================================*/
    /* HOST -> DEVICE DATA TRANSFER*/
    std::cout << "HOST -> DEVICE" << std::endl; 
    htod = clock(); 
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));
    
    q.finish();
    htod = clock() - htod; 
    
    /*STARTING KERNEL(S)*/
    std::cout << "STARTING KERNEL(S)" << std::endl; 
    comp = clock(); 
	OCL_CHECK(err, err = q.enqueueTask(krnl1));
    q.finish(); 
    comp = clock() - comp;
    std::cout << "KERNEL(S) FINISHED" << std::endl; 

    /*DEVICE -> HOST DATA TRANSFER*/
    std::cout << "HOST <- DEVICE" << std::endl; 
    dtoh = clock();
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_b}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    dtoh = clock() - dtoh;

    /*====================================================VERIFICATION & TIMING===============================================================*/

    printf("Host -> Device : %lf ms\n", 1000.0 * htod/CLOCKS_PER_SEC);
    printf("Device -> Host : %lf ms\n", 1000.0 * dtoh/CLOCKS_PER_SEC);
    printf("Computation : %lf ms\n",  1000.0 * comp/CLOCKS_PER_SEC);
    
    bool match = true;
    
    for (int i = 0; i < 2048; i++) 
        std::cout << b[i] << " ";
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

    //free(a);

    //free(b);

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}