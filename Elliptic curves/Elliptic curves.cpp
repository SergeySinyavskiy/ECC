#define NOMINMAX
#include <iostream>
#include <utility>
#include <Windows.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include <conio.h>
namespace num = boost::multiprecision;
num::cpp_int p, a, b, Px, Py, n;
std::pair<num::cpp_int, num::cpp_int> base_point;
std::vector<char> symbols = { '!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8',' ',
							  '9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','\n',
							  'Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h',
							  'i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~','Ґ','Ё',
							  'Є','Ї','І','і','ґ','ё','є','ї','А','Б','В','Г','Д','Е','Ж','З','И','Й','К','Л','М','Н','О','П',
							  'Р','С','Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ','Ы','Ь','Э','Ю','Я','а','б','в','г','д','е','ж','з',
							  'и','й','к','л','м','н','о','п','р','с','т','у','ф','х','ц','ч','ш','щ','ъ','ы','ь','э','ю','я' };
std::vector<unsigned char> alphabet;
std::vector<std::pair<num::cpp_int, num::cpp_int>> alphabet_coordinates;
void DecToBin(num::cpp_int a, std::vector<int>& result) {
	while (a > 0) {
		result.push_back(static_cast<int>(a % 2));
		a /= 2;
	}
	std::reverse(result.begin(), result.end());
}
std::pair<num::cpp_int, num::cpp_int> Sum(std::pair<num::cpp_int, num::cpp_int> P, std::pair<num::cpp_int, num::cpp_int> Q, num::cpp_int mod) {
	std::pair<num::cpp_int, num::cpp_int> Res;
	num::cpp_int first_part = num::powm(Q.second - P.second, 1, mod);
	num::cpp_int second_part = boost::integer::mod_inverse(static_cast<num::cpp_int>(num::powm(Q.first - P.first, 1, mod)), mod);
	num::cpp_int liambda = num::powm(first_part * second_part, 1, mod);
	if (liambda < 0) {
		while (liambda < 0) {
			liambda += mod;
		}
	}
	Res.first = num::powm(static_cast<num::cpp_int>(num::powm(num::pow(liambda, 2), 1, mod)) - P.first - Q.first, 1, mod);
	if (Res.first < 0) {
		while (Res.first < 0) {
			Res.first += mod;
		}
	}
	Res.second = num::powm(liambda * (P.first - Res.first) - P.second, 1, mod);
	if (Res.second < 0) {
		while (Res.second < 0) {
			Res.second += mod;
		}
	}
	return Res;
}
std::pair<num::cpp_int, num::cpp_int> Sub(std::pair<num::cpp_int, num::cpp_int> P, std::pair<num::cpp_int, num::cpp_int> Q, num::cpp_int mod) {
	std::pair<num::cpp_int, num::cpp_int> Res;
	Q.second *= -1;
	return Sum(P, Q, mod);
}
std::pair<num::cpp_int, num::cpp_int> Double(std::pair<num::cpp_int, num::cpp_int> P, num::cpp_int mod, num::cpp_int a, num::cpp_int n) {
	std::pair<num::cpp_int, num::cpp_int> Res;
	num::cpp_int liambda;
	if (n != 1) {
		for (num::cpp_int i = 2; i <= n; i *= 2) {
			num::cpp_int first_part = num::powm(static_cast<num::cpp_int>(num::powm(3 * num::pow(P.first, 2), 1, mod)) + a, 1, mod);
			num::cpp_int second_part = boost::integer::mod_inverse(static_cast<num::cpp_int>(num::powm(2 * P.second, 1, mod)), mod);
			liambda = num::powm(first_part * second_part, 1, mod);
			if (liambda < 0) {
				while (liambda < 0) {
					liambda += mod;
				}
			}

			Res.first = num::powm(static_cast<num::cpp_int>(num::powm(num::pow(liambda, 2), 1, mod)) - static_cast<num::cpp_int>(powm(2 * P.first, 1, mod)), 1, mod);
			if (Res.first < 0) {
				while (Res.first < 0) {
					Res.first += mod;
				}
			}
			Res.second = num::powm(static_cast<num::cpp_int>(num::powm(liambda * (P.first - Res.first), 1, mod)) - P.second, 1, mod);
			if (Res.second < 0) {
				while (Res.second < 0) {
					Res.second += mod;
				}
			}
			P.first = Res.first;
			P.second = Res.second;
		}
	}
	else {
		Res.first = P.first;
		Res.second = P.second;
	}
	return Res;
}
std::pair<num::cpp_int, num::cpp_int> Mul(std::pair<num::cpp_int, num::cpp_int> P, num::cpp_int mod, num::cpp_int a, num::cpp_int m) {
	if (m < 0) {
		m *= -1;
		P.second *= -1;
	}
	std::vector<int> vec;
	std::pair<num::cpp_int, num::cpp_int> temp, Res;
	DecToBin(m, vec);
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i] == 1) {
			temp = Double(P, mod, a, num::pow(static_cast<num::cpp_int>(2), vec.size() - 1 - i));
			if (i < 1) {
				Res = Double(P, mod, a, num::pow(static_cast<num::cpp_int>(2), vec.size() - 1 - i));
			}
			else {
				Res = Sum(Res, temp, mod);
			}
		}
	}
	return Res;
}
void alphabet_generation() {
	for (auto i : symbols) {
		alphabet.push_back(i);
	}
	for (auto i : alphabet) {
		alphabet_coordinates.push_back(Mul(base_point, p, a, static_cast<int>(i)));
	}
}
std::vector<std::pair<num::cpp_int, num::cpp_int>> text_coordinates_generation(std::vector<unsigned char> text) {
	std::vector<std::pair<num::cpp_int, num::cpp_int>> text_coordinates(text.size());
	for (int i = 0; i < text.size(); i++) {
		for (int j = 0; j < alphabet.size(); j++) {
			if (text[i] == alphabet[j]) {
				text_coordinates[i] = alphabet_coordinates[j];
			}
		}
	}
	return text_coordinates;
}
void domain_parameters_length_selection() {
	while (true) {
		std::cout << "Select parameters length pls:" << std::endl
			<< "1 < 192 bit" << std::endl
			<< "2 < 224 bit" << std::endl
			<< "3 < 256 bit" << std::endl
			<< "4 < 384 bit" << std::endl
			<< "5 < 521 bit" << std::endl
			<< "6 < test case" << std::endl;
		char ans = getchar();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (ans) {
		case '1': {//2^192
			p = static_cast<num::cpp_int>("0xfffffffffffffffffffffffffffffffeffffffffffffffff");
			a = static_cast<num::cpp_int>("0xfffffffffffffffffffffffffffffffefffffffffffffffc");
			b = static_cast<num::cpp_int>("0x64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1");
			Px = static_cast<num::cpp_int>("0x188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012");
			Py = static_cast<num::cpp_int>("0x07192b95ffc8da78631011ed6b24cdd573f977a11e794811");
			n = static_cast<num::cpp_int>("0xffffffffffffffffffffffff99def836146bc9b1b4d22831");
			base_point.first = Px;
			base_point.second = Py;
			break;
		}
		case '2': {//2^224
			p = static_cast<num::cpp_int>("0xffffffffffffffffffffffffffffffff000000000000000000000001");
			a = static_cast<num::cpp_int>("0xfffffffffffffffffffffffffffffffefffffffffffffffffffffffe");
			b = static_cast<num::cpp_int>("0xb4050a850c04b3abf54132565044b0b7d7bfd8ba270b39432355ffb4");
			Px = static_cast<num::cpp_int>("0xb70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21");
			Py = static_cast<num::cpp_int>("0xbd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34");
			n = static_cast<num::cpp_int>("0xffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d");
			base_point.first = Px;
			base_point.second = Py;
			break;
		}
		case '3': {//2^256
			p = static_cast<num::cpp_int>("0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff");
			a = static_cast<num::cpp_int>("0xffffffff00000001000000000000000000000000fffffffffffffffffffffffc");
			b = static_cast<num::cpp_int>("0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b");
			Px = static_cast<num::cpp_int>("0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
			Py = static_cast<num::cpp_int>("0x4fe342e2fe1a7F9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
			n = static_cast<num::cpp_int>("0xffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
			base_point.first = Px;
			base_point.second = Py;
			break;
		}
		case '4': {//2^384
			p = static_cast<num::cpp_int>("0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff");
			a = static_cast<num::cpp_int>("0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc");
			b = static_cast<num::cpp_int>("0xb3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef");
			Px = static_cast<num::cpp_int>("0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7");
			Py = static_cast<num::cpp_int>("0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f");
			n = static_cast<num::cpp_int>("0xffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973");
			base_point.first = Px;
			base_point.second = Py;
			break;
		}
		case '5': {//2^521
			p = static_cast<num::cpp_int>("0x01ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
			a = static_cast<num::cpp_int>("0x01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc");
			b = static_cast<num::cpp_int>("0x0051953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00");
			Px = static_cast<num::cpp_int>("0x00c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5E77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66");
			Py = static_cast<num::cpp_int>("0x011839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650");
			n = static_cast<num::cpp_int>("0x01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9B8899c47aebb6fb71e91386409");
			base_point.first = Px;
			base_point.second = Py;
			break;
		}
		case '6': {//test(751)
			p = static_cast<num::cpp_int>(751);
			a = static_cast<num::cpp_int>(-1);
			b = static_cast<num::cpp_int>(1);
			Px = static_cast<num::cpp_int>(0);
			Py = static_cast<num::cpp_int>(1);
			n = static_cast<num::cpp_int>(13);
			base_point.first = Px;
			base_point.second = Py;
			break;
		}
		default: {
			std::cout << "Incorrect input. Press y to try again or n to close the program" << std::endl;
			char q = getchar();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (q == 'y') {
				system("cls");
				continue;
			}
			else if (q == 'n') {
				std::cout << "Good luck then. " << std::endl;
				exit(0);
			}
			else {
				std::cout << "Still incorrect. Bye." << std::endl;
				exit(0);
			}
		}
		}
		break;
	}
}
int randomizer_generator() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> uid(1, std::pow(2, 16));
	return uid(gen);
}
std::pair<num::cpp_int, num::cpp_int> public_key_generation(num::cpp_int secret_key) {
	return Mul(base_point, p, a, secret_key);
}
void Encryption(std::vector<unsigned char> text, std::vector<int> random_values, std::pair<num::cpp_int, num::cpp_int> public_key, std::vector<std::pair<num::cpp_int, num::cpp_int>>& kP_coordinates, std::vector<std::pair<num::cpp_int, num::cpp_int>>& R_coordinates) {
	std::vector<std::pair<num::cpp_int, num::cpp_int>> text_coordinates = text_coordinates_generation(text);
	for (int i = 0; i < text_coordinates.size(); i++) {
		kP_coordinates[i] = Mul(base_point, p, a, random_values[i]);
		R_coordinates[i] = Sum(text_coordinates[i], Mul(public_key, p, a, random_values[i]), p);
	}
	std::cout << "kP = " << std::endl;
	for (int i = 0; i < kP_coordinates.size(); i++) {
		std::cout << kP_coordinates[i].first << " " << kP_coordinates[i].second << std::endl;
	}
	std::cout << "R = " << std::endl;
	for (int i = 0; i < R_coordinates.size(); i++) {
		std::cout << R_coordinates[i].first << " " << R_coordinates[i].second << std::endl;
	}
}
std::vector<unsigned char> Decryption(std::vector<std::pair<num::cpp_int, num::cpp_int>> kP_coordinates, std::vector<std::pair<num::cpp_int, num::cpp_int>> R_coordinates, num::cpp_int secret_key) {
	std::vector<std::pair<num::cpp_int, num::cpp_int>> Temp;
	std::vector<unsigned char> Res;
	bool check;
	for (int i = 0; i < R_coordinates.size(); i++) {
		Temp.push_back(Sub(R_coordinates[i], Mul(kP_coordinates[i], p, a, secret_key), p));
	}
	for (int i = 0; i < Temp.size(); i++) {
		for (int j = 0; j < alphabet_coordinates.size(); j++) {
			if (Temp[i] == alphabet_coordinates[j]) {
				Res.push_back(alphabet[j]);
				check = true;
				break;
			}
			else {
				check = false;
			}
		}
		if (!check) {
			std::cout << "Wrong coordinates.";
			break;
		}
	}
	for (int i = 0; i < Temp.size(); i++) {
		std::cout << Temp[i].first << " " << Temp[i].second << std::endl;
	}
	return Res;
}
void write_to_file(std::vector<std::pair<num::cpp_int, num::cpp_int>> vec, std::string file_name) {
	std::fstream file(file_name, std::ios::out);
	if (file.is_open()) {
		for (auto i : vec) {
			file << i.first << " " << i.second << " ";
		}
		file.close();
	}
	else {
		std::cout << file_name << " was not open, fix the problem and try again" << std::endl;
		exit(0);
	}
}
std::vector<std::pair<num::cpp_int, num::cpp_int>> read_from_file(std::string file_name) {
	std::fstream file(file_name, std::ios::in);
	if (file.is_open()) {
		if (file.peek() != EOF) {
			std::vector<num::cpp_int> vec;
			num::cpp_int temp;
			while (file) {
				file >> temp;
				vec.push_back(temp);
			}
			vec.erase(vec.end() - 1);
			std::vector<std::pair<num::cpp_int, num::cpp_int>> result(vec.size() / 2);
			for (int i = 0, j = 0; i < vec.size(), j < vec.size(); i++, j++) {
				result[i].first = vec[j];
				j++;
				result[i].second = vec[j];
			}
			std::cout << std::endl;
			file.close();
			return result;
		}
		else {
			std::cout << file_name << " is empty, fix the problem and try again" << std::endl;
			exit(0);
		}
	}
	else {
		std::cout << file_name << " was not open, fix the problem and try again" << std::endl;
		exit(0);
	}
}
void key_input_and_check(long long& secret_key) {
	while (true) {
		std::cout << "Enter secret key < ";
		std::cin >> secret_key;
		if (std::cin.good()) {
			break;
		}
		else {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Incorrect input. Secret key must be a number. Press any key to try again or press 'n' to close the program: ";
			char ans = std::getchar();
			if (ans == 'n' or ans == 'N') {
				std::cout << "Good luck then." << std::endl;
				exit(0);
			}
			else {
				continue;
			}
		}
	}
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	while (true) {
		domain_parameters_length_selection();
		alphabet_generation();
		while (true) {
			std::cout << "What do u wanna do?" << std::endl
				<< "1 < Encryption" << std::endl
				<< "2 < Decryption" << std::endl;
			char ans = getchar();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			switch (ans) {//Encryption/Decryption
			//Encryption
			case '1': {
				std::string text_st;
				std::vector<unsigned char> text;
				std::fstream create("text.txt", std::ios::out);
				if (create.is_open()) {
					create.close();
					std::cout << "File for text was create. Enter text and save the file. Press any key to continue: " << std::endl;
					char answer = _getch();
				}
				else {
					std::cout << "Can't create a file for text, fix the problem and try again." << std::endl;
					exit(0);
				}
				//text file opening and checking 
				while (true) {
					system("text.txt");
					std::fstream t("text.txt", std::ios::in);
					if (t.is_open()) {
						if (t.peek() == EOF) {
							std::cout << "File is empty. Press any key to try again or press 'n' to exit: ";
							char answer = _getch();
							if (answer == 'n' or answer == 'N') {
								std::cout << "Good luck then." << std::endl;
								exit(0);
							}
							else {
								continue;
							}
						}
						else {
							text_st.assign((std::istreambuf_iterator<char>(t.rdbuf())), std::istreambuf_iterator<char>());
							t.close();
							break;
						}
					}
					else {
						std::cout << "File with text wasn't open, fix the problem and try again." << std::endl;
						exit(0);
					}
				}
				std::cout << text_st << std::endl;
				for (auto i : text_st) {
					text.push_back(i);
				}
				std::vector<std::pair<num::cpp_int, num::cpp_int>> text_coordinates(text.size()), kP_coordinates(text.size()), R_coordinates(text.size());
				std::vector<int> random_values;

				long long secret_key;
				key_input_and_check(secret_key);
				std::pair<num::cpp_int, num::cpp_int> public_key = public_key_generation(secret_key);
				for (int i = 0; i < text.size(); i++) {
					random_values.push_back(randomizer_generator());
				}
				Encryption(text, random_values, public_key, kP_coordinates, R_coordinates);
				write_to_file(kP_coordinates, "kP.txt");
				write_to_file(R_coordinates, "R.txt");
				break;
			}
			//Decryption
			case '2': {
				long long secret_key;
				key_input_and_check(secret_key);
				std::vector<std::pair<num::cpp_int, num::cpp_int>> kP = read_from_file("kP.txt");
				std::vector<std::pair<num::cpp_int, num::cpp_int>> R = read_from_file("R.txt");
				std::vector<unsigned char> Result = Decryption(kP, R, secret_key);
				std::fstream out("decipher.txt", std::ios::out);
				if (out.is_open()) {
					for (auto i : Result) {
						out << i;
					}
					out.close();
				}
				else {
					std::cout << "File for decipher was not open, fix the problem and try again" << std::endl;
					exit(0);
				}
				std::cout << std::endl;
				std::copy(Result.begin(), Result.end(), std::ostream_iterator<unsigned char>(std::cout));
				break;
			}
			default: {
				std::cout << "Incorrect input";
				break;
			}
			}
			std::cout << std::endl;
			break;
		}
		std::cout << "Again? (y/n) ";
		char repeat_answer = getchar();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (repeat_answer == 'y' or repeat_answer == 'Y') {
			system("cls");
			continue;
		}
		else if (repeat_answer == 'n' or repeat_answer == 'N') {
			std::cout << "Good luck then." << std::endl;
			break;
		}
		else {
			std::cout << "Incorrect input" << std::endl;
			exit(0);
		}
	}
return 0;
}