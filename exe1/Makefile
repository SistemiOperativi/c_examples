all: gen_data process

gen_data: gen_data.o
	gcc gen_data.o -o gen_data

process: process.o
	gcc process.o -o process


%.o: %.c parameters.h
	gcc $^ -c 
