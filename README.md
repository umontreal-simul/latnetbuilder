# Updating LatNet Builder documentation

 To update the documentation of *LatNet Builder* , run the following commands on your clone of the LatNet Builder repository:

 ```
 ./waf configure --build-docs
 ./waf build
 rm -r -f doc/html
 git clone https://github.com/umontreal-simul/latnetbuilder.git doc/html
 cd doc/html
 git checkout -b gh-pages origin/gh-pages
 rm -r !('README.md')
 cp -r ../../build/doc/html/* .
 git add .   
 git status   
 git commit -m "update documentation"
 git push
 ```

 These commands should compile the source code and the documentation, commit and push the documentation changes to the branch `gh-pages` of *LatNet Builder*.
