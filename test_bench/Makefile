CC := gcc

CLIENT_DIR := ./client
SERVER_DIR := ./server

TFHE_FLAGS := -ltfhe-spqlios-fma 

all : 
	make alice; make cloud; make verify;  

alice : $(CLIENT_DIR)/alice.c
	$(CC) $^ -o $@ $(TFHE_FLAGS)

cloud : $(SERVER_DIR)/cloud.c
	$(CC) $^ -o $@ $(TFHE_FLAGS)

verify : $(CLIENT_DIR)/verify.c
	$(CC) $^ -o $@ $(TFHE_FLAGS)

clean :
	rm alice cloud verify ./data/cloud.data ./data/answer.data ./keys/cloud.key ./keys/secret.key


