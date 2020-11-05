all: ft_client

ft_client: ft_client.o ft.o node.o dynArray.o
	gcc217 -g ft_client.o ft.o node.o dynArray.o -o ft_client

ft_client.o: ft_client.c ft.h node.h dynarray.h
	gcc217 -c ft_client.c

ft.o: ft.c ft.h node.h dynarray.h
	gcc217 -c ft.c

node.o: node.c node.h dynarray.h
	gcc217 -c node.c

dynArray.o: dynarray.c dynarray.h
	gcc217 -c dynarray.c
