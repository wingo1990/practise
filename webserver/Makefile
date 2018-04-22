webserver: webserver.c http_parser/http_parser.o
	gcc -o webserver webserver.c http_parser/http_parser.o -luv

http_parser/http_parser.o:
	$(MAKE) -C http_parser http_parser.o

clean:
	$(MAKE) -C http_parser clean
	rm -f *.o webserver
