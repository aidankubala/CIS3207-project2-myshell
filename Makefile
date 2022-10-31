#Makefile Aidan Kubala Project 2 Myshell


myshell: utility.c myshell.c
	gcc -o myshell utility.c myshell.c


clean:
	rm *.o