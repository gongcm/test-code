1、使用oleview工具查找ActiveX空间的clsid
OLEVIEW.EXE
clsid:{00020906-0000-0000-C000-000000000046}

2、使用qt的dumpcpp.exe工具导出类定义
dumpcpp.exe {00020906-0000-0000-C000-000000000046}
导出了word.cpp word.h

3、修改word.h，添加enum类定义
namespace Office {
    enum MsoArrowheadStyle{
		msoArrowheadDiamond           =0x5,
		msoArrowheadNone              =0x1,
		msoArrowheadOpen              =0x3,
		msoArrowheadOval              =0x6,
		msoArrowheadStealth           =0x4,
		msoArrowheadStyleMixed        =-2 ,
		msoArrowheadTriangle          =0x2,
	};

	enum MsoArrowheadLength{
		msoArrowheadLengthMedium      =0x2,
		msoArrowheadLengthMixed       =-2 ,
		msoArrowheadLong              =0x3,
		msoArrowheadShort             =0x1,
	};

	enum MsoAlignCmd{
		msoAlignBottoms               =0x5,
		msoAlignCenters               =0x1,
		msoAlignLefts                 =0x0,
		msoAlignMiddles               =0x4,
		msoAlignRights                =0x2,
		msoAlignTops                  =0x3,
	};
	
	......
}

4、把word.cpp word.h添加到项目

#include <QtGui/QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include "word.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//创建一个新文档
	Word::Document *doc = new Word::Document();
	if(!doc){
		QMessageBox::warning(NULL, QString("Warning"),\
			QString("Fail to Word::Application()")\
		);
		return 0;
	}

	//打印当前文档名
	qDebug()<< "RET: " << doc->Name();

	//向文档中写入信息
	doc->Range()->SetText("hello world office word 2003");

	//保存文档
	QVariant filename = QVariant(QDir::currentPath()+"/a.doc");
	doc->SaveAs(filename);

	//关闭文档
	doc->Close();

	delete doc;

	return 0;
}