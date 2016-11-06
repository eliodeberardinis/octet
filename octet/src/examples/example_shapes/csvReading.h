
#include "..\..\octet.h"

namespace octet {
	
	//Class used to read a CSV file in order to create a Bridge made of planks and bases in any combination.
	class ReadCsv {

	public:
		//store data from text file in variables {'B', 'p', 'p', 'B'}
		dynarray<char> variables;

		//Function to transform what's been read in the file to variables usable in the program.
		void read_csv_data(dynarray<uint8_t> file_content) {

			//Array for the raw data after it's been cleaned from spaces, etc. (Safeguard)
			dynarray<uint8_t> raw_data;

			for each(uint8_t c in file_content) {
				if (c != ' ' && c != '\n' && c != '\r') {
					raw_data.push_back(c);
				}
			}

			// get list of ids for Bridge parts
			for (unsigned int cursor = 0; cursor < raw_data.size(); ++cursor) {
				char current_char = raw_data[cursor];
				if (current_char == ';') {
					break;
				}
				else if (current_char == ',') {
					continue;
				}
				else {
					variables.push_back(current_char);
				}
			}
		}

		//Constructor
		ReadCsv() {
		};

		//Function called to read the file containing the bridge instruction
		void read_file() {
			dynarray<uint8_t> file_content;
			std::string file_name = "assets/bridge.txt";
			app_utils::get_url(file_content, file_name.c_str());
			read_csv_data(file_content);
		}
	};

}