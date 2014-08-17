import sys, atexit
import pickle
import bluetooth as bt
from cmd import Cmd
from service import SerialPortService
import socket

banner = '''Welcome to btsdk shell.
\tType 'help()' for help
'''

class Shell(Cmd):
    prompt = 'btsdk> '
    intro = "btsdk shell"

    def __init__(self):
        Cmd.__init__(self)
        self.device = None
        self.port = None
        self.service = None
        socket.setdefaulttimeout(0.1)

        atexit.register(self.on_exit)
        print 'loading saved data',
        try:
            f = open('save.p', 'rb')
            saved = pickle.load(f)
            self.device = saved['device']
            print 'OK'
        except (IOError):
            print 'FAILED'
        if self.device:
            print 'DUT:', self.device
        else:
            print 'No DUT. please scan and select.'

    def on_exit(self):
        saved = {'device': self.device}
        pickle.dump( saved, open( "save.p", "wb" ) )

    def do_scan(self, arg):
        'scan for bluetooth devices'
        self.devices = bt.discover_devices(lookup_names = True)
        print '%s\t%s\t\t\t%s' %('Index', 'Address', 'Name')
        print
        for i in range(len(self.devices)):
            d = self.devices[i]
            print '%d\t%s\t%s' %(i, d[0], d[1])
        print 'please select one as DUT'

    def do_select(self, line):
        '''select [index]
        select the device'''
        if line == '':
            print 'missing parameter'
            return
        i = int(line)
        if i >= len(self.devices):
            print 'Index %d is out of range 0..%d' %(i, len(self.devices) - 1)
            return
        d = self.devices[i]
        self.device = d[0]
        print 'selected <%d> %s %s' %(i,d[0], d[1])

    def do_conn(self, line):
        'connect to DUT'

        if self.port:
            print 'already connected'
        else:
            print 'connecting ...'
            records = bt.find_service(uuid=bt.SERIAL_PORT_CLASS,
                                      address=self.device)
            if len(records) == 0:
                print "port not found"
                return
            portnum = records[0]['port']
            print 'SPP port is', portnum
        
            self.port = bt.BluetoothSocket(bt.RFCOMM)
            self.port.connect((self.device, portnum))
            self.service = SerialPortService(self.port)
            self.service.start()
            print 'done'

    def do_disc(self, line):
        'disconnect'
        if self.port:
            print 'disconnecting ...'
            self.service.end()
            self.port = None
            print 'done'
        else:
            print 'not connected'

    def do_led(self, line):
        'set led color r,g,b,w'
        self.service.send("1234")

    def do_q(self, line):
        'quit'
        print 'bye'
        return True

    def do_EOF(self, line):
        'quit the system'
        print 'bye'
        return True

shell = Shell()
shell.cmdloop()
