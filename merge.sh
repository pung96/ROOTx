#!/bin/bash

fileList="
TxBaseUtil
TxDEBUG
TxRange
TxHelper
TxHnSparse
"
>TxAllInOne.h
echo $fileList | xargs -n1 -I% echo %.h | xargs cat | grep -v "#NotMerge" >> TxAllInOne.h
echo $fileList | xargs -n1 -I% echo %.cxx | xargs cat | grep -v "#NotMerge" >> TxAllInOne.h

