#!/usr/bin/env python
import os
import sys
import subprocess
import optparse


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
        pass

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

