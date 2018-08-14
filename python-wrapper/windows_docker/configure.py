import os
import sys
import shutil

print('Configuring LatNetBuilder...')
print('Docker MUST be installed on this machine.')
print('Two Docker distributions are available on Windows: "Docker for Windows" and "Docker Toolbox".')

is_docker_installed = ''
while is_docker_installed not in ['y', 'n']:
    is_docker_installed = input('Did you install one of these distributions on this machine (y/n)? ')
if is_docker_installed == 'n':
    exit(1)

is_docker_windows = ''
path_docker = ''

while is_docker_windows not in ['a', 'b']:
    is_docker_windows = input('Which one (a/b)?\n    (a) Docker for Windows \n    (b) Docker Toolbox \n    ')

if is_docker_windows == 'a':
    if shutil.which('docker') is None:
        print('Docker was not found in the path.')
        found = False
        while not found:
            path_docker = input('Input the complete path to the Docker executable (e.g. C:\\Program Files\\Docker\\docker.exe) : ')
            if not os.path.exists(path_docker):
                print('Docker not found in this path.')
            else:
                found = True
    else:
        path_docker = 'docker'

else:
    if shutil.which('docker-machine') is None:
        found = False
        while not found:
            path_docker_machine = input('Input the complete path to the Docker machine (e.g. C:\\Program Files\\Docker Toolbox\\docker-machine.exe) : ')
            if not os.path.exists(path_docker_machine):
                print('Docker machine not found in this path.')
            else:
                found = True
    else:
        path_docker_machine = 'docker-machine'

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'config.txt'), 'w') as f:
    if path_docker:
        f.write('docker: docker for windows\n')
        f.write('"' + path_docker + '"')
    else:
        f.write('docker: docker toolbox\n')
        f.write('"' + path_docker_machine + '"')