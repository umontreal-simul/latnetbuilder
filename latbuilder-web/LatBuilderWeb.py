#!/usr/bin/env python2

# Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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


from pyjamas.ui.RootPanel import RootPanel
from pyjamas.ui.TextArea import TextArea
from pyjamas.ui.Label import Label
from pyjamas.ui.Button import Button
from pyjamas.ui.HTML import HTML
from pyjamas.ui.CaptionPanel import CaptionPanel
from pyjamas.ui.VerticalPanel import VerticalPanel
from pyjamas.ui.HorizontalPanel import HorizontalPanel
from pyjamas.ui.FlowPanel import FlowPanel
from pyjamas.ui.ListBox import ListBox
from pyjamas.ui.CheckBox import CheckBox
from pyjamas.ui.TextBox import TextBox
from pyjamas.ui.Hyperlink import Hyperlink
from pyjamas.ui.DialogBox import DialogBox
from pyjamas import Window
from pyjamas import DOM
from pyjamas import log
from pyjamas.JSONService import JSONProxy

# TODO: validation
# TODO: extend existing lattice

def window_center():
    left = (Window.getClientWidth() - 300) / 2 + Window.getScrollLeft()
    top = (Window.getClientHeight() - 100) / 2 + Window.getScrollTop()
    return left, top

class LatSize:
    def __init__(self, size_str):
        s = size_str.split('^')
        self.base = int(s[0])
        if len(s) == 1:
            self.max_level = 1
        else:
            self.max_level = int(s[1])
    @property
    def points(self):
        return self.base**self.max_level

def format_time(seconds):
    s = seconds % 60
    m = (seconds - s) / 60
    if m == 0:
        return "{0:.3g} s".format(s)
    else:
        return "{0} min {1:.3g} s".format(m, s)


class TextBoxArray:
    def __init__(self, default_value='0', show_indices=True):
        self._default_value = default_value
        self._show_indices = show_indices
        self._values = []
        self.panel = FlowPanel()

    # public interface

    @property
    def values(self):
        return self._values

    @property
    def size(self):
        return len(self._values)

    @size.setter
    def size(self, value):
        add_count = value - len(self._values)
        if add_count < 0:
            for w in self._values[add_count:]:
                self.panel.remove(w.getParent())
                self._values.remove(w)
        elif add_count > 0:
            if self._values:
                newval = self._values[-1].getText()
            else:
                newval = self._default_value
            for i in range(add_count):
                w = TextBox(Width='3em', Text=newval)
                self._values.append(w)
                panel = VerticalPanel(StyleName='TextBoxArrayCell')
                panel.add(w)
                panel.add(HTML("{}".format(i + value - add_count + 1),
                    HorizontalAlignment='center', Visible=self._show_indices))
                self.panel.add(panel)



class WeightValuesArray:
    REMOTE = None
    def __init__(self, label, expr_var, expr_var_desc, default_value='0.0'):
        self._expr_var = expr_var
        self._expr_var_desc = expr_var_desc
        self._expr_dialog, self._expr = self._create_expr_dialog()
        link = Hyperlink("expression...", StyleName="action")
        link.addClickListener(getattr(self, 'show_expr_dialog'))

        self.panel = HorizontalPanel(Spacing=8)
        self._array = TextBoxArray(default_value, show_indices=True)

        panel = VerticalPanel(Spacing)
        panel.add(HTML("{}: ".format(label), StyleName="CaptionLabel"))
        panel.add(HTML("{}: ".format(self._expr_var), StyleName="CaptionLabel"))
        self.panel.add(panel)
        self.panel.add(self._array.panel)
        self.panel.add(link)

    # public interface

    @property
    def values(self):
        return [x.getText() for x in self._array.values]

    @property
    def size(self):
        return self._array.size

    @size.setter
    def size(self, value):
        self._array.size = value

    def show_expr_dialog(self, dialog):
        self._expr_dialog.setPopupPosition(*window_center())
        self._expr_dialog.show()

    # private methods

    def _create_expr_dialog(self):
        contents = VerticalPanel(StyleName="Contents", Spacing=4)
        msg = ("Enter an expression for the weights, using <em>{0}</em> as the "
                "{1}, for example: <ul><li>{0}^-2</li><li>1/(1+{0}^2)</li><li>0.1^{0}</li></ul>.").format(self._expr_var, self._expr_var_desc)
        contents.add(HTML(msg))
        expr = TextBox(Text='0.1')
        contents.add(expr)
        contents.add(Button("OK", getattr(self, '_close_expr_dialog')))
        dialog = DialogBox(glass=True)
        dialog.setHTML('<b>Set the weights from an expression</b>')
        dialog.setWidget(contents)
        return dialog, expr

    def _close_expr_dialog(self):
        self._expr_dialog.hide()
        expr = self._expr.getText()
        id = WeightValuesArray.REMOTE.array_from_expr(expr, self._expr_var, self.size, self)

    def onRemoteResponse(self, response, request_info):
        try:
            if request_info.method == 'array_from_expr':
                values = [float(x) for x in response]
                for w, v in zip(self._array.values, values):
                    w.setText(str(v))
        except:
            Window.alert(response)

    def onRemoteError(self, code, errobj, request_info):
        message = errobj['message']
        if code != 0:
            Window.alert("HTTP error %d: %s" % (code, message['name']))
        else:
            code = errobj['code']
            Window.alert("JSONRPC Error %s: %s" % (code, message))


class SimpleWeights(object):
    def __init__(self, remove_callback):
        self._remove_callback = remove_callback
        self.panel = VerticalPanel()
        self.value_arrays = []
        for va in self._gen_arrays():
            self.value_arrays.append(va)
            self.panel.add(va.panel)
        link = Hyperlink("remove", StyleName="action")
        link.addClickListener(getattr(self, '_remove'))
        self.panel.add(link)

    # public interface

    @property
    def dimension(self):
        return self.value_arrays and max(va.size for va in self.value_arrays) or 0

    @dimension.setter
    def dimension(self, value):
        for va in self.value_arrays:
            va.size = value

    # abstract methods

    def as_arg(self):
        pass

    def _create_arrays(self):
        pass

    # private methods
    
    def _remove(self):
        self._remove_callback(self)


class ProductWeights(SimpleWeights):
    NAME = 'Product Weights'
    def __init__(self, *args):
        super(ProductWeights, self).__init__(*args)
    def as_arg(self):
        pw, = self.value_arrays
        return 'product:0:' + ','.join(pw.values)
    def _gen_arrays(self):
        yield WeightValuesArray('coordinate weights', 'j', 'coordinate index', '0.1')

class OrderDependentWeights(SimpleWeights):
    NAME = 'Order-Dependent Weights'
    def __init__(self, *args):
        super(OrderDependentWeights, self).__init__(*args)
    def as_arg(self):
        ow, = self.value_arrays
        return 'order-dependent:0:' + ','.join(ow.values)
    def _gen_arrays(self):
        yield WeightValuesArray('order weights', 'k', 'projection order', '0.1')

class PODWeights(SimpleWeights):
    NAME = 'Product and Order-Dependent (POD) Weights'
    def __init__(self, *args):
        super(PODWeights, self).__init__(*args)
    def as_arg(self):
        pw, ow = self.value_arrays
        arg = 'POD:0:' + ','.join(pw.values)
        arg += ':0:' + ','.join(ow.values)
        return arg
    def _gen_arrays(self):
        yield WeightValuesArray('coordinate weights', 'j', 'coordinate index', '0.1')
        yield WeightValuesArray('order weights', 'k', 'projection order', '0.1')

class ProjectionDependentWeights(object):
    NAME = 'Projection-Dependent Weights'
    def __init__(self, remove_callback):
        self._remove_callback = remove_callback
        self.dimension = None
        self.panel = VerticalPanel()
        self.panel.add(HTML("Enter the mapping between coordinates and weights.  "
            "Each line must be <b>comma-separated list of coordinates</b> "
            "followed by a <b>colon</b> and a <b>weight value</b>.  "
            "Spaces are ignored.<br/>"
            "Example line: <code>1,2,5: 0.7</code>."))
        self._text = TextArea(CharacterWidth=20, VisibleLines=8)
        self.panel.add(self._text)
        link = Hyperlink("remove", StyleName="action")
        link.addClickListener(getattr(self, '_remove'))
        self.panel.add(link)

    def as_arg(self):
        arg = 'projection-dependent'
        for line in self._text.getText().strip().replace(' ', '').split('\n'):
            arg += ':' + line.strip()
        return arg

    # private methods
    
    def _remove(self):
        self._remove_callback(self)

class CompoundWeights:
    def __init__(self):
        self.WEIGHT_TYPES = [ProductWeights, OrderDependentWeights, PODWeights,
                ProjectionDependentWeights]
        self._add_dialog, self._wtype = self._create_add_dialog()
        self.panel = VerticalPanel()
        self._list_panel = VerticalPanel(Spacing=8)
        self.panel.add(self._list_panel)
        link = Hyperlink("add...", StyleName="action")
        link.addClickListener(getattr(self, 'show_add_dialog'))
        self.panel.add(link)
        self._weights = []
        self._dimension = 0

    # public interface

    @property
    def dimension(self):
        return self._dimension

    @dimension.setter
    def dimension(self, value):
        self._dimension = value
        for w in self._weights:
            w.dimension = value

    @property
    def weights(self):
        for w in self._weights: yield w

    def add_weights(self, wclass):
        self._weights.append(wclass(getattr(self, 'remove_weights')))
        self._list_panel.add(CaptionPanel(wclass.NAME, self._weights[-1].panel))
        self._weights[-1].dimension = self.dimension

    def remove_weights(self, obj):
        self._list_panel.remove(obj.panel.getParent())
        self._weights.remove(obj)

    def show_add_dialog(self):
        self._add_dialog.setPopupPosition(*window_center())
        self._add_dialog.show()

    # private methods

    def _create_add_dialog(self):
        contents = VerticalPanel(StyleName="Contents", Spacing=4)
        wtype = ListBox(Width="14em")
        wtype.addChangeListener(self)
        for wclass in self.WEIGHT_TYPES:
            wtype.addItem(wclass.NAME, value=wclass)
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Weight type: ", **captionstyle))
        panel.add(wtype)
        contents.add(panel)
        contents.add(Button("OK", getattr(self, '_close_add_dialog')))
        dialog = DialogBox(glass=True)
        dialog.setHTML('<b>Add a new type of weights</b>')
        dialog.setWidget(contents)
        return dialog, wtype

    def _close_add_dialog(self):
        self._add_dialog.hide()
        wclass = self.WEIGHT_TYPES[self._wtype.getSelectedIndex()]
        self.add_weights(wclass)


class GeneratingVector(object):
    def __init__(self, latsize):
        self._latsize = latsize
        self.panel = HorizontalPanel(Spacing=8)

        self.panel.add(HTML("<strong>Generating vector</strong>: ", StyleName="CaptionLabel"))
        self._array = TextBoxArray('1', show_indices=False)
        self.panel.add(self._array.panel)

        link = Hyperlink("Korobov...", StyleName="action")
        link.addClickListener(getattr(self, 'show_korobov_dialog'))
        self.panel.add(link)

        self._korobov_dialog, self._korobov_param = self._create_korobov_dialog()

    # public interface

    @property
    def dimension(self):
        return self._array.size

    @dimension.setter
    def dimension(self, value):
        self._array.size = value

    @property
    def values(self):
        return [x.getText() for x in self._array.values]

    # private methods

    def show_korobov_dialog(self, dialog):
        self._korobov_dialog.setPopupPosition(*window_center())
        self._korobov_dialog.show()

    # private methods

    def _create_korobov_dialog(self):
        contents = VerticalPanel(StyleName="Contents", Spacing=4)
        contents.add(HTML( "Enter the Korobov parameter."))
        kparam = TextBox(Text='2')
        contents.add(kparam)
        contents.add(Button("OK", getattr(self, '_close_korobov_dialog')))
        dialog = DialogBox(glass=True)
        dialog.setHTML('<b>Compute a Korobov generating vector</b>')
        dialog.setWidget(contents)
        return dialog, kparam

    def _close_korobov_dialog(self):
        self._korobov_dialog.hide()
        a = int(self._korobov_param.getText())
        n = LatSize(self._latsize.getText()).points
        values = [1]
        for j in range(1, self.dimension):
            values.append((values[-1] * a) % n)
        for w, v in zip(self._array.values, values):
            w.setText(str(v))


class LatBuilderWeb:
    def setStyleSheet(self, sheet):
        e = DOM.createElement('link')
        e.setAttribute('rel', 'stylesheet')
        e.setAttribute('type', 'text/css')
        e.setAttribute('href', sheet)
        html = Window.getDocumentRoot().parentElement
        head = html.getElementsByTagName('head').item(0)
        head.appendChild(e)

    def onModuleLoad(self):

        self.current_request = None

        Window.setTitle("Lattice Builder Web Interface")
        self.setStyleSheet("./LatBuilderWeb.css")

        self.TEXT_WAITING = "Lattice Builder is working..."
        self.TEXT_ERROR = "Server Error"
        self.FIGURES_OF_MERIT = [
                # (key, name)
                ('{cs}P2',         'P2'),
                ('{cs}P4',         'P4'),
                ('{cs}P6',         'P6'),
                ('{cs}R{alpha}',   'R_alpha'),
                ('spectral',        'spectral'),
                ]
        self.CONSTRUCTION_METHODS = [
            ('explicit:{genvec}',
                "Explicit (Evaluation)",
                "Evaluates the figure of merit for a given generating vector.<br/>"
                "<strong>Please specify the generating vector in the Lattice "
                "Properties panel above.</strong>"),
            ('exhaustive',
                "Exhaustive",
                "Examines all generating vectors and retains the best one."),
            ('random:{samples}',
                "Random",
                "Examines a number of randomly selected generating vectors and "
                "retains the best one."),
            ('Korobov',
                "Korobov",
                "Examines all generating vectors of the form (1, a, a^2 mod n, "
                "..., a^s mod n) and retains the best one."),
            ('random-Korobov:{samples}',
                "Random Korobov",
                "Examines a number of randomly selected generating vectors of "
                "the form (1, a, a^2 mod n, ..., a^s mod n) and retains the "
                "best one."),
            ('CBC',
                "Component-by-Component",
                "Examines all possible values of the components of the "
                "generating vector and selects the best ones, one coordinate "
                "at a time."),
            ('random-CBC:{samples}',
                "Random Component-by-Component",
                "Examines a number of randomly selected values of the "
                "components of the generating vector and selects the best "
                "ones, one coordinate at a time."),
            ('fast-CBC',
                "Fast Component-by-Component",
                "Examines all possible values of the components of the "
                "generating vector and selects the best ones, one coordinate "
                "at a time.  Computation is accelerated by using fast "
                "Fourier transforms."),
            ]
        self.COMBINER_TYPES = [
                ('level:max',   'highest level'),
                ('sum',         'weighted sum'),
                ('max',         'maximum weighted value'),
                ]
        self.NORMALIZATION_TYPES = [
                ('norm:P{alpha}-SL10',      'SL10 P-alpha'),
                ('norm:P{alpha}-DPW08',     'DPW08 P-alpha'),
                ]

        captionstyle = {
                'Width': '10em',
                'HorizontalAlignment': 'right',
                }

        self.remote = LatBuilderService()
        WeightValuesArray.REMOTE = self.remote

        main_panel = VerticalPanel(Spacing=30)

        # information

        info = """<h2>Lattice Builder Web Interface</h2>
        <p>This Web interface allows
        <a href="https://github.com/mungerd/latbuilder#readme">Lattice Builder</a>
        users to call the executable program without having to construct the
        command line explicitly.
        </p>
        <p>Enter the construction parameters below, and press the <em>Search for
        Good Lattices</em> button.  The results will show at the bottom.
        </p>"""
        
        main_panel.add(HTML(info))

        self.version_label = HTML()
        main_panel.add(self.version_label)
        self.remote.backend_version(self)

        params_panel = VerticalPanel(Spacing=15)
        main_panel.add(params_panel)

        # lattice type and size and dimension

        lat_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Lattice Properties", lat_panel))

        self.size = TextBox(Text="2^10")

        self.embedded = CheckBox("embedded")
        self.embedded.addClickListener(self)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Size: ", StyleName="CaptionLabel"))
        panel.add(self.size)
        panel.add(self.embedded)
        lat_panel.add(panel)

        self.dimension = TextBox(Text="3")
        self.dimension.addChangeListener(self)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Dimension: ", StyleName="CaptionLabel"))
        panel.add(self.dimension)
        lat_panel.add(panel)

        self.generating_vector = GeneratingVector(self.size)
        self.generating_vector.panel.setVisible(False)
        lat_panel.add(self.generating_vector.panel)

        # figure of merit

        merit_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Figure of Merit", merit_panel))

        self.norm_type = TextBox(Text="2")
        self.norm_type.addChangeListener(self)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Norm type: ", StyleName="CaptionLabel"))
        panel.add(self.norm_type)
        merit_panel.add(panel)

        self.merit = ListBox()
        self.merit.addChangeListener(self)
        for key, name in self.FIGURES_OF_MERIT:
            self.merit.addItem(name)

        self.merit_cs = CheckBox("Use coordinate-symmetric implementation",
                Checked=True)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Figure of merit: ", StyleName="CaptionLabel"))
        panel.add(self.merit)
        panel.add(self.merit_cs)
        merit_panel.add(panel)

        self.merit_alpha_panel = HorizontalPanel(Spacing=8)
        self.merit_alpha = TextBox(Text="2")
        self.merit_alpha_panel.add(HTML("Value of alpha: ", StyleName="CaptionLabel"))
        self.merit_alpha_panel.add(self.merit_alpha)
        merit_panel.add(self.merit_alpha_panel)


        # filters and combiner

        multilevel_panel = VerticalPanel()
        self.multilevel_panel = CaptionPanel("Multilevel Filters and Combiner", multilevel_panel, Visible=False)
        params_panel.add(self.multilevel_panel)

        self.ml_normalization_enable = CheckBox("Normalization type: ")
        self.ml_normalization_enable.addClickListener(self)
        self.ml_normalization_type = ListBox()
        for key, name in self.NORMALIZATION_TYPES:
            self.ml_normalization_type.addItem(name, value=key)
        panel = HorizontalPanel(Spacing=8)
        panel.add(self.ml_normalization_enable)
        panel.add(self.ml_normalization_type)
        multilevel_panel.add(panel)

        self.ml_lowpass_enable = CheckBox("Low-pass filter value: ")
        self.ml_lowpass_enable.addClickListener(self)
        self.ml_lowpass = TextBox(Text="1.0")
        panel = HorizontalPanel(Spacing=8)
        panel.add(self.ml_lowpass_enable)
        panel.add(self.ml_lowpass)
        multilevel_panel.add(panel)

        self.combiner_type = ListBox()
        for key, name in self.COMBINER_TYPES:
            self.combiner_type.addItem(name, value=key)
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Combiner: ", StyleName="CaptionLabel"))
        panel.add(self.combiner_type)
        multilevel_panel.add(panel)

        # weights

        self.weights = CompoundWeights()
        params_panel.add(CaptionPanel("Weights", self.weights.panel))
        self.weights.add_weights(ProductWeights)

        # construction method

        cons_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Construction Method", cons_panel))

        self.construction = ListBox()
        self.construction.addChangeListener(self)
        for key, name, desc in self.CONSTRUCTION_METHODS:
            self.construction.addItem(name, value=key)
        self.construction_desc = HTML()

        panel = HorizontalPanel(Spacing=8)
        panel.add(self.construction)
        panel.add(self.construction_desc)
        cons_panel.add(panel)

        self.construction_samples_panel = HorizontalPanel(Spacing=8)
        self.construction_samples = TextBox(Text="30")
        self.construction_samples_panel.add(HTML("Random samples: ", StyleName="CaptionLabel"))
        self.construction_samples_panel.add(self.construction_samples)
        cons_panel.add(self.construction_samples_panel)

        # execute button

        panel = VerticalPanel(Spacing=8, Width="100%", HorizontalAlignment='center')
        main_panel.add(panel)

        button_panel = HorizontalPanel()
        panel.add(button_panel)
        self.button_search = Button("Search", self)
        button_panel.add(self.button_search)
        self.button_abort = Button("Abort", self, Visible=False)
        button_panel.add(self.button_abort)

        self.status = Label()
        panel.add(self.status)

        # results

        results_panel = VerticalPanel()
        self.results_panel = CaptionPanel("Results", results_panel, Visible=False)
        main_panel.add(self.results_panel)

        self.results_size = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Lattice size: ", StyleName="ResultsCaptionLabel"))
        panel.add(self.results_size)
        results_panel.add(panel)

        self.results_gen = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Generating vector: ", StyleName="ResultsCaptionLabel"))
        panel.add(self.results_gen)
        results_panel.add(panel)

        self.results_merit = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Merit value: ", StyleName="ResultsCaptionLabel"))
        panel.add(self.results_merit)
        results_panel.add(panel)

        self.results_cpu_time = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("CPU time: ", StyleName="ResultsCaptionLabel"))
        panel.add(self.results_cpu_time)
        results_panel.add(panel)

        self.results_cmd = Label(StyleName='Command')
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Command line: ", StyleName="ResultsCaptionLabel"))
        panel.add(self.results_cmd)
        results_panel.add(panel)


        # update selections

        self.construction.selectValue('CBC')
        
        self.onChange(self.construction)
        self.onChange(self.merit)
        self.onChange(self.dimension)
        self.onClick(self.embedded)
        self.onChange(self.ml_normalization_enable)
        self.onChange(self.ml_lowpass_enable)

        RootPanel().add(main_panel)

    def onChange(self, sender):

        if sender == self.construction:
            key, name, desc = \
                    self.CONSTRUCTION_METHODS[self.construction.getSelectedIndex()]
            self.construction_desc.setHTML(desc)
            self.construction_samples_panel.setVisible('{samples}' in key)
            if key.startswith('explicit'):
                self.generating_vector.panel.setVisible(True)
                self.button_search.setText("Evaluate Figure of Merit")
            else:
                self.generating_vector.panel.setVisible(False)
                self.button_search.setText("Search for Good Lattices")

        elif sender == self.merit:
            key, name = \
                    self.FIGURES_OF_MERIT[self.merit.getSelectedIndex()]
            self.merit_alpha_panel.setVisible('{alpha}' in key)
            self.merit_cs.setVisible('{cs}' in key)

        elif sender == self.dimension:
            # resize weights
            dimension = int(self.dimension.getText())
            self.generating_vector.dimension = dimension
            self.weights.dimension = dimension

        elif sender == self.norm_type:
            self.merit_cs.setVisible(self.norm_type.getText() == "2")


    def onClick(self, sender):
        if sender == self.embedded:
            self.multilevel_panel.setVisible(self.embedded.getChecked())

        elif sender == self.ml_normalization_enable:
            self.ml_normalization_type.setVisible(self.ml_normalization_enable.getChecked())

        elif sender == self.ml_lowpass_enable:
            self.ml_lowpass.setVisible(self.ml_lowpass_enable.getChecked())

        elif sender == self.button_search:

            self.results_panel.setVisible(False)
            self.button_search.setVisible(False)
            self.button_abort.setVisible(True)

            lattype = self.embedded.getChecked() and 'embedded' or 'ordinary'
            size = self.size.getText()
            dimension = self.dimension.getText()

            norm_type = self.norm_type.getText()
            merit, merit_name = \
                    self.FIGURES_OF_MERIT[self.merit.getSelectedIndex()]
            alpha = self.merit_alpha.getText()
            cs = norm_type == 2 and self.merit_cs.getChecked() and 'CS:' or ''

            weights = [w.as_arg() for w in self.weights.weights]

            construction, construction_name, desc = \
                    self.CONSTRUCTION_METHODS[self.construction.getSelectedIndex()]
            samples = self.construction_samples.getText()
            genvec = ','.join(self.generating_vector.values)

            mlfilters = []
            combiner_type = None

            if self.embedded.getChecked():
                if self.ml_normalization_enable.getChecked():
                    ml_normalization_type, ml_normalization_name = \
                            self.NORMALIZATION_TYPES[self.ml_normalization_type.getSelectedIndex()]
                    mlfilters.append(ml_normalization_type.format(alpha=alpha))
                if self.ml_lowpass_enable.getChecked():
                    mlfilters.append('low-pass:{}'.format(self.ml_lowpass.getText()))
                
                combiner_type, combiner_name = \
                        self.COMBINER_TYPES[self.combiner_type.getSelectedIndex()]

            self.status.setText(self.TEXT_WAITING)

            self.current_request = self.remote.latbuilder_exec(
                    lattype,
                    size,
                    dimension,
                    norm_type,
                    merit.format(alpha=alpha, cs=cs),
                    weights,
                    construction.format(samples=samples,genvec=genvec),
                    None,
                    mlfilters,
                    combiner_type,
                    self)

        elif sender == self.button_abort:
            # Need to patch JSONService.sendRequest():
            #
            # return HTTPRequest().asyncPost(self.url, msg_data,
            #                                JSONResponseTextHandler(request_info)
            #                                False, self.content_type,
            #                                self.headers)
            if self.current_request:
                self.current_request.abort()
                self.current_request = None
            self.button_abort.setVisible(False)
            self.button_search.setVisible(True)

        elif sender == self.product_weights_expr_link:
            self.showDialog(self._product_weights_expr_dialog)

        elif sender == self.order_weights_expr_link:
            self.showDialog(self._order_weights_expr_dialog)

    def onRemoteResponse(self, response, request_info):
        try:
            if request_info.method == 'latbuilder_exec':
                self.button_search.setVisible(True)
                self.button_abort.setVisible(False)
                cmd, points, gen, merit, seconds = eval(response)
                self.results_size.setText(points)
                self.results_gen.setText(', '.join(gen))
                self.results_merit.setText(merit)
                self.results_cpu_time.setText(format_time(seconds=seconds))
                self.results_cmd.setText(cmd)
                self.results_panel.setVisible(True)
                self.status.setText("")
            elif request_info.method == 'backend_version':
                version = response
                self.version_label.setHTML("<b>Backend:</b> {}".format(version))
        except:
            self.status.setText(response.replace('\n', '    '))

    def onRemoteError(self, code, errobj, request_info):
        if request_info.method == 'latbuilder_exec':
            self.button_search.setVisible(True)
            self.button_abort.setVisible(False)
        message = errobj['message']
        if code != 0:
            self.status.setText("HTTP error %d: %s" % 
                                (code, message['name']))
        else:
            code = errobj['code']
            if code == -32603:
                self.status.setText("Aborted.")
            else:
                self.status.setText("JSONRPC Error %s: %s" % (code, message))


class LatBuilderService(JSONProxy):
    def __init__(self):
        JSONProxy.__init__(self, "services/LatBuilderService.py",
                ['latbuilder_exec', 'backend_version', 'array_from_expr'])

if __name__ == '__main__':
    app = LatBuilderWeb()
    app.onModuleLoad()
