namespace octet {
	class tree : public resource {

		dynarray<char> variables;
		dynarray<char> constants;
		dynarray<char> axiom;
		hash_map<char, dynarray<char>> rules;

		// store data from text file in arrays of variables
		void read_data(dynarray<uint8_t> file_) {

			//srand(time(NULL));

			variables.reset();
			constants.reset();
			axiom.reset();
			rules.clear();

			dynarray<uint8_t> _data;
			for each(uint8_t c in file_) {
				if (c != ' ' && c != '\n' && c != '\r') {
					_data.push_back(c);
				}
			}

			unsigned int cursor = 0;

			// get variables
			for (; cursor < _data.size(); ++cursor) {
				char current_char = _data[cursor];
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
			++cursor;

			// get constants
			for (; cursor < _data.size(); ++cursor) {
				char current_char = _data[cursor];
				if (current_char == ';') {
					break;
				}
				else if (current_char == ',') {
					continue;
				}
				else {
					constants.push_back(current_char);
				}
			}
			++cursor;

			// get axiom
			for (; cursor < _data.size(); ++cursor) {
				char current_char = _data[cursor];
				if (current_char == ';') {
					break;
				}
				else {
					axiom.push_back(current_char);
				}
			}
			++cursor;

			// get rules
			while (cursor < _data.size()) {
				char key = _data[cursor];
				cursor += 3;

				for (; cursor < _data.size(); ++cursor) {
					char current_char = _data[cursor];
					if (current_char == ';') {
						break;
					}
					else {
						rules[key].push_back(current_char);
					}
				}
				++cursor;
			}
		}

		bool is_char_in_array(char ch, dynarray<char> arr) {
			for (unsigned int i = 0; i < arr.size(); ++i) {
				if (arr[i] == ch) {
					return true;
				}
			}
			return false;
		}

	public:

		void read_text_file(int example_number) {
			dynarray<uint8_t> file_content;
			std::string file_name = "assets/lsystems/lsystem" + std::to_string(example_number) + ".txt";
			app_utils::get_url(file_content, file_name.c_str());

			read_data(file_content);
		}

		void evolve() {
			dynarray<char> new_axiom;

			for (unsigned int i = 0; i < axiom.size(); ++i) {
				if (is_char_in_array(axiom[i], variables)) {
					for (unsigned int j = 0; j < rules[axiom[i]].size(); ++j) {
						new_axiom.push_back(rules[axiom[i]][j]);
					}
				}
				else {
					new_axiom.push_back(axiom[i]);
				}
			}

			axiom.resize(new_axiom.size());
			for (unsigned int i = 0; i < new_axiom.size(); ++i) {
				axiom[i] = new_axiom[i];
			}
		}

		void evolve_stoc() {

			srand(static_cast<unsigned int>(time(NULL)));//ADD this or not??

			char rule_type;

			float random = (float)((rand() % 10)*0.1) * 3;
			int i = 0;
			for (i = 1; i <= 3; i++)
			{
				if (random < i) break;
			}

			switch (i)
			{
			case 1:
				rule_type = 'X';
				break;
			case 2:
				rule_type = 'Y';
				break;
			case 3:
				rule_type = 'Z';

				break;
			}

			dynarray<char> new_axiom;
			for (unsigned int i = 0; i < axiom.size(); ++i) {
				if (is_char_in_array(axiom[i], variables)) {
					for (unsigned int j = 0; j < rules[rule_type].size(); ++j) {
						new_axiom.push_back(rules[rule_type][j]);
					}
				}
				else {
					new_axiom.push_back(axiom[i]);
				}
			}

			axiom.resize(new_axiom.size());
			for (unsigned int i = 0; i < new_axiom.size(); ++i) {
				axiom[i] = new_axiom[i];
			}
		}

		dynarray<char> get_axiom() {
			return axiom;
		}

	};
}