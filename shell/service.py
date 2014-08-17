from threading import Thread, Lock
import bluetooth as bt
import socket

class SerialPortService(Thread):
    def __init__(self, port):
        super(SerialPortService, self).__init__()
        self.port = port
        self.send_lock = Lock()
        self.setDaemon(True)

    def run(self):
        self.running = True
        self.port.settimeout(0.1)
        while self.running:
            try:
                data = self.port.recv(2048)
                print 'IN:', data
            except :
                pass
        self.port.close()

    def end(self):
        self.running = False

    def send(self, msg):
        self.send_lock.acquire()
        self.port.send(msg)
        self.send_lock.release()
