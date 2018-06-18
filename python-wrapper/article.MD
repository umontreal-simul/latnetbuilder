# Building a complex GUI with Jupyter widgets

In this article, I want to explain how to combine a few well known tools from the Jupyter ecosystem, in order to build and distribute a User Interface for any software.

My point is that the stack of technologies developed by Jupyter core developers can allow you to write your own UI without much technical overhead. Working knowledge in Python is the only pre-requisite, which puts the design of UI in the reach of multiple organizations with Python-friendly people, but no developer specialized in UIs (think for instance academia). 

And to be as general as possible, we'd like to write our interface for any kind of software, not restricted to Python software. Thus we also present a method to link our Python interface with software using a Command Line Interface.

I developed a UI for a C++ software called Lattice Builder, which is currently developed at University of Montreal in the Department of Computer Science and Operations Research. This software implements a variety of construction algorithms for Quasi Monte Carlo points, which are often used as a replacement for Monte Carlo for high-dimensional integration and stochastic simulation.

This article assumes working knowledge of Python and of the Jupyter interface.

## The stack of tools needed

+ the Jupyter extension [appmode](https://github.com/oschuett/appmode) for the final rendering of the user interface 
+ the [Jupyter notebook](https://github.com/jupyter/notebook), [ipywidgets](https://github.com/jupyter-widgets/ipywidgets) (interactive widgets for Jupyter), and [bqplot](https://github.com/bloomberg/bqplot) (a plotting library for IPython and the Jupyter notebook) for user interactivity
+ the Python [subprocess](https://docs.python.org/3.6/library/subprocess.html#module-subprocess) and [threading](https://docs.python.org/3.6/library/threading.html) modules, which allow to handle processes and threads from Python
+ the Lattice Builder software, written in C++, called from its command line interface

## Writing the interface itself

The main tool here is the [ipywidgets](https://github.com/jupyter-widgets/ipywidgets) package, which allows you to make your Jupyter notebook come alive, by providing interactive HTML widgets (think buttons, sliders, dropdowns, etc.), and a confortable interface to handle user input.
I won't go into too much details about this package, since there are already plenty of resources available online.
The main thing is that each widget ships with an `observe` method which allows your Python GUI to react to user input.

// Put an example here.

Of course scaling from this example to a full-fledged GUI requires some amount of work, and some code organization to keep track of all the rules we want to set up on your GUI.
I personnally chose an object-oriented perspective, seeing each main component of my GUI as an object, which binds individual sub-components and update rules. This allows for this neat wrap-up for my GUI:

// Put code here.

## Communication between the Python interface and the sofware

Here I assume that the software already has a Command Line Interface, i.e. can be called from the command line by specifying some options on the command line.
To make this work with a Python GUI, I used two Python modules: [subprocess](https://docs.python.org/3.6/library/subprocess.html#module-subprocess) and [threading](https://docs.python.org/3.6/library/threading.html).