"""
RunSim.py by Sabrina Mosher (ykf217)
Run the simulation 
"""
import subprocess
import numpy as np
import os
import itertools
import time
import sys

def get_input_args(trace_file, cache_size, replacement_type, block_size, associativity):
    """ Get the input arguments for subprocess """
    input_str = sys.argv[1] + " -f " + trace_file + \
                " -s " + str(cache_size) + \
                " -r " + str(replacement_type) + \
                " -b " + str(block_size) + \
                " -a " + str(associativity) 
    return input_str.split()

def parse_output(output):
    """ Parse the output from subprocess """
    output_l = [x for x in 
                str(output)[2:-1].replace('\\r','').replace("\\\\", '\\').split("\\n")
                if x]
    
    res_dict = {}
    res_dict['Total # blocks'] = 2**(int(output_l[7].split('^')[1][:-1]))
    res_dict['Tag Size (bits)'] = int(output_l[8].split(':')[1].split()[0])
    res_dict['Index Size (bits)'] = int(output_l[9].split(':')[1].split()[0])
    res_dict['Overhead Memory Size (bytes)'] = int(output_l[10].split(':')[1].split()[0])
    res_dict['Implementation Memory Size (bytes)'] = int(output_l[11].split(':')[1].split()[0])
    res_dict['Total Cache Accesses'] = int(output_l[12].split(':')[1].split()[0])
    res_dict['Cache Hits'] = int(output_l[13].split(':')[1].split()[0])
    res_dict['Cache Misses'] = int(output_l[14].split(':')[1].split()[0])
    res_dict['Compulsory Cache Misses'] = int(output_l[15].split(':')[1].split()[0])
    res_dict['Conflict Misses'] = int(output_l[16].split(':')[1].split()[0])
    res_dict['Cache Miss Rate (%)'] = float(output_l[17].split(':')[1].split()[0])
    
    return res_dict

def write_result(f, base_parms, res_dict, keys):
    """ Write the result of the process to a file """
    f.write(base_parms[0])
    
    # input parameters to function call
    for i in range(1, len(base_parms)):
        f.write("," + str(base_parms[i]))
        
    # output results
    for key in keys:
        f.write("," + str(res_dict[key]))
    f.write("\n")
    

if len(sys.argv) < 3:
    print("Error: Usage RunSim.py [executableName] [outputFile] <cache size power>")
    sys.exit(-1)
    
base_trace_dir = "Trace"
tr_all = []
for file in os.listdir(base_trace_dir):
    tr_all += [os.path.join(base_trace_dir, file)]
    

# set base values
cache_size = list(2**(np.arange(14) if len(sys.argv) == 3 else np.array([int(sys.argv[3])])))
replacement_type = ["RR", "LRU", "RND"]
block_size = list(2**np.arange(2,7))
associativity = list(2**np.arange(5))

# get all combinations
input_parms = list(itertools.product(tr_all, 
                                     cache_size, 
                                     replacement_type, 
                                     block_size, 
                                     associativity))


with open(sys.argv[2], "w") as f:
    f.write("Trace file,Cache Size,Replacement Type,Block Size,Associativity,")
    f.write("Total # blocks,Tag Size (bits),Index Size (bits),Overhead Memory Size (bytes),")
    f.write("Implementation Memory Size (bytes),Total Cache Accesses,Cache Hits,Cache Misses,")
    f.write("Compulsory Cache Misses,Conflict Misses,Cache Miss Rate (%)\n")

    keys_str = "Total # blocks,Tag Size (bits),Index Size (bits),Overhead Memory Size (bytes)," +        "Implementation Memory Size (bytes),Total Cache Accesses,Cache Hits,Cache Misses," +        "Compulsory Cache Misses,Conflict Misses,Cache Miss Rate (%)"
    keys = keys_str.split(',')


    # Iterate through all the rows and check the output. 
    values_frame = None
    j = 0
    start_time = time.time()
    input_parms[4]
    for i in input_parms:
        try:
            
            res = parse_output(subprocess.check_output(get_input_args(*i)))
            write_result(f, i, res, keys)
            j += 1
            if  j  % 10 == 0:
                print("Iteration:", j)
                print("Minutes so far:", (time.time()-start_time)/60.0)
                print()
        except Exception as e:
            print("Issue with input parms:", i)
            print()