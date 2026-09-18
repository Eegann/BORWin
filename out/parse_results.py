import csv
import statistics
from pathlib import Path

nb_inst_per_type = 8

val_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
res_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
timef_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
times_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
time_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
it_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
val_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
res_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
time_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
opt_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]
gap_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(3)]

#stats
mean_time_b = [[0 for _ in range(nb_inst_per_type)] for _ in range(3)]
mean_time_i = [[0 for _ in range(nb_inst_per_type)] for _ in range(3)]
std_dev_b = [[0 for _ in range(nb_inst_per_type)] for _ in range(3)]
std_dev_i = [[0 for _ in range(nb_inst_per_type)] for _ in range(3)]

ind_t=0
for t in ['type_A','type_B','type_C']:
    ind_p=0
    for p in ['N=2000_D=0.01_L=50', 'N=2000_D=0.01_L=100', 'N=2000_D=0.01_L=200', 'N=2000_D=0.1_L=100', 'N=2000_D=0.5_L=100', 'N=4000_D=0.01_L=100', 'N=4000_D=0.5_L=100', 'N=8000_D=0.01_L=100']:
        for i in range (1,11):
            s = './'+t+'/'+p+'/instance_'+str(i)+'_BORWin.csv'
            #print('parsing '+s)
            my_file = Path(s)
            if not my_file.is_file():
                print(s+' BORWIN: no solution found')
                val_b[ind_t][ind_p].append(0.0)
                res_b[ind_t][ind_p].append(0.0)
                time_b[ind_t][ind_p].append(1000.0)
                timef_b[ind_t][ind_p].append(1000.0)
                times_b[ind_t][ind_p].append(1000.0)
            else:
                with open(s, mode='r', encoding='utf-8', newline='') as file:
                    reader = csv.reader(file)
                    row=next(reader)
                    val_b[ind_t][ind_p].append(float(row[1]))
                    row=next(reader)
                    res_b[ind_t][ind_p].append(float(row[1]))
                    row=next(reader)
                    time_b[ind_t][ind_p].append(float(row[1]))
                    timef_b[ind_t][ind_p].append(float(row[3]))
                    times_b[ind_t][ind_p].append(float(row[5]))
                    row=next(reader)
                    it_b[ind_t][ind_p].append(int(row[1]))

            s = './'+t+'/'+p+'/instance_'+str(i)+'_ILP.csv'
            #print('parsing '+s)
            my_file = Path(s)
            if not my_file.is_file():
                print(s+' ILP: no solution found')
                val_i[ind_t][ind_p].append(0.0)
                res_i[ind_t][ind_p].append(0.0)
                time_i[ind_t][ind_p].append(1000.0)
                opt_i[ind_t][ind_p].append(0)
                gap_i[ind_t][ind_p].append(1000.0)
            else:
                with open(s, mode='r', encoding='utf-8', newline='') as file:
                    reader = csv.reader(file)
                    row=next(reader)
                    val_i[ind_t][ind_p].append(float(row[1]))
                    row=next(reader)
                    res_i[ind_t][ind_p].append(float(row[1]))
                    row=next(reader)
                    time_i[ind_t][ind_p].append(float(row[1]))
                    row=next(reader)
                    opt_i[ind_t][ind_p].append(int(row[1]))
                    row=next(reader)
                    gap_i[ind_t][ind_p].append(float(row[1]))
        mean_time_b[ind_t][ind_p]=statistics.mean(time_b[ind_t][ind_p])
        mean_time_i[ind_t][ind_p]=statistics.mean(time_i[ind_t][ind_p])
        std_dev_b[ind_t][ind_p]=statistics.stdev(time_b[ind_t][ind_p])
        std_dev_i[ind_t][ind_p]=statistics.stdev(time_i[ind_t][ind_p])
#        print(t+' '+p+' BORWin: mean time = '+str(mean_time_b[ind_t][ind_p])+' std dev = '+str(std_dev_b[ind_t][ind_p]))
#        print(t+' '+p+' ILP: mean time = '+str(mean_time_i[ind_t][ind_p])+' std dev = '+str(std_dev_i[ind_t][ind_p]))
        print(t+' '+p+f' BORWin: mean time = {mean_time_b[ind_t][ind_p]:2f} std dev = '+str(std_dev_b[ind_t][ind_p]))
        print(t+' '+p+' ILP: mean time = '+str(mean_time_i[ind_t][ind_p])+' std dev = '+str(std_dev_i[ind_t][ind_p]))
        
        ind_p+=1
    ind_t+=1

#output stats


#Objective value:,7121.62
#Resource used:,1838.86
#Time:,9.316, First phase:,0.049, Second phase:,9.267
#Iterations:,577
#Path:,(0),(1990),(978),(1231),(589),(1883),(756),(411),(1402),(45),(103),(1999)

#Objective value:,2884.33
#Resource used:,757.348
#Time:,0.375
#Optim:,1
#gap:,0
#Path:,(0),(435),(1177),(968),(636),(1999)

