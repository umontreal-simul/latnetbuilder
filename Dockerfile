FROM pierremarion23/latbuilder:dep

RUN cd $HOME
RUN ./waf configure --prefix $HOME/latsoft --boost $HOME/dependencies --fftw $HOME/dependencies --ntl $HOME/dependencies
RUN ./waf build
RUN ./waf install
