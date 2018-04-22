import socket

dns_cache_size = 100000

class LRUCache(object):
	def __init__(self, size = dns_cache_size):
		self.size = size
		self.cache = dict()
		self.key = []

	def get(self, key):
		if self.cache.has_key(key):
			self.key.remove(key)
			self.key.insert(0, key)
			return self.cache[key]
		else:
			return None

	def set(self, key, value):
		if self.cache.has_key(key):
			self.cache.pop(key)
			self.cache[key] = value
			self.key.remove(key)
			self.key.insert(0, key)
		elif len(self.cache) == self.size:
			old_key = self.key.pop()
			self.cache.pop(old_key)
			self.key.insert(0, key)
			self.cache[key] = value
		else:
			self.cache[key] = value
			self.key.insert(0, key)

dns_cache = LRUCache()

def gethostbyname(name):
	host = dns_cache.get(name)
	if not host:
		try:
			host = socket.gethostbyname(name)
		except:
			host = None
		else:
			dns_cache.set(name, host)
	return host

		
names = []
valid_names = []
def load_names():
	file_obj = open("valid_domains", "r")
	try:
		for line in file_obj:
			names.append(line.strip())
			#print(line.strip())
	finally:
		file_obj.close()
	print("read domains end.")

def save_names():
	sav_file = open("valid_domains", "w");
	for name in valid_names:
		sav_file.write(name+'\n')
	sav_file.close()


def dnscach_test():
	for name in names:
		host = gethostbyname(name)
		if not host:	
			print("can not resolv this domain", name)
		else:
			valid_names.append(name)
			#print(name, host)
		
if __name__ == '__main__':
	load_names()
	from timeit import timeit
	t1 = timeit('dnscach_test()', 'from __main__ import dnscach_test', number = 1)
	#save_names()
	t2 = timeit('dnscach_test()', 'from __main__ import dnscach_test', number = 1)
	print("first excute time", t1)
	print("sencond excute time", t2)
