
#include <QtCore/QtCore>
#include <QtGui/QtGui>

#include <memory>
#include <fstream>
#include <iostream>
#include <filesystem>

class Throw {};

inline bool pass_jpg_image(std::ifstream &varImage, std::streamsize * varPos = nullptr)try {
	if (varPos) { *varPos = 0; }
	{
		//0xFFD8
		if (varImage.good()) {
			if (0x00ff != varImage.get()) { throw Throw{}; }
		}
		else { throw Throw{}; }
		if (varImage.good()) {
			if (0x00d8 != varImage.get()) { throw Throw{}; }
		}
		else { throw Throw{}; }
	}
	bool varHaveReadFFDA = false;
	while (varImage.good()) {
		if (varHaveReadFFDA) {
		next_while:
			while (varImage.good()) {
				if (0x00ff != varImage.get()) { continue; }
				if (0x00D9 == varImage.get()) {
					if (varPos) { *varPos = varImage.gcount(); }
					return varImage.good();
				}
			}
			goto next_while;
		}
		else {
			/*it should start with 0xff*/
			if (0x00ff != varImage.get()) {
				throw Throw{};
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
			if (varLength < 1) { throw Throw{}; }
			if (varLength > 0x00ffff) { throw Throw{}; }
			varImage.seekg(varLength, std::ios::cur);
		}
	}

	return false;
}
catch (...) {
	return false;
}

class MainState {
public:
	bool isMarked = false;
	std::streamsize markedFileSize = 0;
	enum class ImageType : unsigned char {
		JPG = 0,
		PNG = 1,
	};
	ImageType imageType = ImageType::JPG;
};

int main(int argc, char *argv[]) try {

	if (argc < 2) {
		std::cout << "do not have filename" << std::endl;
		return -1;
	}

	auto varState = std::make_shared<MainState>();
	do try {
		std::ifstream varImageFile(argv[1], std::ios::binary);
		if (varImageFile.is_open() == false) {
			std::cout << "can not open file" << argv[1] << std::endl;
			return -1;
		}
		varState->isMarked = pass_jpg_image(varImageFile, &(varState->markedFileSize));
		if (varState->isMarked == false) { break; }
		/*0x55, 0xaa 0x1f, 0x88
		0 : jpg
		1 : png
		*/
		if (0x0055 != varImageFile.get()) { throw Throw{}; }
		if (0x00aa != varImageFile.get()) { throw Throw{}; }
		if (0x001f != varImageFile.get()) { throw Throw{}; }
		if (0x0088 != varImageFile.get()) { throw Throw{}; }
		varState->imageType = static_cast<MainState::ImageType>(varImageFile.get());
		if (varImageFile.good() == false) { throw Throw{}; }
	}
	catch (...) { varState->isMarked = false; }while (false);

	using ImageType = MainState::ImageType;
	if (varState->isMarked) {
		/* we just remove market , 并重命名 */
		{
			std::fstream varImageFile(argv[1], std::ios::binary | std::ios::in | std::ios::out);
			varImageFile.seekg(varState->markedFileSize + 4, std::ios::beg);
			const auto varImageType = static_cast<ImageType>(varImageFile.get());
			for (;;) {
				const auto varEle = varImageFile.get();
				if (false == varImageFile.good()) { break; }
				varImageFile.put(varEle);
			}
		}
		namespace fs = std::filesystem;
		fs::path varImagePath{ argv[1] };
		fs::path varImagePathNew = varImagePath;
		fs::resize_file(varImagePath, varState->markedFileSize)/*remove the data do not used*/;
		/*rename the file*/
		switch (varState->imageType) {
		case MainState::ImageType::JPG: { varImagePathNew.replace_extension("jpg"); }break;
		case MainState::ImageType::PNG: { varImagePathNew.replace_extension("png"); }break;
		}
		fs::rename(varImagePath, varImagePathNew);
	}
	else {
		QGuiApplication Application(argc, argv);
		const auto varImageFileName = QString::fromLocal8Bit(argv[1]);
		QImage varImage{ varImageFileName };
		int varImageWidth;
		int varImageHeight;
		if ((varImageWidth = varImage.width()) < 1) {
			std::cout << "the image is null" << std::endl;
			throw Throw{};
		}
		if ((varImageHeight = varImage.height()) < 1) {
			std::cout << "the image is null" << std::endl;
			throw Throw{};
		}
		QImage varMarkImage{ varImageWidth,varImageHeight,QImage::Format_Grayscale8 };
		auto varFontSize = std::max(1, std::min(varImageHeight, varImageWidth) / 3);
		{
			QPainter varPainter{ &varMarkImage };
			{
				auto varTextFont = varPainter.font();
				varTextFont.setPixelSize(varFontSize);
				varPainter.setFont(varTextFont);
			}
			const static auto varMarkString = QString::fromUtf8(u8R"(作废)");
			const auto varFontMetrics = varPainter.fontMetrics();
			varPainter.drawText(QPoint(
				std::max(0, (varImageWidth - varFontMetrics.width(varMarkString)) / 2),
				std::max(0, (varImageHeight - varFontSize) / 2) + varFontSize),
				varMarkString);
		}
		
	}

}
catch (...) {
	std::cout << "exception" << std::endl;
	return -1;
}

