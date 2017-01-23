sync:
	gcc -o main L2.c
	./main ./$(DIR1) ./$(DIR2)
	rm -f ./main
