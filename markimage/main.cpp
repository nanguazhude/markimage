#include <QCoreApplication>

#include <fstream>
#include <iostream>

bool pass_jpg_image(std::ifstream &varImage)try {
	{
		//0xFFD8
		if (varImage.good()) {
			if (0x00ff != varImage.get()) { throw; }
		}
		else { throw; }
		if (varImage.good()) {
			if (0x00d8 != varImage.get()) { throw; }
		}
		else { throw; }
	}
	bool varHaveReadFFDA = false;
	while (varImage.good()) {
		if (varHaveReadFFDA) {
		next_while:
			while (varImage.good()) {
				if (0x00ff != varImage.get()) { continue; }
				if (0x00D9 == varImage.get()) {
					return varImage.good();
				}
			}
			goto next_while;
		}
		else {
			/*it should start with 0xff*/
			if (0x00ff != varImage.get()) {
				throw;
			}

			{
				int varCurrentChar;
				while (varImage.good()) {
					/*skip next 0xff*/
					if (0x00ff != (varCurrentChar = varImage.get())) { break; }
				}
				if (varCurrentChar == 0x00DA) {
					varHaveReadFFDA = true;
				}
			}
			/*get data length*/
			std::int32_t varLength = -2;
			{
				const auto varPart0 = (0x00ff & varImage.get());
				const auto varPart1 = (0x00ff & varImage.get());
				varLength += (varPart0 << 8) | (varPart1);
			}
			if (varLength < 1) { throw; }
			if (varLength > 0x00ffff) { throw; }
			varImage.seekg(varLength, std::ios::cur);
		}
	}

	return false;
}
catch (...) {
	return false;
}

int main(int argc, char *argv[]) {
	QCoreApplication Application(argc, argv);

	std::ifstream oxx(u8R"(D:\test1\MyTEstXXX\build_mark_image\test.jpg)", std::ios::binary);
	pass_jpg_image(oxx);

}

