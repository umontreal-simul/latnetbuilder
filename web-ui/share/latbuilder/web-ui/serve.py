#!/usr/bin/env python2

# Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Universite de Montreal.
# 
# This file is part of Lattice Builder.
# 
# Lattice Builder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Lattice Builder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.


from __future__ import print_function

from CGIHTTPServer import CGIHTTPRequestHandler
from BaseHTTPServer import HTTPServer
import os
import sys
import socket
import webbrowser
from multiprocessing import Process, Queue

def serve(port, q):
    CGIHTTPRequestHandler.cgi_directories = ['/services']
    try:
        Handler = CGIHTTPRequestHandler
        Handler.protocol_version = 'HTTP/1.0'
        httpd = HTTPServer(('localhost', port), Handler)
        address, port = httpd.socket.getsockname()
        url = 'http://{}:{}/'.format(address, port)
        q.put(url)
        httpd.serve_forever()
    except socket.error:
        q.put(None)

if __name__ == '__main__':
    os.chdir(os.path.dirname(__file__))

    for port in range(8080, 8080 + 20):
        q = Queue()
        p = Process(target=serve, args=(port, q))
        p.start()
        url = q.get()
        if url is None:
            continue
        url += ''
        print('Lattice Builder web interface available at: {}'.format(url))
        try:
            webbrowser.open(url)
        except:
            print('could not start browser')
        p.join()
        sys.exit(0)
    print("could not find an available port")
    sys.exit(1)
