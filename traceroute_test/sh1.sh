#!/bin/bash
for i in {1..100}
do
   traceroute -q 1 130.104.230.45 > route$i.txt
done
