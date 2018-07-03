# For future developers of LatNetBuilder and its Python wrapper / GUI

The code is structured in the following way:

+ in folder `code_output`: .txt files in several languages (as of now Python, C++, and Matlab) that allow to construct data points from the generating vector computed by LatNetBuilder.
The code is displayed as text in the GUI, for the user to copy-paste.
Furthermore, the same Python code is used in the Python wrapper (with an `exec` statement) to generate the points
(see method `points()` of class `Search`).

+ in folder `notebooks`: the notebooks used by the GUI (both in dev and production modes)

+ in folder `gui`: the source code to produce the GUI (see more explanations below)

+ `search.py` and `parse_output.py` contain classes to represent a Search instance (that is, the search parameters defined by the user) and a Result instance. They are (almost) independent from the GUI side, and can be used separately.

## Organization of the GUI and the `gui` folder

The GUI is based on the [ipywidgets] library, and follows an object-oriented design. It is an instanciation of the `GUI` class (defined in `gui/__init__.py`), which possesses several `BaseGUIElement` as attributes. A `BaseGUIElement` corresponds to a logical tile in the GUI. Each `BaseGUIElement` has several attributes, which are individual ipywidgets. These widgets can be buttons, dropdowns, text areas, sliders, and also containers (VBox and Hbox) for other widgets. 

Before going any further, we invite the unfamiliar developer to have a look at [the great documentation of ipywidgets], up to the section about Layout and Styling of Jupyter widgets. The section about widget events is particularly important, as it explains the GUI interactivity.

### Events and user interaction

To summarize the main ideas about widget events, when the user modifies the value of a widget, an event is triggered by the library [Traitlets], and a callback can run to handle the event. The callback registered must have the signature `handler(change)` where `change` is a dictionary holding the information about the change. For us, useful keys of this dictionary are `value` (which contains the name of the trait of the widget that has changed), and `new` (which contains the new value of this trait). 

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

For such a syntax to be valid, we need to use [lambda functions], and declare the callback to be `lambda change: handler(change, gui)`. The class `BaseGUIElement` in  `gui/common.py` implements this little trick, while allowing for a rather intuitive syntax to add new widgets and callbacks.

A typical example of a file used to define an instance of `BaseGUIElement` is `gui/figure_of_merit.py`, which has been written as a tutorial file. Look at the bottom first for the function `figure_of_merit()`. It starts by defining some widgets that will be used by that Element, then instanciates and returns a `BaseGUIElement`. You can see that a couple of callbacks are used by that element. They are defined above in the file. They share the same signature (explained above) and the same structure: depending on the new value of the widget that fired the callback, they update the GUI accordingly.

To add a new GUI element, a good starting point would be to copy-paste this file, and work from there.

Some of the callbacks can be quite complex, reflecting the complexity of the rules that define what the user will see in his GUI, depending on his interaction choices. The complexity thus mostly arises from the rules themselves, rather than from the implementation that is mostly straight-forward.

### Layout and styling

The `layout` attribute of each widget contains information for the frontend to display correctly the widget, in the form of CSS specifiers. For us, a very important feature is the `layout.display` attribute. If set to `none`, the widget is invisible. If set to `flex`, it is visible. A large part of the callbacks is about changing the value of this `display` attribute, because we want some parts of the GUI to appear only when certain conditions are met.

Other layout and styling attributes are used, mainly for design purposes. See the code and the documentation for examples.

### Other files

In the folder `gui`, three files do not correspond to instanciations of `BaseGUIElement` that were explained above:

+ `common.py` contains tools used by different elements of the GUI.
+ `main_tab.py` only contains the callback fired when the user changes the uppermost tab between lattices and nets.
+ `parse_input.py` constructs the Search instance by parsing the user's input in the GUI.

## Communication with LatNetBuilder

Several files deal with the communication with LatNetBuilder; we list them below, and invite the reader to look at them in this order. The Python side and the C++ side interact via the command line interface, and log files. When the user clicks the Search button, a `subprocess` instance is launched which contains a call to the LatNetBuilder executable. Moreover, a `thread` instance is launched as well: it encapsulates a Python thread which monitors the output and updates the GUI accordingly. In particular, when the subprocess has finished running, it reads the output files and updates the Output GUI element. The reason we use a Python thread is to avoid blocking the Python kernel while waiting for the C++ process to end. This allows the user to do something else in his notebook in the meantime, for example to abort the C++ process by clicking the Abort button if he wishes to do so.

Files:

+ `gui/button_box.py` contains the callbacks which are fired when the user clicks the Search or the Abort button

+ in `search.py` the `Search` class contains several methods related to the interaction with the C++ software, namely `_launch_subprocess`, `_parse_subprocess`, `execute` and `_monitor_subprocess`.

+ `parse_output.py` contains classes and functions to parse the output files written by LatNetBuilder, and construct Python objects.

+ `gui/output.py` contains the output Base GUI Element, which displays plots and code snippets for the user to visualize and use the result of the C++ computations.