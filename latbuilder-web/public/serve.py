#!/usr/bin/env python2

from __future__ import print_function

from CGIHTTPServer import CGIHTTPRequestHandler
from BaseHTTPServer import HTTPServer
import os
import sys
import socket
import webbrowser
from multiprocessing import Process, Queue

def serve(port, q):
    try:
        httpd = HTTPServer(('localhost', port), Handler)
        address, port = httpd.socket.getsockname()
        url = 'http://{}:{}/'.format(address, port)
        q.put(url)
        httpd.serve_forever()
    except socket.error:
        q.put(None)

if __name__ == '__main__':
    CGIHTTPRequestHandler.cgi_directories = ['/services']
    os.chdir(os.path.dirname(__file__))

    Handler = CGIHTTPRequestHandler
    Handler.protocol_version = 'HTTP/1.0'

    for port in range(8080, 8080 + 20):
        q = Queue()
        p = Process(target=serve, args=(port, q))
        p.start()
        url = q.get()
        if url is None:
            continue
        url += 'LatBuilderWeb.html'
        print('Lattice Builder web interface available at: {}'.format(url))
        try:
            webbrowser.open(url)
        except:
            print('could not start browser')
        p.join()
        sys.exit(0)
    print("could not find an available port")
    sys.exit(1)
