import docker
import os
import sys
import time
import tempfile
import tarfile

args = sys.argv[1:]
# print('latnetbuilder ' + ' '.join(sys.argv[1:]))

client = docker.from_env()

try:
    client.containers.run('alpine', 'echo hello world')
except:
    raise Exception('Could not communicate with the Docker daemon. Please check that Docker is installed and running.')

try:
    version = client.containers.run('umontrealsimul/latnetbuilder:light', 'latnetbuilder --version').decode("utf-8")[:-1]
    print('Using docker container with ' + version)
except:
    print('Pulling the image...')
    try:
        client.images.pull('umontrealsimul/latnetbuilder:light')
    except:
        print('Could not pull. Please check your Internet connection.')
    print('Running latnetbuilder...')


if '--output-folder' in args:
    folder_path = args[args.index('--output-folder') + 1]
    head, tail = os.path.split(folder_path)
    args[args.index('--output-folder') + 1] = tail

container = client.containers.run('umontrealsimul/latnetbuilder:light', 'latnetbuilder ' + ' '.join(args), detach=True)
name = container.name

for line in container.logs(stream=True, stdout=True, stderr=True):
    print(line.strip().decode("utf-8"))

if '--output-folder' in sys.argv:
    with tempfile.NamedTemporaryFile() as tmp:
        (data, _) = container.get_archive(tail)
        for chunck in data:
            tmp.write(chunck)

        tmp.seek(0)
        with tarfile.open(mode='r', fileobj=tmp) as tar:
            tar.extractall(path=head)
            tar.close()

try:
    container.remove(force=True)
except docker.errors.APIError:
    print('The Docker container %s may not have been removed and/or killed correctly. Please try docker kill %s && docker rm %s' % (name, name, name))