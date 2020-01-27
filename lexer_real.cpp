#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>

		// input 		[letter	digit	!	space	\n	{	}	;	+	-	*	/	%	=	>	<	.	,	(	)
		typedef enum Input {
			LETTER,			// 0
			DIGIT,			// 1
			EXCLAMATION,		// 2
			SPACE,			// 3
			LINE_END,		// 4
			OPEN_CURLY_BLANK, 	// 5
			CLOSE_CURLY_BLANK,	// 6
			SEMI_COLON,		// 7
			PLUS_SIGN,		// 8
			MINUS_SIGN,		// 9
			MULTI_SIGN,		// 10
			DIVIDE_SIGN,		// 11
			MOD_SIGN,		// 12
			EQUAL_SIGN,		// 13
			LEFT_GREATER,		// 14
			RIGHT_GREATER,		// 15
			PERIOD_SIGN,		// 16
			COMMA_SIGN,		// 17
			OPEN_PAREN,		// 18
			CLOSE_PAREN,		// 19

			OTHER,			// 20
			S_END,			// 21
			// Input number, put a new input above this line
			INPUT_COUNT	// 22
		} Input;
		const size_t COL_SIZE = INPUT_COUNT; // for checkking the # of columns

		typedef enum State {
			START,			// 0
			STRING,			// 1	keyword + identifier
			NUMBER,			// 2	number
			IN_COMMENT,		// 3	in comment !
			END_NOT_SAVING,		// 4	end_not_saving (out comment !)
			END_INTEGER,		// 5	end_integer
			END,			// 6	end (regular)
			END_STRING,		// 7	end string
			END_REAL,		// 8	end real number
			TOKEN_PERIOD,		// 9	token .
			TOKEN_COMMA,		// 10	token ,
			TOKEN_OPEN_CUR_BLANK,	// 11	token {
			TOKEN_CLOSE_CUR_BLANK,	// 12	token }
			TOKEN_SEMICOLON,	// 13	token ;
			TOKEN_MOD,		// 14	token %
			TOKEN_INCRE,		// 15	token ++
			TOKEN_PLUS_EQUAL,	// 16	token +=
			TOKEN_DECRE,		// 17	token --
			TOKEN_MINUS_EQUAL,	// 18	token -=
			TOKEN_MULTI_EQUAL,	// 19	token *=
			TOKEN_DIVIDE_EQUAL,	// 20	token /=
			TOKEN_EQUAL,		// 21	token ==
			TOKEN_LEFT_EQ_GREAT,	// 22	token >=
			TOKEN_RIGHT_EQ_GREAT,	// 23	token <=
			FOUND_PERIOD,		// 24	found .
			REAL_NUM,		// 25	real number
			UNKNOWN,		// 26	unknown
			FOUND_PLUS,		// 27	found +
			FOUND_MINUS,		// 28	found -
			FOUND_MULTI,		// 29	found *
			FOUND_DIVIDE,		// 30	found /
			FOUND_EQ,		// 31	found =
			FOUND_LEFT_GREAT,	// 32	found >
			FOUND_RIGHT_GREAT,	// 33	found <
			TOKEN_OPEN_PAREN,	// 34	token (
			TOKEN_CLOSE_PAREN,	// 35	token )
			Other,			// 36	sth else
			end_str,		// 37	'\0'

			// State number, put a new state above this line
			STATE_COUNT	// 38
		} State;
		const size_t ROW_SIZE = STATE_COUNT; // for checking the # of rows

/*		// Token list
		typedef enum Token {
			IDENTIFIER,	// 0
			KEYWORD,	// 1
			INTEGER,	// 2
			REAL_NUMBER,	// 3
			OPERATOR,	// 4
			RELATION_OP,	// 5
			COMMENT,	// 6

			// Token number, put a new lexeme above this line
			TOKEN_COUNT	// 7
		} Token;
*/// not using Token list

// Data structure for the FSA table
class FSA {
	private:
		// real table
		//			0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21
		// input 		[letter	digit	!	space	\n	{	}	;	+	-	*	/	%	=	>	<	.	,  	(	)	other	\0	]

		int fsa_table[38][22] = {{1, 	2, 	3, 	0, 	0,	11,	12,	13,	27,	28,	29,	30,	14,	31,	32,	33,	24,	10,	34,	35,	36,	0},	// 0	start
					{1, 	1, 	7, 	7, 	7, 	7,	7, 	7, 	7, 	7, 	7, 	7, 	7, 	7, 	7, 	7, 	9, 	10,	34,	35,	36,	7}, 	// 1	string
					{4,	2,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	5,	24,	5,	5,	5,	36,	5},	// 2 	number
					{3,	3,	4,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3},	// 3	in comment

					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	//// 4	end_not_saving
//					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 4	end_not_saving

//					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 5	end_integer
					{1,	2,	4,	0,	0,	11,	12,	13,	27,	28,	29,	30,	14,	31,	32,	33,	24,	10,	34,	35,	36,	0},	// 6	end

					{1,	2,	4,	0,	0,	11,	12,	13,	27,	28,	29,	30,	14,	31,	32,	33,	24,	10,	34,	35,	36,	0},	// 6	end

				//	{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	9,	10,	6,	6,	6,	6},	// 7	end_string
					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	9,	10,	0,	0,	0,	0},	// 7	end_string

					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	9,	10,	0,	0,	0,	0},	// 8	end_real

				//	{0,	0,	0,	9,	9,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 9	token .
				//	{0,	0,	0,	10,	10,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 10	token ,
				//	{0,	0,	0,	11,	11,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 11	token {
				//	{0,	0,	0,	12,	12,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 12	token }
				//	{0,	0,	0,	13,	13,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 13	token ;
					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 14	token %
				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 15	token ++
				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 16	token +=

					{6,	6,	6,	9,	9,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 9	token .
					{6,	6,	6,	10,	10,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 10	token ,
					{6,	6,	6,	11,	11,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 11	token {
					{6,	6,	6,	12,	12,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 12	token }
					{6,	6,	6,	13,	13,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 13	token ;
				//	{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 14	token %
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 15	token ++
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 16	token +=

					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 17	token --
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 18	token -=
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 19	token *=
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 20	token /=
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 21	token ==

				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 17	token --
				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 18	token -=
				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 19	token *=
				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 20	token /=
				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 21	token ==

				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 22	token >=
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 22	token >=

				//	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 23	token =<
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	// 23	token =<

					{9,	25,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9},	// 24	found .
					{26,	25,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8},	// 25	real num
					{26,	26,	26,	4,	4,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	26,	4},	// 26	unknown
					{6,	6,	6,	6,	6,	6,	6,	6,	15,	6,	6,	6,	6,	16,	6,	6,	6,	6,	6,	6,	6,	6},	// 27	found +
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	17,	6,	6,	6,	18,	6,	6,	6,	6,	6,	6,	6,	6},	// 28	found -
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	19,	6,	6,	6,	6,	6,	6,	6,	6},	// 29	found *
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	20,	6,	6,	6,	6,	6,	6,	6,	6},	// 30	found /
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	21,	22,	23,	6,	6,	6,	6,	6,	6},	// 31	found =
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	22,	6,	6,	6,	6,	6,	6,	6,	6},	// 32	found >
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	23,	6,	6,	6,	6,	6,	6,	6,	6},	// 33	found <

//					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 34	token (
//					{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},	// 35	token )

					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	//// 34	token (
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	//// 35	token )
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6},	//// 36	other
					{6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6}};	// 37	found '\0'



		std::map<char, int> input_map;
//		std::map<char, int>::iterator input_it;
		std::multimap<int, std::string> lex_storage;//key is state value is lexeme
		std::multimap<int, std::string>::iterator storage_it;
		std::queue<char> tmp_lex;
		std::queue<char> cur_char;

	public:
//		void make_table();   						// later : void make_table(const std::fsteram&);

		int get_input(const char&);					// categorize an input char to a number(enum)
		int get_state(const int&, const int&);				// next state = table [current state] [input]
		void state_check(const int&);					// check the state
		void add_lexeme(const int&);					// add a lexeme to lex_storage
		void print();							// print result
};

int FSA::get_input (const char& c) {
//	input_it = input_map.find(c);
//	if (input_it != input_map.end()) {
//		return input_map[c];
//	}

	if	('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_' || c == '$') {
		input_map[c] = LETTER;
	}
	else if	('0' <= c && c <= '9') {
		input_map[c] = DIGIT;
	}
	else if (c == '\n') {
		input_map[c] = LINE_END;
	}
	else if (c == ' ') {
		input_map[c] = SPACE;
	}
	else if (c == '!') {
		input_map[c] = EXCLAMATION;
	}
	else if (c == '{') {
		input_map[c] = OPEN_CURLY_BLANK;
	}
	else if (c == '}') {
		input_map[c] = CLOSE_CURLY_BLANK;
	}
	else if (c == ';') {
		input_map[c] = SEMI_COLON;
	}
	else if (c == '+') {
		input_map[c] = PLUS_SIGN;
	}
	else if (c == '-') {
		input_map[c] = MINUS_SIGN;
	}
	else if (c == '-') {
		input_map[c] = MULTI_SIGN;
	}
	else if (c == '/') {
		input_map[c] = DIVIDE_SIGN;
	}
	else if (c == '%') {
		input_map[c] = MOD_SIGN;
	}
	else if (c == '=') {
		input_map[c] = EQUAL_SIGN;
	}
	else if (c == '>') {
		input_map[c] = LEFT_GREATER;
	}
	else if (c == '>') {
		input_map[c] = RIGHT_GREATER;
	}
	else if (c == '.') {
		input_map[c] = PERIOD_SIGN;
	}
	else if (c == ',') {
		input_map[c] = COMMA_SIGN;
	}
	else if (c == '(') {
		input_map[c] = OPEN_PAREN;
	}
	else if (c == ')') {
		input_map[c] = CLOSE_PAREN;
	}
//	else if (c == '\0') {
//		input_map[c] = S_END;
//	}
	else {
		input_map[c] = OTHER;
	}
	cur_char.push(c);
	return input_map[c];
}

int FSA::get_state (const int& state, const int& input) {
	return fsa_table[state][input];
}

void FSA::state_check(const int& state) {
	switch (state) {
		case 0:			// Using previous input, advance 1 state
			get_state(state, cur_char.front());
			break;
		case 1:					//IDENTIFIER + KEYWORD
			break;
		case 2:					// Number, keep going
			break;
		case 3:					// In comment, keep going
			break;
		case 4:					// out comment (End_not_saving), clear all tmp_lex and cur_char
			while (tmp_lex.size() != 0) {
				tmp_lex.pop();
			}
			if(cur_char.front() == '!') {
				cur_char.push(' ');
				cur_char.pop();
			}
			break;
		case 5:					// End_integer, add lexeme
			add_lexeme(state);
			break;
		case 6:					// End, mostly Operator, Separator
			add_lexeme(state);
			break;
		case 7 :				// End_string, need keyword checking
			add_lexeme(state);
			break;
		case 8:					// End_real, real number
			add_lexeme(state);
			break;
		case 9:					// token '.'
			add_lexeme(state);
			break;
		case 10:				// token ','
			add_lexeme(state);
			break;
		case 11:				// token '{'
			add_lexeme(state);
			break;
		case 12:				// token '}'
			add_lexeme(state);
			break;
		case 13:				// token ';'
			add_lexeme(state);
			break;
		case 14:				// token '%'
			add_lexeme(state);
			break;
		case 15:				// token '++'
			add_lexeme(state);
			break;
		case 16:				// token '+='
			add_lexeme(state);
			break;
		case 17:				// token '--'
			add_lexeme(state);
			break;
		case 18:				// token '-='
			add_lexeme(state);
			break;
		case 19:				// token '*='
			add_lexeme(state);
			break;
		case 20:				// token '/='
			add_lexeme(state);
			break;
		case 21:				// token '='
			add_lexeme(state);
			break;
		case 22:				// token '>='
			add_lexeme(state);
			break;
		case 23:				// token '=<'
			add_lexeme(state);
			break;
		case 24:				// found '.'
			break;
		case 25:				// real number, keep going
			break;
		case 26:				// unknown, make sure how to deal "unknown"
//			add_lexeme(state);
			break;
		case 27:				// found '+'
			break;
		case 28:				// found '-'
			break;
		case 29:				// found '*'
			break;
		case 30:				// found '/'
			break;
		case 31:				// found '='
			break;
		case 32:				// found '>'
			break;
		case 33:				// found '<'
			break;
		case 34:				// token '('
			add_lexeme(state);
			break;
		case 35:				// token ')'
			add_lexeme(state);
			break;
		case 36:				// other
			break;
//		case 37:				// end string '\0'
//			break;
	}
	tmp_lex.push(cur_char.front());
	cur_char.pop();
	return;
}

void FSA::add_lexeme (const int& state) {
	std::string lex;
	while (!tmp_lex.empty()) {
		if (tmp_lex.front() == ' ' || tmp_lex.front() == '\n') {
			tmp_lex.pop();
		} else {
		lex.push_back(tmp_lex.front());
		tmp_lex.pop();
		}
	}
	lex.push_back('\0');
	lex_storage.insert(std::pair<int, std::string>(state, lex));
	return;
}

void FSA::print() {
	storage_it = lex_storage.begin();
	while(storage_it != lex_storage.end()) {
		switch (storage_it->first) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				std::cout << " Integer : " << storage_it->second << std::endl;
				break;
			case 6:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 7:
//				if (find.(storage_it->second) != KEYWORD) TODO
				std::cout << " Identifier : " << storage_it->second << std::endl;
				break;
			case 8:
				std::cout << " Real : " << storage_it->second << std::endl;
				break;
			case 9:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 10:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 11:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 12:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 13:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 14:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 15:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 16:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 17:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 18:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 19:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 20:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 21:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 22:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 23:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 24:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 25:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 26:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 27:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 28:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 29:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 30:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 31:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 32:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 33:
				std::cout << " Operator : " << storage_it->second << std::endl;
				break;
			case 34:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 35:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
			case 36:
				std::cout << " Separator : " << storage_it->second << std::endl;
				break;
//			case 37:
//				std::cout << " Line ends : " << storage_it->second << std::endl;
//				break;
		}
		storage_it++;
	}
	return;
}

// state = [state, input];
// if (state == Grab_period) then return current str and save it
// else if (state == Grab_Stay) then save it and keep check the state

int main (int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Usage: [lexer] [target file]" << std::endl;
		return 0;
	}

	std::fstream _file;
	_file.open(argv[1]);

	if (_file.is_open()) {

		FSA fsa;
		char c;
		int state = 0;
		int input;

		while(_file.get(c)) {
			input = fsa.get_input(c);		//from character to input(size_t)
			state = fsa.get_state(state, input);	//to the next state
			fsa.state_check(state);			//check the state is final or not, and save the lexeme or not

			if (state == 0) {
				state = fsa.get_state(state, input);
			}
		}

		fsa.print();					//print storage

	} else {
		std::cout << "cannot open file" << std::endl;
	}


	_file.close();
	return 0;
}
