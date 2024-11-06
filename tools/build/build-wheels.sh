#!/bin/bash
set -ex

PYVERSION=${1?"Error: expected the python version"}

# We are in the image, building!
PYVERCL=${PYVERSION//./}
# Bins located in paths similar to /opt/python/cp38-cp38/bin
PYBIN="/opt/python/cp${PYVERCL}-cp${PYVERCL}/bin"
echo "Using python path $PYBIN"

yum install -y boost-devel eigen3-devel

cd /io

PATH=${PYBIN}:$PATH
"${PYBIN}/pip" install -U "setuptools>=42" wheel ninja "cmake>=3.12"
"${PYBIN}/python" setup.py bdist_wheel

WHEELS=$(echo dist/*.whl)

for whl in $WHEELS; do
    auditwheel repair "$whl" -w dist/
done

rm $WHEELS