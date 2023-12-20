#include "Debug.hpp"

/** Writes stage of map to file.
*
* @param map_to_dump map to be dumped.
* @param dump_file_name file to be wtiten.
*/
std::string map_dump(std::vector<std::vector<size_t>>& map_to_dump, std::string dump_file_name) {

	std::ofstream ofile;
	if (dump_file_name != "") {
		ofile.open(dump_file_name, std::ios::app); //open for writing to the end of file
		if (!ofile.is_open()) {
			return "ERROR: cannot open file for writing: " + dump_file_name + "\n";
		}
	}
	else {
		//setting ofile as std::cout
		ofile.copyfmt(std::cout);
		ofile.clear(std::cout.rdstate());
		ofile.basic_ios<char>::rdbuf(std::cout.rdbuf());
	}

	ofile << std::endl;
	for (size_t i = 0; i < map_to_dump.size(); i++) {
		for (size_t j = 0; j < map_to_dump[0].size(); j++) {
			if (map_to_dump[i][j] != 0) {
				ofile << ".";
				//ofile << map_to_dump[i][j];
			}
			else {
				ofile << "#";
			}
		}
		ofile << std::endl;
	}

	if (dump_file_name != "") {
		ofile.close();
	}

	return "OK";
}
