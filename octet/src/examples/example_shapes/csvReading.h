
#include "..\..\octet.h"

namespace octet {

	class ReadCsv {
		//test implementation to read csv into variable

		//store data from text file in variables. At the end I should have a version of {'B', 'p', 'p', 'B'} etc

	public:
		dynarray<char> variables;

		void read_csv_data(dynarray<uint8_t> file_content) {

			dynarray<uint8_t> clean_data;

			for each(uint8_t c in file_content) {
				if (c != ' ' && c != '\n' && c != '\r') {
					clean_data.push_back(c);
				}
			}

			// get list of ids for Bridge parts
			for (unsigned int cursor = 0; cursor < clean_data.size(); ++cursor) {
				char current_char = clean_data[cursor];
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

		ReadCsv() {
		};

		void read_file() {
			dynarray<uint8_t> file_content;
			std::string file_name = "assets/bridge.txt";
			app_utils::get_url(file_content, file_name.c_str());

			read_csv_data(file_content);
		}

	};

}