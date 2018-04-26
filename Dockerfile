FROM pierremarion23/latbuilder:dep

RUN jupyter nbextension enable --py --sys-prefix widgetsnbextension
RUN pip install bqplot==0.10.2
RUN pip install numexpr
RUN pip install wheel
RUN pip install appmode
RUN jupyter nbextension enable --py --sys-prefix appmode
RUN jupyter serverextension enable --py --sys-prefix appmode
RUN jupyter nbextension install --py --sys-prefix bqplot
RUN jupyter nbextension enable --py --sys-prefix bqplot

RUN git clone -b polynomial --single-branch --recursive https://github.com/PierreMarion23/latbuilder
RUN cd $HOME/latbuilder && ls && ./waf configure --prefix $HOME/latsoft --boost $HOME/dependencies --fftw $HOME/dependencies --ntl $HOME/dependencies && ./waf build && ./waf install
