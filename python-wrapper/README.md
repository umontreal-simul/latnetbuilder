# LatNet Builder Web Interface

This interface was built using [Jupyter](http://jupyter.org/) and [ipywidgets](https://github.com/jupyter-widgets/ipywidgets).

It allows to easily run LatNet Builder without having to manually construct the command line.

Installation instructions can be found in the [README of the repository](../README.md).

## Implementation notes

The application is based on the following stack of technologies:
+ the [Jupyter notebook](https://github.com/jupyter/notebook), [ipywidgets](https://github.com/jupyter-widgets/ipywidgets) (interactive widgets for Jupyter) for user interactivity
+ the Python modules [subprocess](https://docs.python.org/3.6/library/subprocess.html#module-subprocess) and [threading](https://docs.python.org/3.6/library/threading.html), which allow to handle processes and threads from Python, as well as [matplotlib](https://matplotlib.org), a plotting library for Python
+ the LatNet Builder software, called from its command line interface
+ the Jupyter extension [appmode](https://github.com/oschuett/appmode) for the final rendering of the user interface 

The packaging as a Docker container is based on Docker and [ready-to-run Docker images](https://github.com/jupyter/docker-stacks) created by the Jupyter project.

The online interface is powered by [Binder](mybinder.org), which is essentially a bridge between:

+ [jupyter-repo2docker](https://github.com/jupyter/repo2docker), which is a tool to build, run, and push Docker images from source code repositories. The image contains a Jupyter installation, customized according to the specifications found in the repository, in the `environment.yml` file.
+ [JupyterHub](https://github.com/jupyterhub/jupyterhub), which hosts user instances with a Jupyter server in the cloud. See [this excellent blog post](https://blog.jupyter.org/binder-2-0-a-tech-guide-2017-fd40515a3a84) for more information. 

## For developers of LatNetBuilder and its Python Interface / GUI

The goal of this part is to present information for developers to extend the Python Interface.

### Dev install

The dev install is pretty straight-forward (at least for Linux and Mac OS X users):

1) Download the LatNet Builder source code
2) Install all the dependencies for LatNet Builder (FFTW, GMP, NTL, Boost) listed [here](../README.MD)
3) Install all the dependencies for the interface: a conda distribution, then

```bash
conda config --add channels conda-forge
conda create -n dev-latnetbuilder python ipywidgets widgetsnbextension notebook matplotlib numexpr appmode numpy
```

4) Run from the root directory of LatNet Builder

```bash
./waf configure --prefix PATH_TO_YOUR_INSTALLATION_FOLDER
./waf build
./waf install
```

Once this is done, you can test the Python GUI by running from the python-wrapper folder

```bash
source activate dev-latnetbuilder
jupyter notebook notebooks/Interface-dev.ipynb
```

In the notebook, modify the latnetbuilder.PATH_TO_LATNETBUILDER variable to fit your PATH_TO_YOUR_INSTALLATION_FOLDER.

This method allows you to modify the Python code and test the new version simply by restarting the Jupyter notebook kernel (see the [doc from Jupyter](https://jupyter-notebook-beginner-guide.readthedocs.io/en/latest/execute.html#executing-a-notebook) for more information).

### Code structure

The code is structured in the following way:

+ in folder `code_output`: .txt files in several languages (as of now Python, C++, and Matlab) that allow to construct data points from the generating vector computed by LatNetBuilder.
The code is displayed as text in the GUI, for the user to copy-paste.
Furthermore, the same Python code is used in the Python interface (with an `exec` statement) to generate the points
(see method `points()` of class `Search`).

+ in folder `notebooks`: the notebooks used by the GUI (both in dev and production modes)

+ in folder `gui`: the source code to produce the GUI (see more explanations below)

+ `search.py` and `parse_output.py` contain classes to represent a Search instance (that is, the search parameters defined by the user) and a Result instance. They are (almost) independent from the GUI side, and can be used separately.

### Organization of the GUI and the `gui` folder

The GUI is based on the [ipywidgets](https://github.com/jupyter-widgets/ipywidgets) library, and follows an object-oriented design. It is an instanciation of the `GUI` class (defined in `gui/__init__.py`), which possesses several `BaseGUIElement` as attributes. A `BaseGUIElement` corresponds to a logical tile in the GUI. Each `BaseGUIElement` has several attributes, which are individual ipywidgets. These widgets can be buttons, dropdowns, text areas, sliders, and also containers (VBox and Hbox) for other widgets. 

Before going any further, we invite the unfamiliar developer to have a look at [the great documentation of ipywidgets](https://ipywidgets.readthedocs.io/en/stable/), up to the section about Layout and Styling of Jupyter widgets. The section about widget events is particularly important, as it explains the GUI interactivity.

#### Events and user interaction

To summarize the main ideas about widget events, when the user modifies the value of a widget, an event is triggered by the library [Traitlets](https://github.com/ipython/traitlets), and a callback can run to handle the event. The registered callback must have the signature `handler(change)` where `change` is a dictionary holding the information about the change. For us, useful keys of this dictionary are `value` (which contains the name of the trait of the widget that has changed), and `new` (which contains the new value of this trait). 

For example, the following is a valid callback:

```python
def handler(change):
    print(change)
```

Because we want the callbacks to modify the gui object, we would like to pass the gui as a second argument to the callback, for instance:

```python
def handler(change, gui):
    print(change)
    gui.my_element.my_widget.value = 42
```

For such a syntax to be valid, we need to use [lambda functions](http://www.diveintopython.net/power_of_introspection/lambda_functions.html), and declare the callback to be `lambda change: handler(change, gui)`. The class `BaseGUIElement` in  `gui/common.py` implements this little trick, while allowing for a rather intuitive syntax to add new widgets and callbacks.

A typical (simple) example of a file used to define an instance of `BaseGUIElement` is `gui/lattice_type.py`, which has been written as a tutorial file. Look at the bottom first for the function `lattice_type()`. It starts by defining some widgets that will be used by that Element, then instanciates and returns a `BaseGUIElement`. Above in the file, you can see a callback used by that element. All callbacks share the same signature (explained above) and the same structure: depending on the new value of the widget that fired the callback, they update the GUI accordingly.

To add a new GUI element, a good starting point would be to copy-paste this file, and work from there.

Some of the callbacks can be quite complex, reflecting the complexity of the rules that define what will be seen in the GUI, depending on the interaction choices. The complexity thus mostly arises from the rules themselves, rather than from the implementation that is mostly straight-forward.

#### Layout and styling

The `layout` attribute of each widget contains information for the frontend to display correctly the widget, in the form of CSS specifiers. For us, a very important feature is the `layout.display` attribute. If set to `'none'`, the widget is invisible. If set to `'flex'`, it is visible. Most callbacks only change the value of this `display` attribute, to hide some parts of the GUI under some conditions.

Other layout and styling attributes are used, mainly for web-design purposes. 

#### Other files

In the folder `gui`, three files do not correspond to instanciations of `BaseGUIElement` that are explained above:

+ `common.py` contains tools used by different elements of the GUI.
+ `main_tab.py` only contains the callback fired when the user changes the uppermost tab between lattices and nets.
+ `parse_input.py` constructs the Search instance by parsing the user's input in the GUI.

### Communication with LatNetBuilder

Several files deal with the communication with LatNetBuilder; we list them below, and invite the reader to look at them in this order. The Python side and the C++ side interact via the command line interface, and log files. When the user clicks the Search button, a [subprocess](https://docs.python.org/3.6/library/subprocess.html#module-subprocess) instance is launched which contains a call to the LatNetBuilder executable. Moreover, a [thread](https://docs.python.org/3.6/library/threading.html) instance is launched as well: it encapsulates a Python thread which monitors the output and updates the GUI accordingly. In particular, when the subprocess has finished running, the thread reads the output files and updates the Output GUI element. The reason we use a Python thread is to avoid blocking the Python kernel while waiting for the C++ process to end. This allows the user to do something else in the notebook in the meantime, for example aborting the C++ process by clicking the Abort button.

Files:

+ `gui/button_box.py` contains the callbacks which are fired when the user clicks the Search or the Abort button

+ in `search.py` the `Search` class contains several methods related to the interaction with the C++ software, namely `_launch_subprocess`, `_parse_subprocess`, `execute` and `_monitor_subprocess`.

+ `parse_output.py` contains classes and functions to parse the output files written by LatNetBuilder, and construct Python objects.

+ `gui/output.py` contains the output Base GUI Element, which displays plots and code snippets for the user to visualize and use the result of the C++ computations.


### Packaging and publishing a new version of the Interface

1) Increment the version number of the conda package in python-wrapper/conda_recipe/meta.yaml
2) Build the new conda package:

```bash
./waf configure --prefix PATH_TO_YOUR_INSTALLATION_FOLDER --link-static --build-full-conda
./waf build
./waf install
```

3) upload the conda package to Anaconda Cloud (where the PATH_TO_PACKAGE can for instance be found in the console ouptut of the ./waf install command):

```bash
anaconda login
anaconda upload PATH_TO_PACKAGE
```

4) update the complete docker image by running from the python-wrapper directory:

```bash
docker build -t umontreal-simul/latnetbuilder:complete --no-cache .
docker push umontreal-simul/latnetbuilder:complete
```

5) update the light docker image by running from the installation directory:

```bash
docker build -t umontreal-simul/latnetbuilder:light -f share/latnetbuilder/DockerfileNoGui .
docker push umontreal-simul/latnetbuilder:light
```