// #include "host.h"
// #include "host_constants.h"
// #include "host_to_fpga.h"
// #include "verifty.h"

// void printLagrangeHalfCPolynomial(const LagrangeHalfCPolynomial* poly, std::ofstream& outputFile) {
//    outputFile << "LagrangeHalfCPolynomial Data Pointer: " << poly->data << "\n";
//     outputFile << "LagrangeHalfCPolynomial Precomp Pointer: " << poly->precomp << "\n";
// }

// // Function to print TLweSampleFFT contents
// void printTLweSampleFFT(const TLweSampleFFT* sample, std::ofstream& outputFile) {
//     outputFile << "TLweSampleFFT current_variance: " << sample->current_variance << "\n";
//     outputFile << "TLweSampleFFT k: " << sample->k << "\n";
//     // Assuming a and b are arrays of length k+1
//     outputFile << "TLweSampleFFT a (polynomials): \n";
//     for (int i = 0; i <= sample->k; ++i) {
//         printLagrangeHalfCPolynomial(&sample->a[i], outputFile);
//     }
//     outputFile << "TLweSampleFFT b (right term): \n";
//     printLagrangeHalfCPolynomial(sample->b, outputFile);
// }

// // Function to print TGswSampleFFT details
// void printTGswSampleFFT(const TGswSampleFFT* bkFFT, int n, std::ofstream& outputFile) {
//     outputFile << "TGswSampleFFT k: " << bkFFT->k << "\n";
//     outputFile << "TGswSampleFFT l: " << bkFFT->l << "\n";
//     outputFile << "TGswSampleFFT all_samples: \n";
//     // Assuming we know the size of all_samples array or using k and l for calculation
//     int total_samples = (bkFFT->k + 1) * bkFFT->l;
//     for (int i = 0; i < total_samples; ++i) {
//         outputFile << "Sample " << i << ":\n";
//         printTLweSampleFFT(&bkFFT->all_samples[i], outputFile);
//     }
//     // If you need to print `sample` which is an array of pointers to TLweSampleFFT
//     outputFile << "TGswSampleFFT sample (optional blocks): \n";
//     for (int i = 0; i < total_samples; ++i) {
//         if (bkFFT->sample[i] != nullptr) { // Checking if the pointer is not null
//             outputFile << "Sample Block " << i << ":\n";
//             printTLweSampleFFT(bkFFT->sample[i], outputFile);
//         }
//     }
// }
// // Placeholder for printing TorusPolynomial contents
// void printTorusPolynomial(const TorusPolynomial* poly,std::ofstream& libOutputFile) {
//     libOutputFile << "N: " << poly->N << "\n";
//     libOutputFile << "CoefsT: ";
//     for (int i = 0; i < poly->N; ++i) {
//         libOutputFile << poly->coefsT[i] << " ";
//     }
//     libOutputFile << "\n";
// }

// // Placeholder for printing TLweSample contents
// void printTLweSample(const TLweSample* sample,std::ofstream& libOutputFile) {
//     libOutputFile << "a (polynomials): \n";
//     for (int i = 0; i <= sample->k; ++i) {
//         printTorusPolynomial(&sample->a[i],libOutputFile);
//     }
//     libOutputFile << "b (right term): \n";
//     printTorusPolynomial(sample->b,libOutputFile);
//     libOutputFile << "Current variance: " << sample->current_variance << "\n";
// }



// void printTfheBlindRotateFftParams(const TLweSample* accum, const TGswSampleFFT* bkFFT, const int32_t* bara, int n, const TGswParams* bk_params) {
//     std::ofstream libOutputFile("lib_tfhe_ouput.log"); // Open or create output.log for writing
//     if (!libOutputFile.is_open()) {
//         std::cerr << "Failed to open output.log for writing." << std::endl;
//     }

//     libOutputFile << "Printing tfhe_blindRotate_FFT parameters:\n";
//     libOutputFile << "N (degree of polynomials): " << bk_params->tlwe_params->N << "\n";
//     libOutputFile << "k (number of polynomials in the mask): " << bk_params->tlwe_params->k << "\n";
//     libOutputFile << "kpl (number of rows): " << bk_params->kpl << "\n";
//     libOutputFile << "l (decomp length): " << bk_params->l << "\n";

//     libOutputFile << "Accumulator:\n";
//     printTLweSample(accum,libOutputFile);

//    libOutputFile << "BK FFT:\n";
//    printTGswSampleFFT(bkFFT, n, libOutputFile);
  

//     libOutputFile << "Bara: ";
//     for (int i = 0; i < n; ++i) {
//         libOutputFile << bara[i] << " ";
//     }
//     libOutputFile << "\n";

//     libOutputFile << "n: " << n << "\n";
//     libOutputFile.close();
// }
// int main(int argc, char** argv) {

//     if (argc != 2) {
//         std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
//         return EXIT_FAILURE;
//     }
    
//     clock_t htod, dtoh, comp; 

//     /*====================================================CL===============================================================*/

//     std::string binaryFile = argv[1];
//     cl_int err;
//     cl::Context context;
//     cl::Kernel krnl1, krnl2, krnl3, krnl4;
//     cl::CommandQueue q;
    
//     auto devices = get_xil_devices();
//     auto fileBuf = read_binary_file(binaryFile);
//     cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
//     bool valid_device = false;
//     for (unsigned int i = 0; i < devices.size(); i++) {
//         auto device = devices[i];
//         OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
//         OCL_CHECK(err, q = cl::CommandQueue(context, device, 0, &err));
//         std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
//         cl::Program program(context, {device}, bins, nullptr, &err);
//         if (err != CL_SUCCESS) {
//             std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
//         } else {
//             std::cout << "Device[" << i << "]: program successful!\n";
//             std::cout << "Setting CUs up..." << std::endl; 
//             OCL_CHECK(err, krnl1 = cl::Kernel(program, "tfhe_bootstrap_FFT", &err));
//             OCL_CHECK(err, krnl2 = cl::Kernel(program, "tfhe_bootstrap_woKS_FFT", &err));
//             OCL_CHECK(err, krnl3 = cl::Kernel(program, "mulByXaiOrCopy", &err));
//            OCL_CHECK(err, krnl4 = cl::Kernel(program, "tfhe_blindRotate_FFT", &err));
//             valid_device = true;
//             break; // we break because we found a valid device
//         }
//     }
//     if (!valid_device) {
//         std::cout << "Failed to program any device found, exit!\n";
//         exit(EXIT_FAILURE);
//     }

//     /*====================================================TFHE CLIENT===============================================================*/

//     //generate a keyset
//     printf("Generating a keyset.\n");
//     const int minimum_lambda = 110;
//     TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

//     //generate a random key
//     printf("Generating a random key\n");
//     uint32_t seed[] = { 314, 1592, 657 };
//     tfhe_random_generator_setSeed(seed,3);
//     TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

//     //export the secret key to file for later use
//     printf("Exporting client secret key.\n");
//     FILE* secret_key = fopen("./keys/secret.key","wb");
//     export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
//     fclose(secret_key);

//     //export the cloud key to a file (for the cloud)
//     printf("Exporting cloud key.\n");
//     FILE* cloud_key = fopen("./keys/cloud.key","wb");
//     export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
//     fclose(cloud_key);
                                                          
//     //you can put additional instructions here!!
//     //...

//     //generate encrypt the 16 bits of 499
//     int16_t plaintext1 = 499;
//     LweSample* ciphertext1 = new_gate_bootstrapping_ciphertext_array(16, params);
//     for (int i=0; i<16; i++) {
//         bootsSymEncrypt(&ciphertext1[i], (plaintext1>>i)&1, key);
//     }

//     //generate encrypt the 16 bits of 388
//     int16_t plaintext2 = 388;
//     LweSample* ciphertext2 = new_gate_bootstrapping_ciphertext_array(16, params);
//     for (int i=0; i<16; i++) {
//         bootsSymEncrypt(&ciphertext2[i], (plaintext2>>i)&1, key);
//     }

//     printf("Hi there! Today, I will ask the cloud what is the OR between %d and %d\n",plaintext1, plaintext2);
    
//     //export the 2x16 ciphertexts to a file (for the cloud)
//     FILE* cloud_data = fopen("./data/cloud.data","wb");
//     for (int i=0; i<16; i++) 
//         export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext1[i], params);
//     for (int i=0; i<16; i++) 
//         export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext2[i], params);
//     fclose(cloud_data);

//     //clean up all pointers
//     delete_gate_bootstrapping_ciphertext_array(16, ciphertext2);
//     delete_gate_bootstrapping_ciphertext_array(16, ciphertext1);


//     //clean up all pointers
//     delete_gate_bootstrapping_secret_keyset(key);
//     delete_gate_bootstrapping_parameters(params);

//     /*====================================================HOST BUFFERS===============================================================*/
    
//     //LweBootstrappingKeyFFT_FPGA *in_bk;
//     TGswSampleFFT_FPGA *in_bkFFT; 
//     Torus32_FPGA *in_mu;
//     LweSample_FPGA *in_x;

//     if (posix_memalign((void**) &in_mu, 4096, sizeof(Torus32_FPGA))) throw std::bad_alloc();
//     if (posix_memalign((void**) &in_bkFFT, 4096, sizeof(TGswSampleFFT_FPGA) * n)) throw std::bad_alloc();
//     if (posix_memalign((void**) &in_x, 4096, sizeof(LweSample_FPGA))) throw std::bad_alloc();
//     //if (posix_memalign((void**) &in_bk, 4096, sizeof(LweBootstrappingKeyFFT_FPGA))) throw std::bad_alloc();

//     TLweSample_FPGA *out_result;

//     if (posix_memalign((void**) &out_result, 4096, sizeof(TLweSample_FPGA))) throw std::bad_alloc();
//     //if (posix_memalign((void**) &out_bara, 4096, sizeof(Torus32_FPGA) * n)) throw std::bad_alloc();
//     //if (posix_memalign((void**) &out_barb, 4096, sizeof(Torus32_FPGA))) throw std::bad_alloc();
//     /*====================================================TFHE CLOUD===============================================================*/

//     //reads the cloud key from file
//     cloud_key = fopen("./keys/cloud.key","rb");
//     TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
//     fclose(cloud_key);
 
//     //if necessary, the params are inside the key
//     const TFheGateBootstrappingParameterSet* cparams = bk->params;

//     //read the 2x16 ciphertexts
//     ciphertext1 = new_gate_bootstrapping_ciphertext_array(16, cparams);
//     ciphertext2 = new_gate_bootstrapping_ciphertext_array(16, cparams);

//     //reads the 2x16 ciphertexts from the cloud file
//     cloud_data = fopen("./data/cloud.data","rb");
//     for (int i=0; i<16; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i], cparams);
//     for (int i=0; i<16; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i], cparams);
//     fclose(cloud_data);

//     //do some operations on the ciphertexts: here, we will compute the
//     //minimum of the two
//     //LweSample* result = new_gate_bootstrapping_ciphertext_array(16, params);
//     //OR(result, ciphertext1, ciphertext2, 16, bk);

//     // //export the 32 ciphertexts to a file (for the cloud)
//     // FILE* answer_data = fopen("./data/answer.data","wb");
//     // for (int i=0; i<16; i++) export_gate_bootstrapping_ciphertext_toFile(answer_data, &result[i], params);
//     // fclose(answer_data);


//     static const Torus32 MU = modSwitchToTorus32(1, 8);
//     const LweParams *in_out_params = bk->params->in_out_params;

//     LweSample *temp_result = new_LweSample(in_out_params);

//     //compute: (0,1/8) + ca + cb
//     static const Torus32 OrConst = modSwitchToTorus32(1, 8);
//     lweNoiselessTrivial(temp_result, OrConst, in_out_params);
//     lweAddTo(temp_result, ciphertext1, in_out_params);
//     lweAddTo(temp_result, ciphertext2, in_out_params);

//     //if the phase is positive, the result is 1/8
//     //if the phase is positive, else the result is -1/8
//     //tfhe_bootstrap_FFT(result, bk->bkFFT, MU, temp_result);


//     *in_mu = MU; 
//     LweSample_Host_to_FPGA(temp_result, in_x);
//     //LweBootstrappingKeyFFT_Host_to_FPGA(bk->bkFFT, in_bk);
//     for (int i = 0; i < n; i++) {
//         TGswSampleFFT_Host_to_FPGA(&(bk->bkFFT->bkFFT[i]), &(in_bkFFT[i]));
//     }


//     /*====================================================CPU===============================================================*/
    
//     const TGswParams *bk_params = bk->bkFFT->bk_params;
//     const TLweParams *accum_params = bk->bkFFT->accum_params;
//     const LweParams *in_params = bk->bkFFT->in_out_params;
//     const int32_t N = accum_params->N;
//     const int32_t Nx2 = 2 * N;
//     const int32_t n = in_params->n;
//     int32_t *bara = new int32_t[N];
//     // Modulus switching
//     TorusPolynomial *testvect = new_TorusPolynomial(N);
//     TorusPolynomial *v = new_TorusPolynomial(N);
//     int32_t barb = modSwitchFromTorus32(temp_result->b, Nx2);
//     for (int32_t i = 0; i < n; i++) {
//         bara[i] = modSwitchFromTorus32(temp_result->a[i], Nx2);
//     }

//     // the initial testvec = [mu,mu,mu,...,mu]
//     for (int32_t i = 0; i < N; i++) testvect->coefsT[i] = MU;

//     // testvector = X^{2N-barb}*v
//     if (barb != 0) torusPolynomialMulByXai(v, 2 * N - barb, testvect);
//     else torusPolynomialCopy(v, testvect);

//     TLweSample *acc = new_TLweSample(accum_params);


//     tLweNoiselessTrivial(acc, v, accum_params);
//     // Blind rotation
//     tfhe_blindRotate_FFT(acc, bk->bkFFT->bkFFT, bara, n, bk_params);

//     //Print the above function parameter
//    // printTfheBlindRotateFftParams(acc, bk->bkFFT->bkFFT, bara, n, bk_params);
//     /*====================================================CPU===============================================================*/
//     delete_LweSample(temp_result);

//     //clean up all pointers
//     //delete_gate_bootstrapping_ciphertext_array(16, result);
//     delete_gate_bootstrapping_ciphertext_array(16, ciphertext2);
//     delete_gate_bootstrapping_ciphertext_array(16, ciphertext1);
    
    
//     delete_gate_bootstrapping_cloud_keyset(bk);
//     /*====================================================Setting up kernel I/O===============================================================*/

//     /* INPUT BUFFERS */
//     OCL_CHECK(err, cl::Buffer buffer_in_bk(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(TGswSampleFFT_FPGA) * n, in_bkFFT, &err)); 
//     OCL_CHECK(err, cl::Buffer buffer_in_mu(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(Torus32_FPGA), in_mu, &err)); 
//     OCL_CHECK(err, cl::Buffer buffer_in_x(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(LweSample_FPGA), in_x, &err)); 

//     /* OUTPUT BUFFERS */
//     OCL_CHECK(err, cl::Buffer buffer_out_result(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(TLweSample_FPGA), out_result, &err)); 
//     //OCL_CHECK(err, cl::Buffer buffer_out_bara(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(Torus32_FPGA) * n, out_bara, &err)); 

//     /* SETTING INPUT PARAMETERS */
//     //OCL_CHECK(err, err = krnl1.setArg(0, buffer_in_bk));
//     OCL_CHECK(err, err = krnl1.setArg(0, buffer_in_mu));
//     OCL_CHECK(err, err = krnl1.setArg(1, buffer_in_x));

//     /* SETTING OUTPUT PARAMETERS */
//     OCL_CHECK(err, err = krnl4.setArg(2, buffer_in_bk));
//     OCL_CHECK(err, err = krnl4.setArg(3, buffer_out_result));

//     /*====================================================KERNEL===============================================================*/
//     /* HOST -> DEVICE DATA TRANSFER*/
//     std::cout << "HOST -> DEVICE" << std::endl; 
//     htod = clock(); 
//     OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in_bk}, 0 /* 0 means from host*/));
//     OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in_mu}, 0 /* 0 means from host*/));
//     OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in_x}, 0 /* 0 means from host*/));
//     q.finish();
//     htod = clock() - htod; 
    
//     /*STARTING KERNEL(S)*/
//     std::cout << "STARTING KERNEL(S)" << std::endl; 
//     comp = clock(); 
// 	OCL_CHECK(err, err = q.enqueueTask(krnl1));
//     OCL_CHECK(err, err = q.enqueueTask(krnl2));
//     OCL_CHECK(err, err = q.enqueueTask(krnl3));
//     OCL_CHECK(err, err = q.enqueueTask(krnl4));
//     q.finish(); 
//     comp = clock() - comp;
//     std::cout << "KERNEL(S) FINISHED" << std::endl; 

//     /*DEVICE -> HOST DATA TRANSFER*/
//     std::cout << "HOST <- DEVICE" << std::endl; 
//     dtoh = clock();
//     OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_out_result}, CL_MIGRATE_MEM_OBJECT_HOST));
//     //OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_out_bara}, CL_MIGRATE_MEM_OBJECT_HOST));
//     q.finish();
//     dtoh = clock() - dtoh;

//     /*=======================================================Output log=====================================================================*/
//     std::ofstream outputFile("output.log"); // Open or create output.log for writing
//     if (!outputFile.is_open()) {
//         std::cerr << "Failed to open output.log for writing." << std::endl;
//         return EXIT_FAILURE;
//     }
//     /*====================================================VERIFICATION & TIMING===============================================================*/

//     printf("Host -> Device : %lf ms\n", 1000.0 * htod/CLOCKS_PER_SEC);
//     printf("Device -> Host : %lf ms\n", 1000.0 * dtoh/CLOCKS_PER_SEC);
//     printf("Computation : %lf ms\n",  1000.0 * comp/CLOCKS_PER_SEC);
    
//     bool match = true;

//     //verifyT(acc, out_result);

//     for(int i = 0; i < k; i++) {
//         for (int j = 0; j < N; j++) {
//             if (acc->a[i].coefsT[j] != out_result->a[i].coefsT[j]) {
//                // std::cout << j << " host->a: " << acc->a[i].coefsT[j] << " device->a " << out_result->a[i].coefsT[j] << std::endl; 
//                 outputFile << j << " host->a: " << acc->a[i].coefsT[j] << " device->a " << out_result->a[i].coefsT[j] << std::endl; 
//                 match = false; 
//             }
//         }   
//     }
//     // for (int i = 0; i < N; i++) {
//     //     if (acc->b->coefsT[i] != out_result->b.coefsT[i]) {
//     //         std::cout << "host->b: " << acc->b->coefsT[i] << " device->b " << out_result->b.coefsT[i] << std::endl; 
//     //         match = false; 
//     //     }
//     // }

//     if (acc->current_variance != out_result->current_variance) {
//         //std::cout << "host->cv: " << acc->current_variance << " device->cv " << out_result->current_variance << std::endl; 
//         outputFile << "host->cv: " << acc->current_variance << " device->cv " << out_result->current_variance << std::endl; 
//         match = false; 
//     }

//     std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

//     free(in_bkFFT);
//     free(in_mu);
//     free(in_x);
//     free(out_result);
//     //free(out_bara);

//     delete[] bara;
//     delete_TorusPolynomial(testvect);
//     delete_TorusPolynomial(v);
//     delete_TLweSample(acc);

//     // Close the outputFile when done
//     outputFile.close();    

//     return (match ? EXIT_SUCCESS : EXIT_FAILURE);
// }