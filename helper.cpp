#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

#define XML_HAT "<?xml version=\"1.0\" encoding=\"utf-8\" ?><sql> <datatypes db=\"mysql\"> <group label=\"Numeric\" color=\"rgb(238,238,170)\"> <type label=\"Integer\" length=\"0\" sql=\"INTEGER\" quote=\"\"/> <type label=\"TINYINT\" length=\"0\" sql=\"TINYINT\" quote=\"\"/> <type label=\"SMALLINT\" length=\"0\" sql=\"SMALLINT\" quote=\"\"/> <type label=\"MEDIUMINT\" length=\"0\" sql=\"MEDIUMINT\" quote=\"\"/> <type label=\"INT\" length=\"0\" sql=\"INT\" quote=\"\"/> <type label=\"BIGINT\" length=\"0\" sql=\"BIGINT\" quote=\"\"/> <type label=\"Decimal\" length=\"1\" sql=\"DECIMAL\" re=\"DEC\" quote=\"\"/> <type label=\"Single precision\" length=\"0\" sql=\"FLOAT\" quote=\"\"/> <type label=\"Double precision\" length=\"0\" sql=\"DOUBLE\" re=\"DOUBLE\" quote=\"\"/> </group> <group label=\"Character\" color=\"rgb(255,200,200)\"> <type label=\"Char\" length=\"1\" sql=\"CHAR\" quote=\"'\"/> <type label=\"Varchar\" length=\"1\" sql=\"VARCHAR\" quote=\"'\"/> <type label=\"Text\" length=\"0\" sql=\"MEDIUMTEXT\" re=\"TEXT\" quote=\"'\"/> <type label=\"Binary\" length=\"1\" sql=\"BINARY\" quote=\"'\"/> <type label=\"Varbinary\" length=\"1\" sql=\"VARBINARY\" quote=\"'\"/> <type label=\"BLOB\" length=\"0\" sql=\"BLOB\" re=\"BLOB\" quote=\"'\"/> </group> <group label=\"Date &amp; Time\" color=\"rgb(200,255,200)\"> <type label=\"Date\" length=\"0\" sql=\"DATE\" quote=\"'\"/> <type label=\"Time\" length=\"0\" sql=\"TIME\" quote=\"'\"/> <type label=\"Datetime\" length=\"0\" sql=\"DATETIME\" quote=\"'\"/> <type label=\"Year\" length=\"0\" sql=\"YEAR\" quote=\"\"/> <type label=\"Timestamp\" length=\"0\" sql=\"TIMESTAMP\" quote=\"'\"/> </group> <group label=\"Miscellaneous\" color=\"rgb(200,200,255)\"> <type label=\"ENUM\" length=\"1\" sql=\"ENUM\" quote=\"\"/> <type label=\"SET\" length=\"1\" sql=\"SET\" quote=\"\"/> <type label=\"Bit\" length=\"0\" sql=\"bit\" quote=\"\"/> </group> </datatypes>\n"

string toupper(string str) {
	for(char& c : str) c = toupper(c);
	return str;
}

struct Table;

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

Table* NANTABLE = new Table{"UNFOUND", {{"pk_UNFOUND", "", "", nullptr}}};

ostream& operator<<(ostream& os, Field field) {
	return os << '\t'<< field.title << ' ' << field.type << (field.info.empty() ? "" : " ") << field.info << ',';;
}

ifstream& operator>>(ifstream& is, Table& table) {
	string temp;
	while (temp.substr(0, 6) != "create") {
		getline(is, temp);
	}

	stringstream ss;
	ss << temp;
	ss >> temp >> temp >> table.title;
	if (table.title[table.title.length()-1]=='(') table.title.erase(table.title.length()-1);

	while (true) {
		getline(is, temp);

		if (temp == ");")
			break;

		if (!temp.empty()) {
			temp.erase(remove_if(temp.begin(), temp.end(), [](char c){return c =='\t';}));
			temp.erase(remove_if(temp.begin(), temp.end(), [](char c){return c ==',';}));
			string firstWord = temp.substr(0, temp.find(' '));

			if (firstWord == "primary" || firstWord == "foreign" || firstWord == "on")
				continue;
			table.fields.emplace_back(temp);
		}
	}

	return is;
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

void init_refs(vector<Table>& tables) {
	for(Table& table : tables) {
		for (Field& field : table.fields){
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
	result << "<table x=\"500\" y=\"200\" name=\""<< table.title << "\">\n";

	for (Field& field : table.fields) {
		int null = field.info.find("not null") == string::npos;
		int autoincrement = field.isPk();

		result << "\t<row name=\"" << field.title << "\" "
		          "null=\"" << null << "\" "
		          "autoincrement=\"" << autoincrement << "\">\n";
		
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


int main(int argc, char *argv[]) {
	string filename;

	if(argc < 2) {
		cout << "Filename = ";
		getline(cin, filename);
	} else {
		filename = argv[1];
	}

	ifstream in(filename);
	if (!in) {
		cout << "Error reading file\n";
		return 1;
	}
	
	vector<Table> tables;
	
	while (!in.eof()) {
		Table temp;
		in >> temp;
		tables.push_back(temp);
	}

	init_refs(tables);
	
	ofstream outsql("output.sql");
	cout << "Writing to output.sql\n";
	for(Table& table : tables) {
		//cout   << table << '\n';
		outsql << table << '\n';
	}

	ofstream outxml("output.xml");
	cout << "Writing to output.xml\n";

	outxml << XML_HAT;

	for(Table& table : tables) {
		string temp = toXML(table);
		//cout   << temp << '\n';
		outxml << temp << '\n';
	}

	outxml << "</sql>";

	cout << "Done!\n";
}
