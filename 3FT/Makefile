all: ft_client

ft_client: ft_client.o ft.o node.o dynarray.o checker.o
	gcc217 -g ft_client.o ft.o node.o dynarray.o checker.o -o ft_client

ft_client.o: ft_client.c ft.h node.h dynarray.h
	gcc217 -g -c ft_client.c

ft.o: ft.c ft.h node.h dynarray.h checker.h
	gcc217 -g -c ft.c

node.o: node.c node.h dynarray.h
	gcc217 -g -c node.c

dynarray.o: dynarray.c dynarray.h
	gcc217 -g -c dynarray.c

checker.o: checker.c checker.h dynarray.h
	gcc217 -g -c checker.c
