#!/usr/bin/env python2

from pyjamas.ui.RootPanel import RootPanel
from pyjamas.ui.TextArea import TextArea
from pyjamas.ui.Label import Label
from pyjamas.ui.Button import Button
from pyjamas.ui.HTML import HTML
from pyjamas.ui.CaptionPanel import CaptionPanel
from pyjamas.ui.VerticalPanel import VerticalPanel
from pyjamas.ui.HorizontalPanel import HorizontalPanel
from pyjamas.ui.ListBox import ListBox
from pyjamas.ui.CheckBox import CheckBox
from pyjamas.ui.TextBox import TextBox
from pyjamas.ui.Hyperlink import Hyperlink
from pyjamas.ui.DialogBox import DialogBox
from pyjamas import Window
from pyjamas import DOM
from pyjamas.JSONService import JSONProxy

# TODO: validation
# TODO: expression (j^-2) for weights
# TODO: explicit construction
# TODO: extend existing lattice
# TODO: guess if latbuilder can be found
# TODO: combinations of weights
# TODO: filters, combiners, normalizations

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
        self.WEIGHT_TYPES = [
                ('product',               'product'),
                ('order-dependent',       'order-dependent'),
                ('POD',                   'product and order-dependent'),
                #('projection-dependent',  'projection-dependent'),
                ]

        captionstyle = {
                'Width': '10em',
                'HorizontalAlignment': 'right',
                }

        self.remote = LatBuilderService()

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

        params_panel = VerticalPanel(Spacing=15)
        main_panel.add(params_panel)

        # lattice type and size and dimension

        lat_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Lattice Properties", lat_panel))

        self.size = TextBox(Text="2^13")

        self.embedded = CheckBox("embedded")
        self.embedded.addClickListener(self)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Size: ", **captionstyle))
        panel.add(self.size)
        panel.add(self.embedded)
        lat_panel.add(panel)

        self.dimension = TextBox(Text="5")
        self.dimension.addChangeListener(self)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Dimension: ", **captionstyle))
        panel.add(self.dimension)
        lat_panel.add(panel)

        # figure of merit

        merit_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Figure of Merit", merit_panel))

        self.norm_type = TextBox(Text="2")

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Norm type: ", **captionstyle))
        panel.add(self.norm_type)
        merit_panel.add(panel)

        self.merit = ListBox()
        self.merit.addChangeListener(self)
        for key, name in self.FIGURES_OF_MERIT:
            self.merit.addItem(name)

        self.merit_cs = CheckBox("Use coordinate-symmetric implementation",
                Checked=True)

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Figure of merit: ", **captionstyle))
        panel.add(self.merit)
        panel.add(self.merit_cs)
        merit_panel.add(panel)

        self.merit_alpha_panel = HorizontalPanel(Spacing=8)
        self.merit_alpha = TextBox(Text="2")
        self.merit_alpha_panel.add(HTML("Value of alpha: ", **captionstyle))
        self.merit_alpha_panel.add(self.merit_alpha)
        merit_panel.add(self.merit_alpha_panel)

        # filters and combiner

        # TODO

        # --multilevel-filters norm:P2-SL10:even:8,13
        # --combiner max

        # weights

        weight_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Weights", weight_panel))

        self.weight_type = ListBox()
        self.weight_type.addChangeListener(self)
        for key, name in self.WEIGHT_TYPES:
            self.weight_type.addItem(name, value=key)
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Weight type: ", **captionstyle))
        panel.add(self.weight_type)
        weight_panel.add(panel)

        self.product_weights_expr_link = Hyperlink("expression")
        self.product_weights_expr_link.addClickListener(self)

        self.product_weights = []
        self.product_weights_panel = HorizontalPanel(Visible=False, Spacing=8)
        self.product_weights_array = HorizontalPanel()
        self.product_weights_panel.add(HTML("Product weights: ", **captionstyle))
        self.product_weights_panel.add(self.product_weights_array)
        self.product_weights_panel.add(self.product_weights_expr_link)
        weight_panel.add(self.product_weights_panel)

        self.order_weights_expr_link = Hyperlink("expression")
        self.order_weights_expr_link.addClickListener(self)

        self.order_weights = []
        self.order_weights_panel = HorizontalPanel(Visible=False, Spacing=8)
        self.order_weights_array = HorizontalPanel()
        self.order_weights_panel.add(HTML("Order-dependent weights: ", **captionstyle))
        self.order_weights_panel.add(self.order_weights_array)
        self.order_weights_panel.add(self.order_weights_expr_link)
        weight_panel.add(self.order_weights_panel)

        # construction method

        cons_panel = VerticalPanel()
        params_panel.add(CaptionPanel("Construction Method", cons_panel))

        self.construction = ListBox()
        self.construction.addChangeListener(self)
        for key, name, desc in self.CONSTRUCTION_METHODS:
            self.construction.addItem(name, value=key)
        self.construction_desc = Label()

        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Construction method: ", **captionstyle))
        panel.add(self.construction)
        panel.add(self.construction_desc)
        cons_panel.add(panel)

        self.construction_samples_panel = HorizontalPanel(Spacing=8)
        self.construction_samples = TextBox(Text="30")
        self.construction_samples_panel.add(HTML("Random samples: ", **captionstyle))
        self.construction_samples_panel.add(self.construction_samples)
        cons_panel.add(self.construction_samples_panel)

        # execute button

        panel = VerticalPanel(Spacing=8, Width="100%", HorizontalAlignment='center')
        main_panel.add(panel)

        self.button_search = Button("Search for Good Lattices", self)
        panel.add(self.button_search)

        self.status = Label()
        panel.add(self.status)

        # results

        self.results_panel = VerticalPanel(Visible=False)
        main_panel.add(CaptionPanel("Results", self.results_panel))

        self.results_size = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Lattice size: ", **captionstyle))
        panel.add(self.results_size)
        self.results_panel.add(panel)

        self.results_gen = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Generating vector: ", **captionstyle))
        panel.add(self.results_gen)
        self.results_panel.add(panel)

        self.results_merit = Label()
        panel = HorizontalPanel(Spacing=8)
        panel.add(HTML("Merit value: ", **captionstyle))
        panel.add(self.results_merit)
        self.results_panel.add(panel)


        # update selections

        self.construction.selectValue('CBC')
        
        self.onChange(self.construction)
        self.onChange(self.merit)
        self.onChange(self.weight_type)
        self.onChange(self.dimension)

        RootPanel().add(main_panel)

        self._product_weights_expr_dialog, self.product_weights_expr = \
                self.create_weights_dialog(
                        getattr(self, 'set_product_weights_expr'),
                        "Enter an expression for the weights, using <em>j</em> as the "
                        "coordinate index, e.g., j^-2 or 1/(1+j^2).")
        self._order_weights_expr_dialog, self.order_weights_expr = \
                self.create_weights_dialog(
                        getattr(self, 'set_order_weights_expr'),
                        "Enter an expression for the weights, using <em>k</em> as the "
                        "projection order, e.g., k^-2 or 1/(1+k^2).")

    def fill_weights(self, array, uiarray, dimension):
        # fill weights panel
        add_count = dimension - len(array)
        if add_count < 0:
            for w in array[add_count:]:
                uiarray.remove(w.getParent())
                array.remove(w)
        elif add_count > 0:
            if len(array) > 0:
                value = array[-1].getText()
            else:
                value = '0.1'
            for i in range(add_count):
                w = TextBox(Width='3em', Text=value)
                array.append(w)
                panel = VerticalPanel()
                panel.add(w)
                panel.add(HTML("{}".format(i + dimension - add_count + 1),
                    HorizontalAlignment='center'))
                uiarray.add(panel)

    def create_weights_dialog(self, callback, msg):
        contents = VerticalPanel(StyleName="Contents", Spacing=4)
        contents.add(HTML(msg))
        expr = TextBox(Text='0.1')
        contents.add(expr)
        contents.add(Button("OK", callback))
        dialog = DialogBox(glass=True)
        dialog.setHTML('<b>Set the weights from an expression</b>')
        dialog.setWidget(contents)
        return dialog, expr

    def set_product_weights_expr(self, event):
        self._product_weights_expr_dialog.hide()
        self.status.setText("computing weights...")
        id = self.remote.make_product_weights(
                self.product_weights_expr.getText(),
                len(self.product_weights),
                self)

    def set_order_weights_expr(self, event):
        self._order_weights_expr_dialog.hide()
        self.status.setText("computing weights...")
        id = self.remote.make_order_weights(
                self.order_weights_expr.getText(),
                len(self.order_weights),
                self)

    def showDialog(self, dialog):
        left = (Window.getClientWidth() - 200) / 2 + Window.getScrollLeft()
        top = (Window.getClientHeight() - 100) / 2 + Window.getScrollTop()
        dialog.setPopupPosition(left, top)
        dialog.show()

    def onChange(self, sender):

        if sender == self.construction:
            key, name, desc = \
                    self.CONSTRUCTION_METHODS[self.construction.getSelectedIndex()]
            self.construction_desc.setText(desc)
            self.construction_samples_panel.setVisible('{samples}' in key)

        elif sender == self.merit:
            key, name = \
                    self.FIGURES_OF_MERIT[self.merit.getSelectedIndex()]
            self.merit_alpha_panel.setVisible('{alpha}' in key)
            self.merit_cs.setVisible('{cs}' in key)

        elif sender == self.embedded:
            pass

        elif sender == self.dimension:
            # fill weights panels
            dimension = int(self.dimension.getText())
            self.fill_weights(self.product_weights, self.product_weights_array, dimension)
            self.fill_weights(self.order_weights, self.order_weights_array, dimension)

        elif sender == self.weight_type:
            key, name = \
                    self.WEIGHT_TYPES[self.weight_type.getSelectedIndex()]
            self.product_weights_panel.setVisible(key in ['product', 'POD'])
            self.order_weights_panel.setVisible(key in ['order-dependent', 'POD'])


    def onClick(self, sender):
        if sender == self.button_search:
            self.status.setText(self.TEXT_WAITING)

            lattype = self.embedded.getChecked() and 'embedded' or 'ordinary'
            size = self.size.getText()
            dimension = self.dimension.getText()

            norm_type = self.norm_type.getText()
            merit, merit_name = \
                    self.FIGURES_OF_MERIT[self.merit.getSelectedIndex()]
            alpha = self.merit_alpha.getText()
            cs = self.merit_cs.getChecked() and 'CS:' or ''

            
            weight_type, weight_name = \
                    self.WEIGHT_TYPES[self.weight_type.getSelectedIndex()]
            weights = weight_type
            if weight_type in ['order-dependent', 'POD']:
                weights += ':0:' + ','.join(w.getText() for w in self.order_weights)
            if weight_type in ['product', 'POD']:
                weights += ':0:' + ','.join(w.getText() for w in self.product_weights)

            construction, construction_name, desc = \
                    self.CONSTRUCTION_METHODS[self.construction.getSelectedIndex()]
            samples = self.construction_samples.getText()


            id = self.remote.execute(
                    lattype,
                    size,
                    dimension,
                    norm_type,
                    merit.format(alpha=alpha, cs=cs),
                    weights,
                    construction.format(samples=samples),
                    self)

        elif sender == self.product_weights_expr_link:
            self.showDialog(self._product_weights_expr_dialog)

        elif sender == self.order_weights_expr_link:
            self.showDialog(self._order_weights_expr_dialog)

    def onRemoteResponse(self, response, request_info):
        try:
            if request_info.method == 'execute':
                points, gen, merit = eval(response)
                self.results_size.setText(points)
                self.results_gen.setText(', '.join(gen))
                self.results_merit.setText(merit)
                self.results_panel.setVisible(True)
                self.status.setText("")
            elif request_info.method == 'make_product_weights':
                weights = [float(x) for x in response]
                for w, val in zip(self.product_weights, weights):
                    w.setText(str(val))
                self.status.setText("")
            elif request_info.method == 'make_order_weights':
                weights = [float(x) for x in response]
                for w, val in zip(self.order_weights, weights):
                    w.setText(str(val))
                self.status.setText("")
        except:
            self.status.setText(response.replace('\n', '    '))

    def onRemoteError(self, code, errobj, request_info):
        message = errobj['message']
        if code != 0:
            self.status.setText("HTTP error %d: %s" % 
                                (code, message['name']))
        else:
            code = errobj['code']
            self.status.setText("JSONRPC Error %s: %s" %
                                (code, message))


class LatBuilderService(JSONProxy):
    def __init__(self):
        JSONProxy.__init__(self, "services/LatBuilderService.py",
                ['execute', 'make_product_weights', 'make_order_weights'])

if __name__ == '__main__':
    app = LatBuilderWeb()
    app.onModuleLoad()
