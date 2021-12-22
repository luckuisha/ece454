make clean
cd ..
tar -cvvf gol.tar src/
gzip --force gol.tar
submitece454f 5 gol.tar.gz report.txt
cd src