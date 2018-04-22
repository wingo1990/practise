import socket
import select
#import Queue

try:
	from http_parser.parser import HttpParser
except ImportError:
	from http_parser.pyparser import HttpParser
	
proxy_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
proxy_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
proxy_socket_addr = ("0.0.0.0", 9901)
proxy_socket.bind(proxy_socket_addr);
proxy_socket.listen(10);

print "server start listen:", proxy_socket_addr

proxy_socket.setblocking(False)
timeout = 10
epoll = select.epoll();
epoll.register(proxy_socket.fileno(), select.EPOLLIN)
msg_queue = {}
fd_to_socket = {proxy_socket.fileno():proxy_socket}
http_parser = HttpParser()

while True:
	print "waiting ...", "msg_queue len:", len(msg_queue)
	events = epoll.poll(timeout)
	if not events:
		print "no events continue..."
		continue
	
	print "got envets to dispatch.."
	for fd, event in events:
		socket = fd_to_socket[fd]
		if socket == proxy_socket:
			conn, addr = proxy_socket.accept()
			print "new connection:", addr
			conn.setblocking(False)
			epoll.register(conn.fileno(), select.EPOLLIN);
			fd_to_socket[conn.fileno()] = conn;
			msg_obj = {"http_parser": HttpParser(), "data": "", "server_host": "", "server_ip": ""}
			msg_queue[conn] = msg_obj 

		elif event & select.EPOLLHUP or event & select.EPOLLERR:
			print "client already closed"
			epoll.unregister(fd)
			fd_to_socket[fd].close()
			del fd_to_socket[fd]

		elif event & select.EPOLLIN:
			data = socket.recv(1024)
			if not data:
				print "may be fin packet, if not unregister, the event will repeat,repeat and repeat."
				epoll.unregister(fd)				
				break				
			
			msg_obj = msg_queue.get(socket, None)
			if not msg_obj:
				msg_queue[socket] = msg_obj
				
			recv_len = len(data);			
			p = msg_obj["http_parser"]
			nparsed = p.execute(data, recv_len)
			assert recv_len == nparsed			
			if p.is_headers_complete():
				print p.get_headers()
							
			#print "recv:", data, "client:", socket.getpeername()
			msg_obj["data"].join(data)
			#epoll.modify(fd, select.EPOLLOUT)
	
		elif event & select.EPOLLOUT:
			try:
				msg = msg_queue[socket].get_nowait()
			except Queue.Empty:
				print socket.getpeername(), "queue empty"
				epoll.modify(fd, select.EPOLLIN)
			else:
				print "sending:", data, "client:", socket.getpeername()
				# todo

epoll.unregister(proxy_socket.fileno())
epoll.close()
proxy_socket.close()

