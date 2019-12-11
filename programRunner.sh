#!/bin/bash
cd 'PL_final_project'
g++ -I './' -o smurfParser *.cpp -pthread
cd '../test_cases'
python3 '../test_runner.py' '../PL_final_project/smurfParser'
