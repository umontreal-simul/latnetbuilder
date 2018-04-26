FROM pierremarion23/latbuilder:dep

RUN cd $HOME
RUN git clone -b polynomial --single-branch --recursive https://github.com/PierreMarion23/latbuilder
RUN cd $HOME/latbuilder && ls && ./waf configure --prefix $HOME/latsoft --boost $HOME/dependencies --fftw $HOME/dependencies --ntl $HOME/dependencies && ./waf build && ./waf install

