##
## EPITECH PROJECT, 2025
## Makefile
##

all:
	make -C sources/client
	make -C sources/server

install:
	make -C sources/client install
	make -C sources/server install

clean:
	make -C sources/client clean
	make -C sources/server clean
	rm server
	rm client

fclean:
	make -C sources/client clean
	make -C sources/server clean
	rm -f server
	rm -f client

re: clean all
