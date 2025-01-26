import argparse
import xlsxwriter
import csv
import os
from fnmatch import fnmatch
from natsort import os_sorted
import ntpath

################################################# Script arguments ##################################################

parser = argparse.ArgumentParser()
parser.add_argument("--data_path", default="Tests", type=str, help="Relative path to data folder.")
parser.add_argument("--data_ending_pattern", default="*results.txt", type=str, help="Matching pattern of endings of files containing data. Example: *results.txt")

def main(args: argparse.Namespace):

    ############################################### Workbook creation ###############################################

    # Create a workbook and add a worksheet.
    workbook = xlsxwriter.Workbook('Results.xlsx')
    results_sheet = workbook.add_worksheet("Results")

    ################################################ Header creation ################################################

    # Format B header.
    B_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "valign": "vcenter",
            "fg_color": "#FF0000",
        }
    )

    # Format C header.
    C_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#FFC000",
        }
    )

    # Format M header.
    M_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#00B050",
        }
    )

    # Format P header.
    P_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#0059FF",
        }
    )

    # Format Common header.
    Common_information_black_format = workbook.add_format(
        {
            "fg_color": "#FFFFFF",
        }
    )

    Biaset_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#FFFF00",
        }
    )

    Random_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#92D050",
        }
    )

    WCross_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#7030A0",
        }
    )

    XCross_format = workbook.add_format(
        {
            "bold": 1,
            "border": 1,
            "align": "center",
            "fg_color": "#00B0F0",
        }
    )

    Rec_format = workbook.add_format(
    {
        "bold": 1,
        "border": 1,
        "align": "center",
        "fg_color": "#FFFFFF",
    }
    )

    subheader_format = workbook.add_format(
        {
            "border": 1,
            "align": "center",
            "fg_color": "#83A5EC",
        }
    )

    different_lb_optimal_format = workbook.add_format(
        {
            "bold": 1,
            "fg_color": "#FFFF00",
        }
    )

    bottom_line_format = workbook.add_format(
        {
            'bottom': 5,
        }
    )

    right_line_format = workbook.add_format(
        {
            'right': 1,
        }
    )

    baseline_algorithm_name = "B"
    algorithms_names = ["C", "M", "P"]
    algorithms_formats = [C_format, M_format, P_format]
    optimal_algorithms = ["B", "P"]
    path_finder_names = ["Biaset", "Random", "WithoutCrossing", "WithoutCrossingAtSameTimes", "RecursivePaths"]
    path_finder_names_short = ["Bia", "Ran", "WCr", "XCr", "Rec"]
    path_finder_formats = [Biaset_format, Random_format, WCross_format, XCross_format, Rec_format]
    common_alg_subheader_data = ["V", "S-mks", "P-time-ms", "S-time-ms", "SO"]
    common_map_information = ["map_name", "size", "Scenario", "Agents", "LB", "OS"]

    common_info_and_baseline_header_offset = len(common_map_information) + len(common_alg_subheader_data)

    relative_algorithms_offsets = {}
    relative_algorithms_offsets[baseline_algorithm_name] = len(common_map_information)
    for i, algorithm in enumerate(algorithms_names):
        relative_algorithms_offsets[algorithm] = common_info_and_baseline_header_offset + i*len(common_alg_subheader_data)*len(path_finder_names)

    for i, algorithm in enumerate(path_finder_names):
        relative_algorithms_offsets[algorithm] = i*(len(common_alg_subheader_data))

    # Common information and Algorithms headers creation.
    results_sheet.merge_range(0, 0, 0, len(common_map_information) - 1, "", Common_information_black_format)
    results_sheet.merge_range(1, 0, 1, len(common_map_information) - 1, "", Common_information_black_format)

    
    results_sheet.set_column(len(common_map_information) - 1, len(common_map_information) - 1, None, right_line_format)

    # Baseline header creation.
    results_sheet.merge_range(0, len(common_map_information), 1, common_info_and_baseline_header_offset - 1, baseline_algorithm_name, B_format)
    for i in range(len(common_alg_subheader_data)):
        results_sheet.write(2, i + len(common_map_information), common_alg_subheader_data[i], subheader_format)

    # Other algorithms header creation.
    for i in range(len(algorithms_names)):
        if i == 0:
            results_sheet.merge_range(0, common_info_and_baseline_header_offset, 0, common_info_and_baseline_header_offset - 1 + len(path_finder_names)*len(common_alg_subheader_data), algorithms_names[i], algorithms_formats[i])
        else:
            results_sheet.merge_range(0, common_info_and_baseline_header_offset + i*len(path_finder_names)*len(common_alg_subheader_data), 0, common_info_and_baseline_header_offset - 1 + (i+1)*len(path_finder_names)*len(common_alg_subheader_data), algorithms_names[i], algorithms_formats[i])


    # Common information headers creation.
    for i in range(len(common_map_information)):
        results_sheet.write(2, i, common_map_information[i], subheader_format)

    # Common algorithms subheaders creation.
    for i in range(len(algorithms_names)*len(path_finder_names)):
        for j in range(len(common_alg_subheader_data)):
            # Pathfinder algorithms subheader creation.
            if j == 0:
                results_sheet.merge_range(1, common_info_and_baseline_header_offset + i*len(common_alg_subheader_data), 1, common_info_and_baseline_header_offset + i*len(common_alg_subheader_data) + len(common_alg_subheader_data) - 1, path_finder_names_short[i % len(path_finder_names)], path_finder_formats[i % len(path_finder_formats)])
            if i == 0:
                results_sheet.write(2, common_info_and_baseline_header_offset + j, common_alg_subheader_data[j], subheader_format)
            else:
                results_sheet.write(2, common_info_and_baseline_header_offset + i*len(common_alg_subheader_data) + j, common_alg_subheader_data[j], subheader_format)
            

    # Freeze two header rows and common_map_information from mooving while scrolling
    results_sheet.freeze_panes(3, len(common_map_information))


    ################################################ Data insertion #################################################

    # Get all paths to all files containing results
    root = os.getcwd() + "\\" + args.data_path
    results_pattern = args.data_ending_pattern
    map_pattern = "*.map"
    paths_data_files = []
    maps_and_scenarios = {}
    for _, subdirs, _ in os.walk(root):
        for dir in os_sorted(subdirs):
            for path, _, files in os.walk(root + "\\" + dir):
                for name in files:
                    if fnmatch(name, results_pattern):
                        print(os.path.join(path, name))

                        paths_data_files.append(os.path.join(path, name))
                    if fnmatch(name, map_pattern):
                        maps_and_scenarios[name] = []
                        for _, _, scen_files in os.walk(root + "\\" + dir + "\\scen-even"):
                            for scen_file in os_sorted(scen_files):
                                maps_and_scenarios[name].append(scen_file)


    data = []

    for path_to_file in paths_data_files:
        with open(path_to_file, newline = '') as res:                                                                                          
            res = csv.reader(res, delimiter='\t')
            splitted_name = ntpath.basename(path_to_file).split('_')
            for result in res:
                print([splitted_name[0], splitted_name[1]] + result)
                data.append([splitted_name[0], splitted_name[1]] + result)


    # Printing data to result sheet.
    printable_data_indexes = [4, 7, 8, 10]
    row = 3
    for map in maps_and_scenarios:
        for scenario in maps_and_scenarios[map]:
            map_scenario_data = [x for x in data if x[2] == map and x[3] == scenario]
            all_agents = sorted(list(set([int(x[5]) for x in map_scenario_data])))
            for agents in all_agents:

                optimal_solution = None
                lower_bound = None

                # Write baseline.
                baseline_results = [x for x in map_scenario_data if x[0] == baseline_algorithm_name and int(x[5]) == agents][:1:]
                if baseline_results:
                    baseline_printable_data = [int(baseline_results[0][index]) for index in printable_data_indexes]
                    optimal_solution = baseline_printable_data[1]
                    lower_bound = int(baseline_results[0][6])
                    for i, data_to_print in enumerate(baseline_printable_data):
                        results_sheet.write(row, relative_algorithms_offsets[baseline_algorithm_name] + i, data_to_print)
                    results_sheet.write(row, relative_algorithms_offsets[baseline_algorithm_name] + len(baseline_printable_data), 0)

                # Write optimal algorithms.
                for algorithm_name in list(set(optimal_algorithms) - set([baseline_algorithm_name])):
                    for path_finder_name in path_finder_names:
                        algorithm_results = [x for x in map_scenario_data if x[0] == algorithm_name and x[1] == path_finder_name and int(x[5]) == agents][:1:]
                        if algorithm_results:
                            algorithm_printable_data = [int(algorithm_results[0][index]) for index in printable_data_indexes]
                            lower_bound = int(algorithm_results[0][6])
                            for i, data_to_print in enumerate(algorithm_printable_data):
                                results_sheet.write(row, relative_algorithms_offsets[algorithm_name] + relative_algorithms_offsets[path_finder_name] + i, data_to_print)
                            results_sheet.write(row, relative_algorithms_offsets[algorithm_name] + relative_algorithms_offsets[path_finder_name] + len(algorithm_printable_data), 0)
                            optimal_solution = algorithm_printable_data[1]

                # Write other algorithms.
                for algorithm_name in list(set(algorithms_names) - set(optimal_algorithms)):
                    for path_finder_name in path_finder_names:
                        algorithm_results = [x for x in map_scenario_data if x[0] == algorithm_name and x[1] == path_finder_name and int(x[5]) == agents][:1:]
                        if algorithm_results:
                            algorithm_printable_data = [int(algorithm_results[0][index]) for index in printable_data_indexes]
                            lower_bound = int(algorithm_results[0][6])
                            for i, data_to_print in enumerate(algorithm_printable_data):
                                results_sheet.write(row, relative_algorithms_offsets[algorithm_name] + relative_algorithms_offsets[path_finder_name] + i, data_to_print)
                            results_sheet.write(row, relative_algorithms_offsets[algorithm_name] + relative_algorithms_offsets[path_finder_name] + len(algorithm_printable_data), -1 if optimal_solution == None else algorithm_printable_data[1] - optimal_solution)


                # Write common information
                results_sheet.write(row, 0, map)
                results_sheet.write(row, 1, int(map.split('-')[1]))
                results_sheet.write(row, 2, scenario)
                results_sheet.write(row, 3, agents)
                if lower_bound != optimal_solution:
                    results_sheet.write(row, 4, lower_bound, different_lb_optimal_format)
                    results_sheet.write(row, 5, optimal_solution if optimal_solution != None else -1, different_lb_optimal_format)
                else:
                    results_sheet.write(row, 4, lower_bound)
                    results_sheet.write(row, 5, optimal_solution if optimal_solution != None else -1)
                
                row += 1

            # Horizontal lines creations.
            results_sheet.set_row(row - 1, None, bottom_line_format)
            
    # Vertical lines creations creation.
    for i in range(len(algorithms_names)*len(path_finder_names) + 1):
        results_sheet.set_column(len(common_map_information) + i*len(common_alg_subheader_data) + len(common_alg_subheader_data) - 1, len(common_map_information) + i*len(common_alg_subheader_data) + len(common_alg_subheader_data) - 1, None, right_line_format)


    ############################################### Document finish #################################################

    # Autofit whole document (adjust all colums width to fit all data corectly).
    results_sheet.autofit()
    workbook.close()

if __name__ == "__main__":
    args = parser.parse_args([] if "__file__" not in globals() else None)
    main(args)
