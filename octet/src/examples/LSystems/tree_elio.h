namespace octet {
	class tree : public resource {

		//Declaration of arrays/hash-maps to store the data read from the files
		dynarray<char> variables;
		dynarray<char> constants;
		dynarray<char> axiom;
		hash_map<char, dynarray<char>> rules;
		//dynarray<unsigned int> seeds;
		dynarray<char> rule_type_stoc;
		

		//Store data read from text file in the relevant arrays 
		void read_data(dynarray<uint8_t> file_) {

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

			// Get variables (F,X,A,B)
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

			// Get constants (+,-,[,])
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

			// Get axiom
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

			// Get rules
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

		//Checks if a character is in an array
		bool is_char_in_array(char ch, dynarray<char> arr) {
			for (unsigned int i = 0; i < arr.size(); ++i) {
				if (arr[i] == ch) {
					return true;
				}
			}
			return false;
		}

	public:

		//Reads the content of the specified file
		void read_text_file(int example_number) {
			dynarray<uint8_t> file_content;
			std::string file_name = "assets/lsystems/lsystem" + std::to_string(example_number) + ".txt";
			app_utils::get_url(file_content, file_name.c_str());

			read_data(file_content);
		}

		//Used to expand the rule that will be read by create_geometry at each iteration (Deterministic L-Systems)
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

		//Used to expand the rule that will be read by create_geometry at each iteration (Stochastic L-Systems)
		void evolve_stoc() {

			//Generate the seed using the current time
			unsigned int seed = static_cast<unsigned int>(time(NULL));

			//Seeding the random funciton
			srand(static_cast<unsigned int>(time(NULL)));

			char rule_type;

			//Generating the random variable to choose wich rule tu use with equal probability
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

			//Saving the rule type in an array (for use in devolve_stoc function)
			rule_type_stoc.push_back(rule_type);

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

		//Used to expand the rule that will be read by create_geometry at each iteration (Stochastic L-Systems)
		void evolve_stoc_new() {

			dynarray<char> new_axiom;
			for (unsigned int i = 0; i < axiom.size(); ++i) {

				if (is_char_in_array(axiom[i], variables)) {

					char rule_type;

					//Seeding the random function
					srand(static_cast<unsigned int>(time(NULL)));

					//Generating the random variable to choose wich rule tu use with equal probability
					float random = (float)((rand() % 10)*0.1) * 3;
					int k = 0;
					for (k = 1; k <= 3; k++)
					{
						if (random < k) break;
					}

					switch (k)
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

					for (unsigned int j = 0; j < rules[rule_type].size(); ++j)
					{


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

		//Re-draws the stochastic system using the rule_type array saved in evolve_stoc() (Method used)
		void devolve_stoc(unsigned int iteration) {

			char rule_type;
			rule_type = rule_type_stoc[iteration-1];

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

		//Clears the arrays containing the rule_type and seeds
		void reset_stoc(){

			rule_type_stoc.reset();

		}

		//Deletes the last element of the arrays containing the rule_type or seeds and rezises them
		void decrese_stoc_array() {

			rule_type_stoc.resize(rule_type_stoc.size() - 1);
		}

		//Access to the axiom containing the rules
		dynarray<char> get_axiom() {
			return axiom;
		}

	};
}