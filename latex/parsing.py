def retrieve_BORWin_result(type, N, D, L, instance_num):
    try:
        with open(f"../results_type_{type}/N={N}_D={D}_L={L}/instance_{instance_num}_BORWin.csv") as f:
            lines = f.readlines()
            objective_value = '{:.2f}'.format(float(lines[0].split(",")[1]))
            cpt_time = float(lines[2].split(",")[1])
            nb_iter = int(lines[3].split(",")[1])
            if cpt_time>=600:
                cpt_time = "TO"
                if objective_value == "0.00":
                    objective_value = "\\text{-}"
            else:
                cpt_time = '{:.1f}'.format(cpt_time)
            return objective_value, cpt_time, nb_iter
    except:
        return "\\text{-}", "TO", 0

def retrieve_ILP_result(type, N, D, L, instance_num):
    try:
        with open(f"../results_type_{type}/N={N}_D={D}_L={L}/instance_{instance_num}_ILP.csv") as f:
            lines = f.readlines()
            status = int(lines[3].split(",")[1])
            match status:
                case -1:
                    nb_nodes = int(lines[5].split(",")[1])
                    return "\\text{-}", "TO", "\\text{-}", nb_nodes
                case 0:
                    objective_value = '{:.2f}'.format(float(lines[0].split(",")[1]))
                    gap = '{:.2f}'.format(float(lines[4].split(",")[1]))
                    nb_nodes = str(int(lines[5].split(",")[1]))
                    return objective_value, "TO", gap, nb_nodes
                case _:
                    objective_value = '{:.2f}'.format(float(lines[0].split(",")[1]))
                    time = '{:.2f}'.format(float(lines[2].split(",")[1]))
                    gap = '{:.2f}'.format(float(lines[4].split(",")[1]))
                    nb_nodes = str(int(lines[5].split(",")[1]))
                    return objective_value, time, gap, nb_nodes
    except:
        return "\\text{-}", "TO", "\\text{-}", 0

def build_table_res(type, N, degrees, nb_instance):
    L=100
    text = ""
    text += "\\begin{table}\n"
    text += "\\begin{center}\n"
    text += "\\begin{tabular}{r|r||S[table-format=7.2]|S[table-format=1.2]|S[table-format=4.1]|r||S[table-format=7.2]|S[table-format=4.1]|r}\n"
    text += "\t & & \\multicolumn{4}{c||}{CPLEX} & \\multicolumn{3}{c}{BORWin} \\\\\n"
    text += "\t\\text{P} & \\text{instance} & \\text{value} & \\text{gap} & \\text{time} & \\text{\\#nodes} & \\text{value} & \\text{time} & \\text{\\#iter} \\\\\n"
    for D in degrees:
        for i in range(nb_instance):
            if i ==0:
                text += f"\t\\hline\n\t\\hline\n" + "\t\\multirow{"+str(nb_instance)+"}{*}{"+str(D)+"} & "
            else:
                text += "\t & "
            text += f"{i+1}"
            obj, cpt_time, gap, nb_nodes = retrieve_ILP_result(type, N, D, L, i+1)
            text += f" & {obj} & {gap} & {cpt_time} & {nb_nodes}"
            obj, cpt_time, nb_iter = retrieve_BORWin_result(type, N, D, L, i+1)
            text += f" & {obj} & {cpt_time} & {nb_iter}\\\\\n"
    text += "\\end{tabular}\n"
    text += "\\end{center}\n"
    text += "\\caption{\AH{Performance of the ILP model and BORWin for instances type "+type+" of the AWCLPP with "+N+" nodes}}\n"
    if type == "A":
        match N:
            case "4000":
                text += "\\label{tab:res_awclpp_a2}\n"
            case "8000":
                text += "\\label{tab:res_awclpp_a3}\n"
            case _:
                text += "\\label{tab:res_awclpp_a1}\n"
    else:
        match N:
            case "4000":
                text += "\\label{tab:res_awclpp_b2}\n"
            case "8000":
                text += "\\label{tab:res_awclpp_b3}\n"
            case _:
                text += "\\label{tab:res_awclpp_b1}\n"
    text += "\\end{table}"
    with open(f"table_AWCLPP_type={type}_N={N}.txt", "w") as f:
        f.write(text)


def build_cactus_plot(type, nb_instance):
    L=100
    BORWin_times = []
    ILP_times = []
    total_nb_instances=  0
    for N in degrees.keys():
        for D in degrees[N]:
            total_nb_instances+=nb_instance
            for i in range(nb_instance):
                obj, cpt_time, nb_iter = retrieve_BORWin_result(type, N, D, L, i+1)
                if cpt_time != "TO":
                    BORWin_times.append(float(cpt_time))
                
                obj, cpt_time, gap, nb_nodes = retrieve_ILP_result(type, N, D, L, i+1)
                if cpt_time != "TO":
                    ILP_times.append(float(cpt_time))
    ILP_times.sort()
    BORWin_times.sort()

    text = "\\begin{tikzpicture}\n"
    text += "\\begin{axis}[\n"
    text +="\txlabel={Time},"
    text +="\tylabel={\#instances solved},"
    text +="\txmin=0, xmax=600,\n"
    text +=f"\tymin=0, ymax={total_nb_instances},\n"
    text +="\txmode=log,\n"
    text +="\tlegend pos=north west]\n"

    text +="\t\\addplot[\n"
    text +="\t\tcolor=blue,\n"
    text +="\t\tmark=o,\n"
    text +="\t]\n"
    text +="\tcoordinates {\n"
    text +="\t\t"
    for i in range(len(ILP_times)):
        text+=f"({ILP_times[i]}, {i})({ILP_times[i]}, {i+1})"
    text +=f"(600, {len(ILP_times)})\n"
    text+="\t};\n"
    
    text +="\t\\addplot[\n"
    text +="\t\tcolor=green,\n"
    text +="\t\tmark=triangle,\n"
    text +="\t]\n"
    text +="\tcoordinates {\n"
    text +="\t\t"
    for i in range(len(BORWin_times)):
        text+=f"({BORWin_times[i]}, {i})({BORWin_times[i]}, {i+1})"
    text +=f"(600, {len(BORWin_times)})\n"
    text+="\t};\n"
    text+="\t\\legend{CPLEX, BORWin}\n"

    text +="\\end{axis}\n"
    text +="\\end{tikzpicture}"
    with open(f"figure_AWCLPP_type{type}.txt", "w") as f:
        f.write(text)

for type in ["A", "C"]:
    degrees = {"2000" : ["0.01", "0.1", "0.25", "0.5"],
            "4000" : ["0.01", "0.1", "0.25", "0.5"],
            "8000" : ["0.01", "0.1", "0.25"]}

    for N in degrees:
        build_table_res(type, N, degrees[N], 10)
    build_cactus_plot(type, 10)