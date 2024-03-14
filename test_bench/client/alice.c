#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>

int main() {
    //generate a keyset
    printf("Generating a keyset.\n");
    const int minimum_lambda = 110;
    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    printf("Generating a random key\n");
    uint32_t seed[] = { 314, 1592, 657 };
    tfhe_random_generator_setSeed(seed,3);
    TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

    //export the secret key to file for later use
    printf("Exporting client secret key.\n");
    FILE* secret_key = fopen("./keys/secret.key","wb");
    export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
    fclose(secret_key);

    //export the cloud key to a file (for the cloud)
    printf("Exporting cloud key.\n");
    FILE* cloud_key = fopen("./keys/cloud.key","wb");
    export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
    fclose(cloud_key);
                                                          
    //you can put additional instructions here!!
    //...

    //generate encrypt the 16 bits of 499
    int16_t plaintext1 = 499;
    LweSample* ciphertext1 = new_gate_bootstrapping_ciphertext_array(16, params);
    for (int i=0; i<16; i++) {
        bootsSymEncrypt(&ciphertext1[i], (plaintext1>>i)&1, key);
    }

    //generate encrypt the 16 bits of 388
    int16_t plaintext2 = 388;
    LweSample* ciphertext2 = new_gate_bootstrapping_ciphertext_array(16, params);
    for (int i=0; i<16; i++) {
        bootsSymEncrypt(&ciphertext2[i], (plaintext2>>i)&1, key);
    }

    printf("Hi there! Today, I will ask the cloud what is the OR between %d and %d\n",plaintext1, plaintext2);
    
    //export the 2x16 ciphertexts to a file (for the cloud)
    FILE* cloud_data = fopen("./data/cloud.data","wb");
    for (int i=0; i<16; i++) 
        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext1[i], params);
    for (int i=0; i<16; i++) 
        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext2[i], params);
    fclose(cloud_data);

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(16, ciphertext2);
    delete_gate_bootstrapping_ciphertext_array(16, ciphertext1);


    //clean up all pointers
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);
}
