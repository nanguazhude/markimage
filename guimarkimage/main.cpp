extern int _p_main(int argc, char *argv[]);

#include <thread>
#include <QtWidgets/QtWidgets>

class CWidget : public QWidget{
public:
	QHBoxLayout * $Layout = nullptr;
	QLineEdit * $Dir = nullptr;
	QPushButton * $SelectButton = nullptr;
	QPushButton * $DoButton = nullptr;
	CWidget() {
		$Dir = new QLineEdit;
		$Layout = new QHBoxLayout;
		this->setLayout($Layout);
		$Layout->addWidget($Dir);
		$SelectButton = new QPushButton;
		$SelectButton->setText(QString::fromUtf8(u8R"(选择)"));
		$Layout->addWidget($SelectButton);
		$DoButton = new QPushButton;
		$DoButton->setText(QString::fromUtf8(u8R"(执行)"));
		$Layout->addWidget($DoButton);

		connect($DoButton, &QPushButton::clicked,this, &CWidget::do_);
		connect($SelectButton, &QPushButton::clicked, this, &CWidget::select_);

	}

	void do_() {
		const auto varFileName_ = $Dir->text();
		if (varFileName_.isEmpty()) { return; }
		QDir varDir ;
		const auto varFileName__ = varDir.absoluteFilePath( varFileName_ );
		auto varFileName = varFileName__.toLocal8Bit();
		varFileName.push_back(char(0));
		std::thread _run_this_([varFileName]() {
			auto varData = varFileName.begin();
			char * argv[2] = { const_cast<char*>(varData) ,
				const_cast<char*>(varData) };
			_p_main(2, argv);
		});
		_run_this_.detach();

	}

	void select_() {
		const auto varFileName = QFileDialog::getOpenFileName();
		if (varFileName.isEmpty()) { return; }
		$Dir->setText(varFileName);
	}

};

int main(int argc,char ** argv) {

	QApplication varApplication{argc,argv};

	QMainWindow varMainWindow;
	varMainWindow.setCentralWidget( new CWidget );
	varMainWindow.resize({512,64});
	varMainWindow.show();

	return varApplication.exec();
}

