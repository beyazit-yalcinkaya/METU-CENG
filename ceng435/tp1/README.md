<!-- Sina Sehlaver, 2099729 -->
<!-- Beyazit Yalcinkaya, 2172138 -->


# CENG 435 - Data Communications and Networking Term Project - Part I

## Requirements

1. Python3
2. Python3 Module socket
3. Python3 Module threading
4. Python3 Module time
5. Python3 Module subprocess
6. Python3 Module numpy
7. Python3 Module scipy
8. Python3 Module sys
9. Linux Command ntpdate
10. Linux Command tc/netem


## Project Structure

Below, we first give the file structure of the project. `configurationScripts` folder contains all configuration scripts that applies all `tc/netem` commands. `discoveryScripts` folder contains all scripts for discoverying the RTTs of edges between nodes. `experimentScripts` folder contains all the scripts for experiments 1, 2, and 3.

```
TP_Part1_16
│   README.md
│   tr2_request_rspec.xml    
│
└───configurationScripts
│   │   configExp1R3.sh
│   │   configExp1SD.sh
│   │   configExp2R3.sh
│   │   configExp2SD.sh
│   │   configExp3R3.sh
│   │   configExp3SD.sh
│   │   configureR1.sh
│   │   configureR2.sh
│   │   resetConfigureR1.sh
│   │   resetConfigureR2.sh
│   │   resetConfigureR3.sh
│   │   resetConfigureSD.sh
│   │
│   
└───discoveryScripts
│   │   link_costs_merged.txt
│   │
│   └───s
│   │   │   main.py
│   │   │   start.sh
│   │
│   └───r1
│   │   │   main.py
│   │   │   start.sh
│   │   │   link_costs.txt
│   │
│   └───r2
│   │   │   main.py
│   │   │   start.sh
│   │   │   link_costs.txt
│   │
│   └───r3
│   │   │   main.py
│   │   │   start.sh
│   │   │   link_costs.txt
│   │
│   └───d
│   │   │   main.py
│   │   │   start.sh
│   │
│
└───experimentScripts
│   │   exps.csv
│   │   merge_and_calculate.py
│   │
│   └───s
│   │   │   main.py
│   │   │   start.sh
│   │   │   exp1.txt
│   │   │   exp2.txt
│   │   │   exp3.txt
│   │
│   └───r3
│   │   │   main.py
│   │   │   start.sh
│   │
│   └───d
│   │   │   main.py
│   │   │   start.sh
│   │
│
```

## How to Run

Below, we specify how to run scripts to find link costs and experiment.

### Finding Link Costs


1. Copy TP\_Part1\_16 to each machine in `~/` directory, you can also copy only the corresponding folder, i.e., for s, you do not need to copy folders named r1, r2, r3, and d.
2. Run `cd ~/TP_Part1_16`
3. From r1, run `bash configurationScripts/configureR1.sh`
4. From r2, run `bash configurationScripts/configureR2.sh`
5. From s, run `bash discoveryScripts/s/start.sh`
6. From d, run `bash discoveryScripts/d/start.sh`
7. From r1, run `bash discoveryScripts/r1/start.sh 1000` for 1000 samples.
8. From r3, run `bash discoveryScripts/r3/start.sh 1000` for 1000 samples.
9. The order of steps 5, 6, 7, and 8 are not important.
10. From r2, run `bash discoveryScripts/r2/start.sh 1000` for 1000 samples. This step should be run last because r2 initiates all communication among nodes.

After following these steps, `discoveryScripts/r1/link_costs.txt` contains costs for s-r1 and d-r1; `discoveryScripts/r2/link_costs.txt` contains costs for s-r2, r1-r2, r3-r2, and d-r2; and `discoveryScripts/r3/link_costs.txt` contains costs for s-r3 and d-r3.

In order to merge all results run the following command.

```
cat discoveryScripts/r1/link_costs.txt discoveryScripts/r2/link_costs.txt discoveryScripts/r3/link_costs.txt > discoveryScripts/link_costs_merged.txt
```

All link costs are listed in `discoveryScripts/link_costs_merged.txt`.




### Experiment

#### Experiment 1

For experiment 1, do the following steps.

1. Copy TP\_Part1\_16 to each machine in `~/` directory, you can also copy only the corresponding folder, i.e., for s, you do not need to copy folders named r1, r2, r3, and d.
2. Run `cd ~/TP_Part1_16 `
3. From r3, run `bash configurationScripts/resetConfigureR3`
4. From r3, run `bash configurationScripts/configExp1R3`
5. From s, run `bash configurationScripts/resetConfigureSD`
6. From s, run `bash configurationScripts/configExp1SD`
7. From d, run `bash configurationScripts/resetConfigureSD`
8. From d, run `bash configurationScripts/configExp1SD`
9. From r3, run `bash experimentScripts/r3/start.sh`
10. From d, run `bash experimentScripts/d/start.sh`
11. The order of steps 9 and 10 are not important.
12. From s, run `bash experimentScripts/s/start.sh 1000 exp1` for 1000 samples. This step should be run last because s initiates all communication amoung nodes.

After following these steps, `experimentScripts/s/exp1.txt` contains number of samples, mean, and standard deviation.

#### Experiment 2

For experiment 1, do the following steps.

1. Copy TP\_Part1\_16 to each machine in `~/` directory, you can also copy only the corresponding folder, i.e., for s, you do not need to copy folders named r1, r2, r3, and d.
2. Run `cd ~/TP_Part1_16`
3. From r3, run `bash configurationScripts/resetConfigureR3`
4. From r3, run `bash configurationScripts/configExp2R3`
5. From s, run `bash configurationScripts/resetConfigureSD`
6. From s, run `bash configurationScripts/configExp2SD`
7. From d, run `bash configurationScripts/resetConfigureSD`
8. From d, run `bash configurationScripts/configExp2SD`
9. From r3, run `bash experimentScripts/r3/start.sh`
10. From d, run `bash experimentScripts/d/start.sh`
11. The order of steps 9 and 10 are not important.
12. From s, run `bash experimentScripts/s/start.sh 1000 exp2` for 1000 samples. This step should be run last because s initiates all communication amoung nodes.

After following these steps, `experimentScripts/s/exp2.txt` contains number of samples, mean, and standard deviation.

#### Experiment 3

For experiment 1, do the following steps.

1. Copy TP\_Part1\_16 to each machine in `~/` directory, you can also copy only the corresponding folder, i.e., for s, you do not need to copy folders named r1, r2, r3, and d.
2. Run `cd ~/TP_Part1_16`
3. From r3, run `bash configurationScripts/resetConfigureR3`
4. From r3, run `bash configurationScripts/configExp3R3`
5. From s, run `bash configurationScripts/resetConfigureSD`
6. From s, run `bash configurationScripts/configExp3SD`
7. From d, run `bash configurationScripts/resetConfigureSD`
8. From d, run `bash configurationScripts/configExp3SD`
9. From r3, run `bash experimentScripts/r3/start.sh`
10. From d, run `bash experimentScripts/d/start.sh`
11. The order of steps 9 and 10 are not important.
12. From s, run `bash experimentScripts/s/start.sh 1000 exp3` for 1000 samples. This step should be run last because s initiates all communication amoung nodes.

After following these steps, `experimentScripts/s/exp3.txt` contains number of samples, mean, and standard deviation.


In order to merge all results run the following command.

```
python3 experimentScripts/merge_and_calculate.py > experimentScripts/exps.csv
```

All experiment results with calculated margin of error, lower, and upper bounds are listed in `experimentScripts/exps.csv`. This csv file can be opened in Excel or a similar tool to generate needed graphs.