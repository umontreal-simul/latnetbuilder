import subprocess
import os
import sys
import time
import tempfile
import tarfile
import atexit

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    try:
        with open(os.path.join(dir_path, 'config.txt'), 'r') as f:
            config = f.readlines()
        config = [x.strip() for x in config]
    except:
        print('LatNet Builder was not configured. Run "latnetbuilder_configure".')
        return

    if 'docker: docker toolbox' in config:
        res = subprocess.run(config[1] + ' status latnetbuilder', stdout=subprocess.PIPE).stdout.decode('utf-8')
        if 'Stopped' in res:
            subprocess.run(config[1] + ' start latnetbuilder')
        elif not 'Running' in res:
            subprocess.run(config[1] + ' rm latnetbuilder -y')
            subprocess.run(config[1] + ' create latnetbuilder')
            subprocess.run(config[1] + ' start latnetbuilder')

        res = subprocess.run(config[1] + ' env latnetbuilder', stdout=subprocess.PIPE).stdout.decode('utf-8')
        docker_host = res.split('DOCKER_HOST=')[1].split('\n')[0]
        os.environ["DOCKER_HOST"] = docker_host
        docker_tls_verify = res.split('DOCKER_TLS_VERIFY=')[1].split('\n')[0]
        os.environ["DOCKER_TLS_VERIFY"] = docker_tls_verify
        docker_cert_path = res.split('DOCKER_CERT_PATH=')[1].split('\n')[0]
        os.environ["DOCKER_CERT_PATH"] = docker_cert_path

    import docker
    args = sys.argv[1:]
    client = docker.from_env()

    if '--output-folder' in args:
        folder_path = args[args.index('--output-folder') + 1]
        head, tail = os.path.split(folder_path)
        args[args.index('--output-folder') + 1] = tail

    container = client.containers.run('umontrealsimul/latnetbuilder:light', 'latnetbuilder ' + ' '.join(args), detach=True)
    name = container.name

    def stop_container():
        try:
            container.stop(timeout=1)
        except Exception:
            pass
    atexit.register(stop_container)

    for line in container.logs(stream=True, stdout=True, stderr=False):
        print(line.strip().decode("utf-8"))
    
    is_error = False
    for line in container.logs(stream=True, stdout=False, stderr=True):
        print(line.strip().decode("utf-8"), file=sys.stderr)
        is_error = True
    if is_error:
        exit(1)

    try:
        if '--output-folder' in sys.argv:
            with tempfile.NamedTemporaryFile() as tmp:
                (data, _) = container.get_archive(tail)
                for chunck in data:
                    tmp.write(chunck)

                tmp.seek(0)
                with tarfile.open(mode='r', fileobj=tmp) as tar:
                    def is_within_directory(directory, target):
                        
                        abs_directory = os.path.abspath(directory)
                        abs_target = os.path.abspath(target)
                    
                        prefix = os.path.commonprefix([abs_directory, abs_target])
                        
                        return prefix == abs_directory
                    
                    def safe_extract(tar, path=".", members=None, *, numeric_owner=False):
                    
                        for member in tar.getmembers():
                            member_path = os.path.join(path, member.name)
                            if not is_within_directory(path, member_path):
                                raise Exception("Attempted Path Traversal in Tar File")
                    
                        tar.extractall(path, members, numeric_owner=numeric_owner) 
                        
                    
                    safe_extract(tar, path=head)
                    tar.close()
    except:
        print('Impossible to write to the output folder.')

    try:
        container.remove(force=True)
    except docker.errors.APIError:
        print('The Docker container %s may not have been removed and/or killed correctly. Please try docker kill %s && docker rm %s' % (name, name, name))


if __name__=='__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('Interrupted')
