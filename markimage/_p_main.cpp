/*适用于小文件，文件太大会爆内存*/
#include <QtCore/QtCore>
#include <QtGui/QtGui>

#include <vector>
#include <thread>
#include <memory>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <string_view>
using namespace std::string_view_literals;

namespace {

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
					const auto varPart0 = (varImage.get());
					const auto varPart1 = (varImage.get());
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

	inline bool pass_png_image(std::ifstream &varImage, std::streamsize * varPos = nullptr) try {
		if (varPos) { *varPos = 0; }
		{
			//89 50 4E 47 0D 0A 1A 0A
			if (varImage.good()) {
				if (0x0089 != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x0050 != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x004e != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x0047 != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x000D != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x000A != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x001A != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
			if (varImage.good()) {
				if (0x000A != varImage.get()) { throw Throw{}; }
			}
			else { throw Throw{}; }
		}

		bool varIsEndl = false;
		while (varImage.good()) {
			if (varIsEndl) {
				*varPos = varImage.tellg();
				return true;
			}
			std::uint32_t varLength = 4;
			{
				const auto varPart0 = (varImage.get());
				const auto varPart1 = (varImage.get());
				const auto varPart2 = (varImage.get());
				const auto varPart3 = (varImage.get());
				varLength += (varPart0 << 24) | (varPart1 << 16) | (varPart2 << 8) | (varPart3);
			}
			{
				const auto varPart0 = (varImage.get());
				const auto varPart1 = (varImage.get());
				const auto varPart2 = (varImage.get());
				const auto varPart3 = (varImage.get());
				//IEND 
				if ((varPart0 == 'I') && (varPart1 == 'E') && (varPart2 == 'N') && (varPart3 == 'D')) {
					varIsEndl = true;
				}
			}
			varImage.seekg(varLength, std::ios::cur);
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
		enum class ImageType : char {
			JPG = 0,
			PNG = 1,
		};
		ImageType imageType = ImageType::JPG;
	};

}

extern int _p_main(int argc, char *argv[]) try {

	if (argc < 2) {
		std::cout << "do not have filename"sv << std::endl;
		return -1;
	}

	auto varState = std::make_shared<MainState>();
	do try {
		std::ifstream varImageFile(argv[1], std::ios::binary);
		if (varImageFile.is_open() == false) {
			std::cout << "can not open file"sv << argv[1] << std::endl;
			return -1;
		}
		varState->isMarked = pass_png_image(varImageFile, &(varState->markedFileSize));
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
		std::streamsize varHaveRead = 0;
		{
			std::fstream varImageFile(argv[1], std::ios::binary | std::ios::in | std::ios::out);
			const auto varReadBasic = varState->markedFileSize + 5;
			constexpr const static std::size_t varTmpBufferSize = 1024;
			std::vector<char> varTmpBuffer(varTmpBufferSize, '1');
			for (;;) {
				varImageFile.seekg(varHaveRead + varReadBasic, std::ios::beg);
				varImageFile.read(varTmpBuffer.data(), varTmpBufferSize);
				const auto varReadSize = varImageFile.gcount();
				if (varReadSize < 1) {
					break;
				}
				varImageFile.clear();
				varImageFile.seekp(varHaveRead, std::ios::beg);
				varHaveRead += varReadSize;
				varImageFile.write(std::as_const(varTmpBuffer).data(), varReadSize);
				varImageFile.flush();
			}
		}
		namespace fs = std::filesystem;
		fs::path varImagePath{ argv[1] };
		fs::path varImagePathNew = varImagePath;
		fs::resize_file(varImagePath, varHaveRead)/*remove the data do not used*/;
		/*rename the file*/
		switch (varState->imageType) {
		case MainState::ImageType::JPG: { varImagePathNew.replace_extension("jpg"sv); }break;
		case MainState::ImageType::PNG: { varImagePathNew.replace_extension("png"sv); }break;
		}
		fs::rename(varImagePath, varImagePathNew);
	}
	else {
		{
#ifndef QT_WIDGETS_LIB
			QGuiApplication Application(argc, argv);
#endif // !QT_WIDGETS

			const auto varImageFileName = QString::fromLocal8Bit(argv[1]);
			QImage varImage{ varImageFileName };
			
			int varImageWidth;
			int varImageHeight;
			if ((varImageWidth = varImage.width()) < 1) {
				std::cout << "the image is null"sv << std::endl;
				throw Throw{};
			}
			if ((varImageHeight = varImage.height()) < 1) {
				std::cout << "the image is null"sv << std::endl;
				throw Throw{};
			}

			QByteArray varOrigin; 
			std::thread varThreadToSaveData([varOrigin = &varOrigin, varImage = varImage.copy()] () mutable {
				varImage = varImage.convertToFormat(QImage::Format_RGBA8888)/*重新整理数据格式*/;
				QBuffer varBuffer{ varOrigin };
				varBuffer.open(QBuffer::WriteOnly);
				varImage.save(&varBuffer, "png");
				});

			/*被标记的图片仅仅显示缩略图即可，不必太大...*/
			{
				constexpr const static int varMinSize = 1280;
				if (varImageWidth > varMinSize) {
					varImage = varImage.scaledToWidth(varMinSize, Qt::SmoothTransformation);
				}
				if (varImageHeight > varMinSize) {
					varImage = varImage.scaledToHeight(varMinSize, Qt::SmoothTransformation);
				}
				varImageWidth = varImage.width();
				varImageHeight = varImage.height();
				varImage = varImage.convertToFormat(QImage::Format_RGB888);
			}

			{//QImage varMarkImage{ varImageWidth,varImageHeight,QImage::Format_Grayscale8 };
				const auto varFontSize = std::max(1, std::min(varImageHeight, varImageWidth) / 3);
				QPainter varPainter{ &varImage };
				varPainter.setRenderHints(QPainter::HighQualityAntialiasing |
					QPainter::SmoothPixmapTransform |
					QPainter::TextAntialiasing |
					QPainter::Antialiasing);
				{
					auto varTextFont = varPainter.font();
					varTextFont.setPixelSize(varFontSize);
					varPainter.setFont(varTextFont);
				}
				const static auto varMarkString = QString::fromUtf8(u8R"(作废)");
				const auto varFontMetrics = varPainter.fontMetrics();
				{
					auto varPen = varPainter.pen();
					varPen.setColor(QColor(205, 30, 30, 99));
					varPainter.setPen(varPen);
				}
				auto varYHeight = std::max(0, (varImageHeight - varFontSize) / 2) + varFontSize;
				const auto varXWidth = std::max(0, (varImageWidth - varFontMetrics.width(varMarkString)) / 2);
				varPainter.drawText(QPoint(varXWidth, varYHeight),
					varMarkString);
				const auto varFontHeight = std::max(2, varFontSize / 4);
				{
					auto varTextFont = varPainter.font();
					varTextFont.setPixelSize(varFontHeight);
					varPainter.setFont(varTextFont);
				}
				{
					auto varPen = varPainter.pen();
					varPen.setColor(QColor(40, 50, 60, 220));
					varPainter.setPen(varPen);
				}
				varPainter.drawText(QPoint(varXWidth, 1 + varFontHeight + varYHeight),
					QDateTime::currentDateTime().toString(QStringLiteral("yyyy/MM/dd")));
			}

			QByteArray varMarked;

			{
				QBuffer varBuffer{ &varMarked };
				varBuffer.open(QBuffer::WriteOnly);
				varImage.save(&varBuffer, "png");
			}

			/*cleare image data now*/
			varImage = {};

			std::ofstream varOutStream(argv[1], std::ios::binary);
			if (varOutStream.is_open() == false) {
				throw Throw{};
			}
			/*write marked data*/
			varOutStream.write(std::as_const(varMarked).data(), varMarked.size());
			varMarked = {};
			/*write boom*/
			constexpr static const char varBom[] = {
				static_cast<char>(0x55U),
				static_cast<char>(0xaaU),
				static_cast<char>(0x1fU),
				static_cast<char>(0x88U),
				static_cast<char>(MainState::ImageType::PNG) };
			varOutStream.write(varBom, std::size(varBom));
			/*write origin data*/
			if (varThreadToSaveData.joinable()) { varThreadToSaveData.join(); }
			varOutStream.write(std::as_const(varOrigin).data(), varOrigin.size());
			varOrigin = {};
		}
		namespace fs = std::filesystem;
		fs::path varFilePath{ argv[1] };
		if (varFilePath.extension() != "png"sv) {
			varFilePath.replace_extension("png"sv);
			fs::rename(argv[1], varFilePath);
		}
	}
	return 0;
}
catch (...) {
	std::cout << "exception"sv << std::endl;
	return -1;
}

