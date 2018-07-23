import ipywidgets as widgets

from .common import style_default

def output_folder():
    return widgets.VBox([
        widgets.HTML(value="Optional <b>path to the folder for the outputs of LatNeBuilder</b>. The contents of the folder may be overwritten. If the folder does not exist, it is created. If no path is provided, no output folder is created.",
            style = style_default),
        widgets.Text(placeholder='e.g. output or /home/output',
            description = 'Path:', 
            style = style_default,
            layout = widgets.Layout(width='500px'))])