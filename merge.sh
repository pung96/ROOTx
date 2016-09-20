#!/bin/bash

fileList="
TxBaseUtil
TxDEBUG
TxRange
TxHelper
TxHnSparse
"
>src/TxAllInOne.h
echo $fileList | xargs -n1 -I% echo src/%.h | xargs cat | grep -v "#NotMerge" >> src/TxAllInOne.h
echo $fileList | xargs -n1 -I% echo src/%.cxx | xargs cat | grep -v "#NotMerge" >> src/TxAllInOne.h

