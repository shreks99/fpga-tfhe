#ifndef HOST_H
#define HOST_H

#pragma once

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

// OCL_CHECK doesn't work if call has templatized function call
#define OCL_CHECK(error, call)                                                                   \
    call;                                                                                        \
    if (error != CL_SUCCESS) {                                                                   \
        printf("%s:%d Error calling " #call ", error code is: %d\n", __FILE__, __LINE__, error); \
        exit(EXIT_FAILURE);                                                                      \
    }

#include <CL/cl2.hpp>
#include <CL/cl_ext_xilinx.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <sys/stat.h>
#include <string>
#include <iomanip>
#include <unistd.h>

typedef float data_t;  

template <typename T>
struct aligned_allocator {
    using value_type = T;

    aligned_allocator() {}

    aligned_allocator(const aligned_allocator&) {}

    template <typename U>
    aligned_allocator(const aligned_allocator<U>&) {}

    T* allocate(std::size_t num) {
        void* ptr = nullptr;

#if defined(_WINDOWS)
        {
            ptr = _aligned_malloc(num * sizeof(T), 4096);
            if (ptr == nullptr) {
                std::cout << "Failed to allocate memory" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
#else
        {
            if (posix_memalign(&ptr, 4096, num * sizeof(T))) throw std::bad_alloc();
        }
#endif
        return reinterpret_cast<T*>(ptr);
    }
    void deallocate(T* p, std::size_t num) {
#if defined(_WINDOWS)
        _aligned_free(p);
#else
        free(p);
#endif
    }
};

std::vector<cl::Device> get_devices(const std::string& vendor_name) {
    size_t i;
    cl_int err;
    std::vector<cl::Platform> platforms;
    OCL_CHECK(err, err = cl::Platform::get(&platforms));
    cl::Platform platform;
    for (i = 0; i < platforms.size(); i++) {
        platform = platforms[i];
        OCL_CHECK(err, std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err));
        if (!(platformName.compare(vendor_name))) {
            std::cout << "Found Platform" << std::endl;
            std::cout << "Platform Name: " << platformName.c_str() << std::endl;
            break;
        }
    }
    if (i == platforms.size()) {
        std::cout << "Error: Failed to find Xilinx platform" << std::endl;
        std::cout << "Found the following platforms : " << std::endl;
        for (size_t j = 0; j < platforms.size(); j++) {
            platform = platforms[j];
            OCL_CHECK(err, std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err));
            std::cout << "Platform Name: " << platformName.c_str() << std::endl;
        }
        exit(EXIT_FAILURE);
    }
    // Getting ACCELERATOR Devices and selecting 1st such device
    std::vector<cl::Device> devices;
    OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices));
    return devices;
}

std::vector<cl::Device> get_xil_devices() {
    return get_devices("Xilinx");
}

cl::Device find_device_bdf(const std::vector<cl::Device>& devices, const std::string& bdf) {
    char device_bdf[20];
    cl_int err;
    cl::Device device;
    int cnt = 0;
    for (uint32_t i = 0; i < devices.size(); i++) {
        OCL_CHECK(err, err = devices[i].getInfo(CL_DEVICE_PCIE_BDF, &device_bdf));
        if (bdf == device_bdf) {
            device = devices[i];
            cnt++;
            break;
        }
    }
    if (cnt == 0) {
        std::cout << "Invalid device bdf. Please check and provide valid bdf\n";
        exit(EXIT_FAILURE);
    }
    return device;
}
cl_device_id find_device_bdf_c(cl_device_id* devices, const std::string& bdf, cl_uint device_count) {
    char device_bdf[20];
    cl_int err;
    cl_device_id device;
    int cnt = 0;
    for (uint32_t i = 0; i < device_count; i++) {
        err = clGetDeviceInfo(devices[i], CL_DEVICE_PCIE_BDF, sizeof(device_bdf), device_bdf, 0);
        if (err != CL_SUCCESS) {
            std::cout << "Unable to extract the device BDF details\n";
            exit(EXIT_FAILURE);
        }
        if (bdf == device_bdf) {
            device = devices[i];
            cnt++;
            break;
        }
    }
    if (cnt == 0) {
        std::cout << "Invalid device bdf. Please check and provide valid bdf\n";
        exit(EXIT_FAILURE);
    }
    return device;
}
std::vector<unsigned char> read_binary_file(const std::string& xclbin_file_name) {
    std::cout << "INFO: Reading " << xclbin_file_name << std::endl;
    FILE* fp;
    if ((fp = fopen(xclbin_file_name.c_str(), "r")) == nullptr) {
        printf("ERROR: %s xclbin not available please build\n", xclbin_file_name.c_str());
        exit(EXIT_FAILURE);
    }
    // Loading XCL Bin into char buffer
    std::cout << "Loading: '" << xclbin_file_name.c_str() << "'\n";
    std::ifstream bin_file(xclbin_file_name.c_str(), std::ifstream::binary);
    bin_file.seekg(0, bin_file.end);
    auto nb = bin_file.tellg();
    bin_file.seekg(0, bin_file.beg);
    std::vector<unsigned char> buf;
    buf.resize(nb);
    bin_file.read(reinterpret_cast<char*>(buf.data()), nb);
    return buf;
}

bool is_emulation() {
    bool ret = false;
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr) {
        ret = true;
    }
    return ret;
}

bool is_hw_emulation() {
    bool ret = false;
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if ((xcl_mode != nullptr) && !strcmp(xcl_mode, "hw_emu")) {
        ret = true;
    }
    return ret;
}
double round_off(double n) {
    double d = n * 100.0;
    int i = d + 0.5;
    d = i / 100.0;
    return d;
}

std::string convert_size(size_t size) {
    static const char* SIZES[] = {"B", "KB", "MB", "GB"};
    uint32_t div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    double size_d = (float)size + (float)rem / 1024.0;
    double size_val = round_off(size_d);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << size_val;
    std::string size_str = stream.str();
    std::string result = size_str + " " + SIZES[div];
    return result;
}

bool is_xpr_device(const char* device_name) {
    const char* output = strstr(device_name, "xpr");

    if (output == nullptr) {
        return false;
    } else {
        return true;
    }
}

#endif