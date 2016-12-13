#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\" ?><sql> <datatypes db=\"mysql\"> <group label=\"Numeric\" color=\"rgb(238,238,170)\"> <type label=\"Integer\" length=\"0\" sql=\"INTEGER\" quote=\"\"/> <type label=\"TINYINT\" length=\"0\" sql=\"TINYINT\" quote=\"\"/> <type label=\"SMALLINT\" length=\"0\" sql=\"SMALLINT\" quote=\"\"/> <type label=\"MEDIUMINT\" length=\"0\" sql=\"MEDIUMINT\" quote=\"\"/> <type label=\"INT\" length=\"0\" sql=\"INT\" quote=\"\"/> <type label=\"BIGINT\" length=\"0\" sql=\"BIGINT\" quote=\"\"/> <type label=\"Decimal\" length=\"1\" sql=\"DECIMAL\" re=\"DEC\" quote=\"\"/> <type label=\"Single precision\" length=\"0\" sql=\"FLOAT\" quote=\"\"/> <type label=\"Double precision\" length=\"0\" sql=\"DOUBLE\" re=\"DOUBLE\" quote=\"\"/> </group> <group label=\"Character\" color=\"rgb(255,200,200)\"> <type label=\"Char\" length=\"1\" sql=\"CHAR\" quote=\"'\"/> <type label=\"Varchar\" length=\"1\" sql=\"VARCHAR\" quote=\"'\"/> <type label=\"Text\" length=\"0\" sql=\"MEDIUMTEXT\" re=\"TEXT\" quote=\"'\"/> <type label=\"Binary\" length=\"1\" sql=\"BINARY\" quote=\"'\"/> <type label=\"Varbinary\" length=\"1\" sql=\"VARBINARY\" quote=\"'\"/> <type label=\"BLOB\" length=\"0\" sql=\"BLOB\" re=\"BLOB\" quote=\"'\"/> </group> <group label=\"Date &amp; Time\" color=\"rgb(200,255,200)\"> <type label=\"Date\" length=\"0\" sql=\"DATE\" quote=\"'\"/> <type label=\"Time\" length=\"0\" sql=\"TIME\" quote=\"'\"/> <type label=\"Datetime\" length=\"0\" sql=\"DATETIME\" quote=\"'\"/> <type label=\"Year\" length=\"0\" sql=\"YEAR\" quote=\"\"/> <type label=\"Timestamp\" length=\"0\" sql=\"TIMESTAMP\" quote=\"'\"/> </group> <group label=\"Miscellaneous\" color=\"rgb(200,200,255)\"> <type label=\"ENUM\" length=\"1\" sql=\"ENUM\" quote=\"\"/> <type label=\"SET\" length=\"1\" sql=\"SET\" quote=\"\"/> <type label=\"Bit\" length=\"0\" sql=\"bit\" quote=\"\"/> </group> </datatypes>\n"
#define XML_FOOTER "</sql>"

//////////////////////////////////////////////////////////////////////////
///////////////////////////   UTILS   ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define print(x) std::cout << #x << " = " << (x) << '\n'

string toupper(string str) {
	for(char& c : str) c = toupper(c);
	return str;
}

string erase_chars(string str, char ch) {
	str.erase(remove_if(str.begin(), str.end(), [&](char c){return c == ch;}));
	return str;
}

string read_file(string& filepath) {
	//C style instead of C++'s ifstream because of speed
	FILE* file;
	fopen_s(&file, filepath.c_str(), "rt");

	if (!file) {
		cerr << "Cant open file";
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	char* data = new char[length + 1];
	memset(data, 0, length + 1);
	fseek(file, 0, SEEK_SET);
	fread(data, 1, length, file);
	fclose(file);
	string result = data;
	delete[] data;
	return result;
}

bool isname(char c) {
	return isalpha(c) || c == '_';
}

vector<string> get_words(string& str, int n = -1) {
	vector<string> result;
	int k = 0;
	for (size_t i = 0; i < str.length(); ++i) {
		if (isname(str[i])) {
			if (n != -1 && ++k > n) break;

			string word;
			while (isname(str[i])) {
				word += str[i++];
			}
			result.push_back(word);
		}
	}
	return result;
}

vector<string> split(string str, string delimiter) {
	vector<string> result;

	size_t pos    = 0;
	size_t oldPos = 0;
	while ((pos = str.find(delimiter, pos)) != string::npos) {
		result.push_back(str.substr(oldPos, pos-oldPos));
		oldPos = pos += delimiter.length();
	}
	result.push_back(str.substr(oldPos));
	return result;
}

template<typename T>
bool operator==(vector<T> lhs, vector<T> rhs) {
	if (lhs.size() != rhs.size()) return false;
	for (size_t i = 0; i < lhs.size(); ++i) {
		if (lhs[i] != rhs[i]) return false;
	}
	return true;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const vector<T>& vec) {
	os << '{';
	for (int i = 0; i < vec.size(); ++i) {
		os << vec[i];
		if (i != vec.size()-1) {
			os << ", ";
		}
	}
	return os << '}';
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////   STRUCTS   //////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct Table;

struct CharQueue {
	string data;
	int size;

	CharQueue(int size) : size(size) {}

	void push(char c) {
		data += c;
		while (data.length() > size) data.erase(0, 1);
	}
};

struct Lexeme {
	string data;
};

struct Field {
	string title, type, info;
	Table* ref;

	Field(string title, string type, string info, Table* ref) :
		title(title),
		type(type),
		info(info),
		ref(ref) {
	}

	Field(string str) {
		stringstream ss;
		ss << str;
		ss >> title >> type;
		ss.get();
		getline(ss, info);
		ref = nullptr;
	}

	bool isPk() {
		return title.substr(0, 3) == "pk_";
	}
	
	bool isFk() {
		return title.substr(0, 3) == "fk_";
	}
	
	string getPureTitle() {
		if(isPk() || isFk()) return title.substr(3);
		return title;
	}
};

struct Table {
	string title;
	vector<Field> fields;

	Table(string title, vector<Field> fields) : title(title), fields(fields) {}

	Table(string& str) {
		vector<string> words = get_words(str, 3);
		title = words[2];

		size_t leftBracket  = str. find('(');
		size_t rightBracket = str.rfind(')');
		vector<string> body = split(str.substr(leftBracket+1, rightBracket-leftBracket-1), ",");

		for (string& str : body) {
			string firstWord = get_words(str, 1)[0];
			if (firstWord != "primary" && firstWord != "foreign")
				fields.emplace_back(str);
		}
	}

	Field& getPk() {
		for(Field& field : fields) if (field.isPk()) return field;
		return fields[0];
	}

	int fkAmount() {
		int sum = 0;
		for(Field& field : fields) if (field.isFk()) sum += 1;
		return sum;
	}
};

//////////////////////////////////////////////////////////////////////////
///////////////////////////   GLOBALS   //////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Table* NANTABLE = new Table{"UNFOUND", {{"pk_UNFOUND", "", "", nullptr}}};
vector<string> CREATETABLE = {"create", "table"};

//////////////////////////////////////////////////////////////////////////
///////////////////////////   IO   ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

istream& operator>>(istream& is, Lexeme& lexeme) {
	stringstream ss;
	char c;
	CharQueue cq(2);
	bool inString = false;

	while (!is.eof()) {
		c = is.get();		
		cq.push(c);

		if (cq.data[0] != '\\' && c == '\'')
			inString = !inString;
		
		//cut comments
		if (!inString) {
			if (c == '-' || c == '#') {
				while (c != '\n') {
					c = is.get();
				}
			} else if (c == '/') {
				while (cq.data != "*/") {
					c = is.get();
					cq.push(c);
				}
				c = is.get();
			}
		}

		if (c == '\n') continue;
		if (c == '\t') continue;
		if (c == ' ' && cq.data.length() != 0 && cq.data[0] == ' ') continue;
		if (c == ';') break;

		ss << c;
	}
	lexeme.data = ss.str();
	return is;
} 

ostream& operator<<(ostream& os, Field field) {
	return os << '\t'<< field.title << ' ' << field.type << (field.info.empty() ? "" : " ") << field.info << ',';
}

ostream& operator<<(ostream& os, Table table) {
	os << "create table " << table.title << "(\n";

	for (Field field : table.fields) {
		os << field << '\n';
	}

	os << "\n\tprimary key (" << table.getPk().title << ")";

	if (table.fkAmount() != 0) os << ",";

	for (size_t i = 0; i < table.fields.size(); ++i) {
		Field& field = table.fields[i];
		
		if (!field.isFk()) continue;

		Table* ref = field.ref;
		if (!ref) ref = NANTABLE;

		os << "\n\n\tforeign key (" << field.title << ") references " << ref->title << "(" << ref->getPk().title << ")\n"
		      "\t\ton update cascade on delete restrict";
		
		if (i != table.fields.size()-1)
			os << ",";
	}

	return os << "\n);\n";
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////   LOGIC   ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void init_refs(vector<Table>& tables) {
	for (Table& table : tables) {
		for (Field& field : table.fields) {
			if (field.isFk()) {
				string pk_title = "pk_" + field.getPureTitle();

				auto it =  find_if(tables.begin(),             tables.end(), [&](Table& table){
				    return find_if(table.fields.begin(), table.fields.end(), [&](Field& field){return field.title == pk_title;}) != table.fields.end();
				});

				field.ref = (it != tables.end()) ? (&(*it)) : (nullptr);

				while (!field.ref) {
					cout << "Cant resolve reference for " << table.title << "." << field.title << ", "
					        "which table should it references to?\n";
					string temp;
					getline(cin, temp); cout << '\n';
					it = find_if(tables.begin(), tables.end(), [&](Table& table){return table.title == temp;});

					field.ref = (it != tables.end()) ? (&(*it)) : (nullptr);
				}
			}	
		}
	}
}

string toXML(Table& table) {
	stringstream result;

	static int x = 0;
	static int y = 0;
	result << "<table x=\"" << x << "\" y=\"" << y << "\" name=\""<< table.title << "\">\n";
	x += 50;
	y += 50;

	for (Field& field : table.fields) {
		int isNull = (field.info.find("not null") == string::npos);
		int isAutoincrement = field.isPk();

		result << "\t<row name=\"" << field.title << "\" "
		          "null=\"" << isNull << "\" "
		          "autoincrement=\"" << isAutoincrement << "\">\n";
		
		result << "\t\t<datatype>" << toupper(field.type) << "</datatype>\n";
		result << "\t\t<default>NULL</default>\n";

		if (field.isFk()) {
			Table* ref = field.ref;
			if (!ref) ref = NANTABLE;
			result << "\t\t<relation table=\"" << ref->title << "\" row=\"" << ref->getPk().title<< "\" />\n";
		}
		result<<"\t</row>\n";
	}
	result << "</table>";
	return result.str();
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////   MAIN   /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {
	string filename;

	if(argc < 2) {
		cout << "Filename = ";
		getline(cin, filename);
	} else {
		filename = argv[1];
	}

	string file = read_file(filename);
	stringstream fs;
	fs << file;

	vector<Lexeme> lexemes;
	while (!fs.eof()) {
		Lexeme temp;
		fs >> temp;
		lexemes.push_back(temp);
	}
	
	vector<Table> tables;
	
	for (Lexeme& lexeme : lexemes) {
		if (get_words(lexeme.data, 2) == CREATETABLE) {
			tables.emplace_back(lexeme.data);
		}
	}

	init_refs(tables);
	
	ofstream outsql("outSQL.sql");
	cout << "Writing to outSQL.sql\n";

	for(Table& table : tables) {
		outsql << table << '\n';
	}

	ofstream outxml("outXML.xml");
	cout << "Writing to outXML.xml\n";

	outxml << XML_HEADER;

	for(Table& table : tables) {
		string temp = toXML(table);
		outxml << temp << '\n';
	}

	outxml << XML_FOOTER;
	cout << "Done!\n";
}
