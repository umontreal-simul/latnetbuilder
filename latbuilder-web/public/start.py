#!/usr/bin/env python2

from __future__ import print_function

from CGIHTTPServer import CGIHTTPRequestHandler
import BaseHTTPServer
import SimpleHTTPServer
import os

if __name__ == '__main__':
    CGIHTTPRequestHandler.cgi_directories = ['/services']
    os.chdir(os.path.dirname(__file__))
    print("connect a browser to: http://localhost:8000/")
    SimpleHTTPServer.test(CGIHTTPRequestHandler, BaseHTTPServer.HTTPServer)
