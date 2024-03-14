VPP := v++

#PLATFORM := xilinx_u250_gen3x16_xdma_4_1_202210_1
PLATFORM := xilinx_u280_xdma_201920_3

TARGET := sw_emu
CONFIG_NAME := config.cfg

KERNEL_SRC := ./src

KERNEL_XO := tfhe_blindRotate_FFT.xo
KERNEL_SOURCES := fft_transform.cpp TorusPolynomial_fft.cpp tLweFFTClear.cpp tLweFromFFTConvert.cpp IntPolynomial_ifft.cpp tGswTorus32PolynomialDecompH.cpp tLweFFTAddMulRTo.cpp tGswFFTExternMulToTLwe.cpp tfhe_MuxRotate_FFT.cpp

PROJECT_NAME := fft

ifeq ($(TARGET), sw_emu) # sw_emu needs the sources, easiest to generate their xo files to include the sources. hw(_emu) have sources manually included
	KERNEL_XO += fft_transform.xo TorusPolynomial_fft.xo tLweFFTClear.xo tLweFromFFTConvert.xo IntPolynomial_ifft.xo tGswTorus32PolynomialDecompH.xo tLweFFTAddMulRTo.xo tGswFFTExternMulToTLwe.xo tfhe_MuxRotate_FFT.xo
endif

# 
VPP_XCLBIN_FLAGS := -l -j 16 -O2 --save-temps  --platform $(PLATFORM) -t $(TARGET) --input_files $(KERNEL_XO) -o $(PROJECT_NAME).xclbin
VPP_XO_FLAGS := -c -j 16 -O2 --save-temps --config $(CONFIG_NAME) --platform $(PLATFORM) -t $(TARGET) -I$(KERNEL_SRC)/include/

xclbin: $(KERNEL_XO)
	$(VPP) $(VPP_XCLBIN_FLAGS)
	emconfigutil --platform $(PLATFORM) --nd 1

%.xo : $(KERNEL_SRC)/%.cpp
ifeq ($(TARGET), sw_emu)
	$(VPP) $(VPP_XO_FLAGS) -k $(basename $(notdir $<)) $< -o $@
else
	$(VPP) $(VPP_XO_FLAGS) -k $(basename $(notdir $<)) $< $(addprefix $(KERNEL_SRC)/, $(KERNEL_SOURCES)) -o $@
endif

runtest: test xclbin
	cp $(PROJECT_NAME).xclbin emconfig.json ./buildotests/test
	XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-bootstrapping-fft-fpga
	XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-gate-bootstrapping-fpga
# XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-decomp-tgsw-fpga
	XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-lwe-fpga
	XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-multiplication-fpga
	XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-addition-boot-fpga
	XCL_EMULATION_MODE=$(TARGET) ./buildotests/test/test-long-run-fpga

clean: 
	rm *.log *.xo *.xo.compile_summary *.xclbin *.xclbin.info *.xclbin.link_summary; rm -rf _x; 