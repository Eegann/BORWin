import csv
import statistics
from pathlib import Path

nb_inst_per_type = 13
seuil = 600

val_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
res_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
timef_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
times_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
time_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
it_b = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
val_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
res_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
time_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
opt_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
gap_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]
node_i = [[[] for _ in range(nb_inst_per_type)] for _ in range(1)]

#stats
mean_time_b = [[0 for _ in range(nb_inst_per_type)] for _ in range(1)]
mean_time_i = [[0 for _ in range(nb_inst_per_type)] for _ in range(1)]
std_dev_b = [[0 for _ in range(nb_inst_per_type)] for _ in range(1)]
std_dev_i = [[0 for _ in range(nb_inst_per_type)] for _ in range(1)]
mean_node_i = [[0 for _ in range(nb_inst_per_type)] for _ in range(1)]
mean_it_b = [[0 for _ in range(nb_inst_per_type)] for _ in range(1)]

ind_t=0
for t in ['type_A']:
    #print ("t="+str(ind_t))
    ind_p=0
    for p in ['N=2000_D=0.01_L=50', 'N=2000_D=0.01_L=100', 'N=2000_D=0.01_L=200', 'N=2000_D=0.1_L=100', 'N=2000_D=0.25_L=100', 'N=2000_D=0.5_L=100', 'N=4000_D=0.01_L=100', 'N=4000_D=0.1_L=100', 'N=4000_D=0.25_L=100', 'N=4000_D=0.5_L=100', 'N=8000_D=0.01_L=100', 'N=8000_D=0.1_L=100', 'N=8000_D=0.25_L=100']:
        #print("p="+str(ind_p))
        for i in range (1,11):
            s = './'+t+'/'+p+'/instance_'+str(i)+'_BORWin.csv'
            #print('parsing '+s)
            my_file = Path(s)
            if not my_file.is_file():
#                print(s+' BORWIN: no solution found')
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
#                print(s+' ILP: no solution found')
                val_i[ind_t][ind_p].append(0.0)
                res_i[ind_t][ind_p].append(0.0)
                time_i[ind_t][ind_p].append(1000.0)
                opt_i[ind_t][ind_p].append(-1)
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
                    row=next(reader)
                    node_i[ind_t][ind_p].append(float(row[1]))
        filtre_time_opt_b = [x for x in time_b[ind_t][ind_p] if x < seuil]
        filtre_time_opt_i =  [x for x in time_i[ind_t][ind_p] if x < seuil]
        filtre_node_opt_i = [a for a, b in zip(node_i[ind_t][ind_p], time_i[ind_t][ind_p]) if b < seuil]
        filtre_it_opt_b = [a for a, b in zip(it_b[ind_t][ind_p], time_b[ind_t][ind_p]) if b < seuil]

        if filtre_time_opt_b:
            mean_time_b[ind_t][ind_p]=statistics.mean(filtre_time_opt_b)
            std_dev_b[ind_t][ind_p]=statistics.stdev(filtre_time_opt_b)
            mean_it_b[ind_t][ind_p]=statistics.mean(filtre_it_opt_b)
        else:
            mean_time_b[ind_t][ind_p]=-1
            std_dev_b[ind_t][ind_p]-1
            mean_it_b[ind_t][ind_p]=-1
        if filtre_time_opt_i:
            mean_time_i[ind_t][ind_p]=statistics.mean(filtre_time_opt_i)
            mean_node_i[ind_t][ind_p]=statistics.mean(filtre_node_opt_i)
            std_dev_i[ind_t][ind_p]=statistics.stdev(filtre_time_opt_i)
        else:
            mean_time_i[ind_t][ind_p]=-1
            std_dev_i[ind_t][ind_p]-1
            mean_node_i[ind_t][ind_p]=-1
#        print(t+' '+p+' BORWin: mean time = '+str(mean_time_b[ind_t][ind_p])+' std dev = '+str(std_dev_b[ind_t][ind_p]))
#        print(t+' '+p+' ILP: mean time = '+str(mean_time_i[ind_t][ind_p])+' std dev = '+str(std_dev_i[ind_t][ind_p]))
        print(t+' '+p+f' BORWin: nb opt = {len(filtre_time_opt_b)}, mean time = {mean_time_b[ind_t][ind_p]:2f}, std dev = {std_dev_b[ind_t][ind_p]:2f}, av it = {mean_it_b[ind_t][ind_p]}')
        print(t+' '+p+f' ILP: nb opt = {len(filtre_time_opt_i)}, mean time = {mean_time_i[ind_t][ind_p]:2f}, std dev = {std_dev_i[ind_t][ind_p]:2f}, av nodes = {mean_node_i[ind_t][ind_p]}')
        
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

