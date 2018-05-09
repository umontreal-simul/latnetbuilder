# Lattice Builder Web Interface

This interface was built using [Jupyter](http://jupyter.org/) and [ipywidgets](https://github.com/jupyter-widgets/ipywidgets).

It allows to easily run Lattice Builder without having to manually construct the command line.

## Test and installation

### Run online (no installation needed)

Click here: [![Binder](https://mybinder.org/badge.svg)](https://mybinder.org/v2/gh/PierreMarion23/latbuilder/polynomial?urlpath=/apps/latbuilder/web-ui/Interface.ipynb)

A server is launched on [Binder](https://mybinder.org/), to run the Lattice Builder application.

This is not advised for a production usage, as the computational resource available are limited (for example around 1 GB of RAM).

### Install locally

Two possibilities are given:

#### Use Docker (simplest)

[Docker](https://docs.docker.com/) provides a way to run applications securely isolated in a container, packaged with all its dependencies and libraries.

A Docker image will soon be available, with the whole software bundled (LatticeBuilder, Python, Jupyter) running on a Linux distribution. 

To install:
+ install Docker: https://docs.docker.com/install/
+ pull the image:

```bash
docker pull pierremarion23/latbuilder:complete
```

+ run the image:

```bash
docker run pierremarion23/latbuilder:complete
```

#### Manually install the software and its dependencies 

Required for dev.

1) Install Lattice Builder (see [here](../README.md))
2) Install a Python distribution with the dependencies listed [here](./environment.yml). The simplest way to do so is to download Anaconda, and then execute the commands:

```bash
conda env create -f environment.yml
source activate latbuilder  # on Linux/Max
activate latbuilder  # on Windows
jupyter notebook
```

The `conda env create` command creates a new conda environment. You only need to do it the first time. Afterwards, just activate the environment, and run the Jupyter notebook.
Fore more information, see the [conda documentation about environments](https://conda.io/docs/user-guide/tasks/manage-environments.html).

## More technical details

The application is based on the following stack of technologies:
+ the Jupyter extension [appmode](https://github.com/oschuett/appmode) for the final rendering of the user interface 
+ the [Jupyter notebook](https://github.com/jupyter/notebook), [ipywidgets](https://github.com/jupyter-widgets/ipywidgets) (interactive widgets for Jupyter), and [bqplot](https://github.com/bloomberg/bqplot) (a plotting library for IPython and the Jupyter notebook) for user interactivity
+ the Python [subprocess](https://docs.python.org/3.6/library/subprocess.html#module-subprocess) and [threading](https://docs.python.org/3.6/library/threading.html) modules, which allow to handle processes and threads from Python
+ the Lattice Builder software, written in C++, called from its command line interface

The packaging as a Docker container is based on Docker and [jupyter-repo2docker](https://github.com/jupyter/repo2docker), which is a tool to build, run, and push Docker images from source code repositories. The image contains a Jupyter installation, customized according to the specifications found in the repo. 

The online interface is powered by [Binder](mybinder.org) to construct images. It's essentially a bridge between jupyter-repo2docker and [JupyterHub](https://github.com/jupyterhub/jupyterhub), which hosts user instances with a Jupyter server in the cloud.
 See [this excellent blog post](https://blog.jupyter.org/binder-2-0-a-tech-guide-2017-fd40515a3a84) for more information. 