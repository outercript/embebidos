#!/usr/bin/env python
import os
import sys

import time
import serial
import subprocess
import optparse

SERIAL_PORT = "/dev/ttyUSB0"
SERIAL_BAUD = 9600
SERIAL_SIZE = 32

FRAME_ERROR = "FFFFFFFF"

class Runtest():

    def __init__(self, target, operation, operA, operB, exp, verbose=False):
        self.verbose = verbose
        self.target  = target

        self.operA      = operA
        self.operB      = operB
        self.expected   = exp
        self.operation  = operation

    def run(self):
        if self.target == 'local':
            return self.run_local()
        else:
            return self.run_remote()

    def run_local(self):
        exec_tool = 'flotante'
        cmd = "./%s %s %s %s %s" % (exec_tool, self.operation,
                                    self.operA, self.operB, self.expected)

        if self.verbose:
            print "Executing : ", cmd

        p = subprocess.Popen(cmd, stderr=subprocess.STDOUT, shell=True)
        p.communicate()
        return p.returncode

    def run_remote(self):
        result = ""
        retry_count = 3
        oper_symbol = {'a':'+', 's':'-', 'm':'*', 'd':'/'}
        oper_string = "%s%s%s\n" % (self.operA, oper_symbol[self.operation], self.operB)

        # Initialize the serial port
        serial_com = serial.Serial(port=SERIAL_PORT, baudrate=SERIAL_BAUD, timeout=0.5)

        # Try to send data to uC, retry if data gets corrupted
        while retry_count > 0:

            # Write the data stream for the uC
            serial_com.write(oper_string)

            # Read the response from the uC
            result = serial_com.readline()
            result = result.strip()

            # Check for frame errors
            if result == FRAME_ERROR:
                retry_count -= 1
                print "Frame error: Retrys left %s" % retry_count

            else:
                break

        # Close serial port
        serial_com.close()

        # Check we received a valid value
        if retry_count > 0:
            return self.compare_result(result)

        else:
            print "Error: Too many frame errors"
            return 5

    def compare_result(result):
        diff   = 0
        result = result.strip()

        # Convert Hex to Int representation
        real     = int(result, 16)
        expected = int(self.expected, 16)
        diff     = abs(expected - real)

        if diff > 1:
            print "Real     : %s" % result
            print "Expected : %s" % self.expected
            print "Diff     : %s" % diff

            return 2



if __name__ == "__main__":

    parser = optparse.OptionParser()
    parser.add_option("-r", "--remote", dest="remote", default=False, action="store_true")
    parser.add_option("-v", "--verbose", dest="verbose", default=False, action="store_true")
    parser.add_option("-o", dest="operation")
    parser.add_option("-a", dest="operA")
    parser.add_option("-b", dest="operB")
    parser.add_option("-e", dest="expected")

    (options, remainder) = parser.parse_args()

    if not options.operation:
        sys.exit("Need an operation to dooo!!")

    if not options.operA and not options.operB:
        sys.exit("Missing operands!!")

    if not options.expected:
        sys.exit("Missing expected result")

    target = 'remote' if options.remote else 'local'
    verbose = True if options.verbose else False

    tester = Runtest(target, options.operation,
                     options.operA, options.operB,
                     options.expected, verbose)
    retcode = tester.run()

    if retcode and verbose:
        print "Test Failed"

    sys.exit(retcode)

