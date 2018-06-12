import ipywidgets as widgets

from .common import style_default, BaseGUIElement, trigger_display

def filters():
    is_normalization = widgets.Checkbox(description='Normalization', value=False, style=style_default)

    low_pass_filter = widgets.Checkbox(description='Low-Pass Filter', value=False, style=style_default)

    equidistribution_filter = widgets.Checkbox(description='k-bits Equidistribution', value=False, style=style_default, layout=widgets.Layout(display='none'))

    normalization_options = widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:',
                        layout=widgets.Layout(width='30%'), style=style_default)
    normalization_warning = widgets.HTML('<span style="color:red"> Warning: SL10 Palpha and DPW08 Palpha only work with Palpha figure of merit.</span>\
                                        <br><span style="color:red"> DPW08 only works for product weights.</span>')
    normalization_box = widgets.HBox([normalization_options, normalization_warning], layout=widgets.Layout(display='none'))

    low_pass_filter_options = widgets.Text(value='1', description='Low-Pass Threshold',
                                        layout=widgets.Layout(display='none', width='25%'), style=style_default)

    equidistribution_options = widgets.BoundedIntText(value=1, min=1, 
                        description='k-bits Equidistribution - set k:',
                        layout=widgets.Layout(width='230px'), style=style_default)
    equidistribution_weight = widgets.Text(value='inf',
        description="Set weight w>0 or 'inf':", layout=widgets.Layout(width='200px'), style=style_default)
    equidistribution_box = widgets.HBox([equidistribution_options, equidistribution_weight], layout=widgets.Layout(display='none', justify_content='space-around'))

    filters_wrapper = widgets.Accordion([widgets.VBox([widgets.HBox([is_normalization, low_pass_filter, equidistribution_filter], layout=widgets.Layout(justify_content='space-around')),
                                            normalization_box, low_pass_filter_options, equidistribution_box])])
    filters_wrapper.set_title(0, 'Filters (optional)')
    filters_wrapper.selected_index = None

    return BaseGUIElement(is_normalization=is_normalization,
                          low_pass_filter=low_pass_filter,
                          equidistribution_filter=equidistribution_filter,
                          normalization_options=normalization_options,
                          normalization_warning=normalization_warning,
                          normalization_box=normalization_box,
                          low_pass_filter_options=low_pass_filter_options,
                          equidistribution_box=equidistribution_box,
                          equidistribution_weight=equidistribution_weight,
                          equidistribution_options=equidistribution_options,
                          main=filters_wrapper,
                          _callbacks={'is_normalization': trigger_display,
                                      'low_pass_filter': trigger_display,
                                      'equidistribution_filter': trigger_display})
