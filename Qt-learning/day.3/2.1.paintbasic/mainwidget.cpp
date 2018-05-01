#include <QFont>
#include <QColor>
#include <QColorDialog>
#include <QColor>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    QFont font("ZYSong18030",12);
    setFont(font);
    
    area = new PaintArea;

    QLabel *label1 = new QLabel(tr("Shape:"));
    QLabel *label2 = new QLabel(tr("Pen Width:"));
    QLabel *label3 = new QLabel(tr("Pen Color:"));
    QLabel *label4 = new QLabel(tr("Pen Style:"));
    QLabel *label5 = new QLabel(tr("Pen Cap:"));
    QLabel *label6 = new QLabel(tr("Pen Join:"));
    QLabel *label7 = new QLabel(tr("Brush:"));

    shapeComboBox = new QComboBox;
    shapeComboBox->addItem(tr("Line"), PaintArea::Line);
    shapeComboBox->addItem(tr("Polygon"), PaintArea::Polygon);
    shapeComboBox->addItem(tr("Rectangle"), PaintArea::Rectangle);
    shapeComboBox->addItem(tr("Round Rectangle"), PaintArea::RoundRect);
    shapeComboBox->addItem(tr("Ellipse"), PaintArea::Ellipse);
    shapeComboBox->addItem(tr("Path"), PaintArea::Path);
    shapeComboBox->addItem(tr("Polyline"), PaintArea::Polyline);
    shapeComboBox->addItem(tr("Arc"), PaintArea::Arc);
    shapeComboBox->addItem(tr("Points"), PaintArea::Points);
    shapeComboBox->addItem(tr("Text"), PaintArea::Text);
    shapeComboBox->addItem(tr("Pixmap"), PaintArea::Pixmap);
    connect(shapeComboBox,SIGNAL(activated(int)),this,SLOT(slotShape(int)));

    widthSpinBox = new QSpinBox;
    widthSpinBox->setRange(0,20);
    connect(widthSpinBox,SIGNAL(valueChanged(int)),this,SLOT(slotPenWidth(int)));

    colorFrame = new QFrame;
    colorFrame->setAutoFillBackground(true);
    colorFrame->setPalette(QPalette(Qt::blue));
    QPushButton *colorPushButton = new QPushButton(tr("Change"));
    connect(colorPushButton,SIGNAL(clicked()),this,SLOT(slotPenColor()));

    penStyleComboBox = new QComboBox;
    /*
Qt::NoPen	0	no line at all. For example, QPainter::drawRect() fills but does not draw any boundary line.
Qt::SolidLine	1	A plain line.
Qt::DashLine	2	Dashes separated by a few pixels.
Qt::DotLine	3	Dots separated by a few pixels.
Qt::DashDotLine	4	Alternate dots and dashes.
Qt::DashDotDotLine	5	One dash, two dots, one dash, two dots.
Qt::CustomDashLine	6	A custom pattern defined using QPainterPathStroker::setDashPattern().
   */
    penStyleComboBox->addItem("NoPen");
    penStyleComboBox->addItem("SolidLine");
    penStyleComboBox->addItem("DashLine");
    penStyleComboBox->addItem("DotLine");
    penStyleComboBox->addItem("DashDotLine");
    penStyleComboBox->addItem("DashDotDotLine");
    penStyleComboBox->addItem("CustomDashLine");
    connect(penStyleComboBox,SIGNAL(activated(int)),this,SLOT(slotPenStyle(int)));

    penCapComboBox = new QComboBox;
    /*
Qt::FlatCap	0x00	a square line end that does not cover the end point of the line.
Qt::SquareCap	0x10	a square line end that covers the end point and extends beyond it by half the line width.
Qt::RoundCap	0x20	a rounded line end.
    */
    penCapComboBox->addItem(tr("Flat"));
    penCapComboBox->addItem(tr("Square"));
    penCapComboBox->addItem(tr("Round"));
    connect(penCapComboBox,SIGNAL(activated(int)),this,SLOT(slotPenCap(int)));

    penJoinComboBox = new QComboBox;
    /*
Qt::MiterJoin	0x00	The outer edges of the lines are extended to meet at an angle, and this area is filled.
Qt::BevelJoin	0x40	The triangular notch between the two lines is filled.
Qt::RoundJoin	0x80	A circular arc between the two lines is filled.
Qt::SvgMiterJoin	0x100	A miter join corresponding to the definition of a miter join in the SVG 1.2 Tiny specification.
     */
    penJoinComboBox->addItem(tr("Miter"));
    penJoinComboBox->addItem(tr("Bevel"));
    penJoinComboBox->addItem(tr("Round"));
    penJoinComboBox->addItem(tr("SvgMiterJoin"));
    connect(penJoinComboBox,SIGNAL(activated(int)),this,SLOT(slotPenJoin(int)));

    brushStyleComboBox = new QComboBox;
/*
Qt::NoBrush	0	No brush pattern.
Qt::SolidPattern	1	Uniform color.
Qt::Dense1Pattern	2	Extremely dense brush pattern.
Qt::Dense2Pattern	3	Very dense brush pattern.
Qt::Dense3Pattern	4	Somewhat dense brush pattern.
Qt::Dense4Pattern	5	Half dense brush pattern.
Qt::Dense5Pattern	6	Somewhat sparse brush pattern.
Qt::Dense6Pattern	7	Very sparse brush pattern.
Qt::Dense7Pattern	8	Extremely sparse brush pattern.
Qt::HorPattern	9	Horizontal lines.
Qt::VerPattern	10	Vertical lines.
Qt::CrossPattern	11	Crossing horizontal and vertical lines.
Qt::BDiagPattern	12	Backward diagonal lines.
Qt::FDiagPattern	13	Forward diagonal lines.
Qt::DiagCrossPattern	14	Crossing diagonal lines.
Qt::LinearGradientPattern	15	Linear gradient (set using a dedicated QBrush constructor).
Qt::RadialGradientPattern	16	Radial gradient (set using a dedicated QBrush constructor).
Qt::ConicalGradientPattern	17	Conical gradient (set using a dedicated QBrush constructor).
Qt::TexturePattern	24	Custom pattern (see QBrush::setTexture()).
*/
    brushStyleComboBox->addItem(tr("NoBrush"));
    brushStyleComboBox->addItem(tr("SolidPattern"));
    brushStyleComboBox->addItem(tr("Dense1Pattern"));
    brushStyleComboBox->addItem(tr("Dense2Pattern"));
    brushStyleComboBox->addItem(tr("Dense3Pattern"));
    brushStyleComboBox->addItem(tr("Dense4Pattern"));
    brushStyleComboBox->addItem(tr("Dense5Pattern"));
    brushStyleComboBox->addItem(tr("Dense6Pattern"));
    brushStyleComboBox->addItem(tr("Dense7Pattern"));
    brushStyleComboBox->addItem(tr("HorPattern"));
    brushStyleComboBox->addItem(tr("VerPattern"));
    brushStyleComboBox->addItem(tr("CrossPattern"));
    brushStyleComboBox->addItem(tr("BDiagPattern"));
    brushStyleComboBox->addItem(tr("FDiagPattern"));
    brushStyleComboBox->addItem(tr("DiagCrossPattern"));
    brushStyleComboBox->addItem(tr("LinearGradientPattern"));
    brushStyleComboBox->addItem(tr("ConicalGradientPattern"));
    brushStyleComboBox->addItem(tr("RadialGradientPattern"));
    brushStyleComboBox->addItem(tr("TexturePattern"));
    connect(brushStyleComboBox,SIGNAL(activated(int)),this,SLOT(slotBrush(int)));

    // ²¼¾Ö
    QGridLayout *ctrlLayout = new QGridLayout;
    int labelCol=0;
    int contentCol=1;
    ctrlLayout->addWidget(label1,0,labelCol);
    ctrlLayout->addWidget(shapeComboBox,0,contentCol);
    ctrlLayout->addWidget(label2,1,labelCol);
    ctrlLayout->addWidget(widthSpinBox,1,contentCol);
    ctrlLayout->addWidget(label3,2,labelCol);
    ctrlLayout->addWidget(colorFrame,2,contentCol);
    ctrlLayout->addWidget(colorPushButton,2,2);
    ctrlLayout->addWidget(label4,3,labelCol);
    ctrlLayout->addWidget(penStyleComboBox,3,contentCol);
    ctrlLayout->addWidget(label5,4,labelCol);
    ctrlLayout->addWidget(penCapComboBox,4,contentCol);
    ctrlLayout->addWidget(label6,5,labelCol);
    ctrlLayout->addWidget(penJoinComboBox,5,contentCol);
    ctrlLayout->addWidget(label7,6,labelCol);
    ctrlLayout->addWidget(brushStyleComboBox,6,contentCol);
    
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(area);
    mainLayout->addLayout(ctrlLayout);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);    
    
    setWindowTitle(tr("Basic Paint"));    
    
    slotShape(0);
    slotPenWidth(0);
    slotPenStyle(0);
    slotPenCap(0);
    slotPenJoin(0);
    slotBrush(0);
}
   
void MainWidget::slotShape(int value)
{
    PaintArea::Shape shape = PaintArea::Shape(shapeComboBox->itemData(value,  Qt::UserRole).toInt());
    area->setShape(shape);
}

void MainWidget::slotPenWidth(int value)
{
     QColor color = colorFrame->palette().color(QPalette::Window);
     Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->currentIndex());
     int capArr[] = {Qt::FlatCap,Qt::SquareCap,Qt::RoundCap};
     Qt::PenCapStyle cap = Qt::PenCapStyle(capArr[penCapComboBox->currentIndex()]);
     int joinArr[] = {Qt::MiterJoin,Qt::BevelJoin,Qt::RoundJoin,Qt::SvgMiterJoin};
     Qt::PenJoinStyle join = Qt::PenJoinStyle(joinArr[penJoinComboBox->currentIndex()]);

     area->setPen(QPen(color, value, style, cap, join));
}

void MainWidget::slotPenColor()
{
    QColor color = QColorDialog::getColor(Qt::blue);
    
    colorFrame->setPalette(QPalette(color));
    
    int width = widthSpinBox->value();

    Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->currentIndex());
    int capArr[] = {Qt::FlatCap,Qt::SquareCap,Qt::RoundCap};
    Qt::PenCapStyle cap = Qt::PenCapStyle(capArr[penCapComboBox->currentIndex()]);
    int joinArr[] = {Qt::MiterJoin,Qt::BevelJoin,Qt::RoundJoin,Qt::SvgMiterJoin};
    Qt::PenJoinStyle join = Qt::PenJoinStyle(joinArr[penJoinComboBox->currentIndex()]);

    area->setPen(QPen(color, width, style, cap, join));
}

void MainWidget::slotPenStyle(int value)
{
    int width = widthSpinBox->value();
    QColor color = colorFrame->palette().color(QPalette::Window);

    Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->currentIndex());
    int capArr[] = {Qt::FlatCap,Qt::SquareCap,Qt::RoundCap};
    Qt::PenCapStyle cap = Qt::PenCapStyle(capArr[penCapComboBox->currentIndex()]);
    int joinArr[] = {Qt::MiterJoin,Qt::BevelJoin,Qt::RoundJoin,Qt::SvgMiterJoin};
    Qt::PenJoinStyle join = Qt::PenJoinStyle(joinArr[penJoinComboBox->currentIndex()]);

    area->setPen(QPen(color, width, style, cap, join));  
}

void MainWidget::slotPenCap(int value)
{
    int width = widthSpinBox->value();
    QColor color = colorFrame->palette().color(QPalette::Window);

    Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->currentIndex());
    int capArr[] = {Qt::FlatCap,Qt::SquareCap,Qt::RoundCap};
    Qt::PenCapStyle cap = Qt::PenCapStyle(capArr[penCapComboBox->currentIndex()]);
    int joinArr[] = {Qt::MiterJoin,Qt::BevelJoin,Qt::RoundJoin,Qt::SvgMiterJoin};
    Qt::PenJoinStyle join = Qt::PenJoinStyle(joinArr[penJoinComboBox->currentIndex()]);

    area->setPen(QPen(color, width, style, cap, join)); 
}

 
void MainWidget::slotPenJoin(int value)
{
    int width = widthSpinBox->value();
    QColor color = colorFrame->palette().color(QPalette::Window);

    Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->currentIndex());
    int capArr[] = {Qt::FlatCap,Qt::SquareCap,Qt::RoundCap};
    Qt::PenCapStyle cap = Qt::PenCapStyle(capArr[penCapComboBox->currentIndex()]);
    int joinArr[] = {Qt::MiterJoin,Qt::BevelJoin,Qt::RoundJoin,Qt::SvgMiterJoin};
    Qt::PenJoinStyle join = Qt::PenJoinStyle(joinArr[penJoinComboBox->currentIndex()]);

    area->setPen(QPen(color, width, style, cap, join)); 
}

void MainWidget::slotBrush(int value)
{
    Qt::BrushStyle style;
    int id = brushStyleComboBox->currentIndex();
    if(id <= Qt::ConicalGradientPattern){
        style = Qt::BrushStyle(brushStyleComboBox->currentIndex());
    }else{
        style = Qt::TexturePattern;
    }

    if (style == Qt::LinearGradientPattern) {
        QLinearGradient linearGradient(0, 0, 400, 400);
        linearGradient.setColorAt(0.0, Qt::white);
        linearGradient.setColorAt(0.2, Qt::green);
        linearGradient.setColorAt(1.0, Qt::black);
        area->setBrush(linearGradient);
    } else if (style == Qt::RadialGradientPattern) {
        QRadialGradient radialGradient(200, 200, 150, 150, 100);
        radialGradient.setColorAt(0.0, Qt::white);
        radialGradient.setColorAt(0.2, Qt::green);
        radialGradient.setColorAt(1.0, Qt::black);
        area->setBrush(radialGradient);
    } else if (style == Qt::ConicalGradientPattern) {
        QConicalGradient conicalGradient(200, 200, 30);
        conicalGradient.setColorAt(0.0, Qt::white);
        conicalGradient.setColorAt(0.2, Qt::green);
        conicalGradient.setColorAt(1.0, Qt::black);
        area->setBrush(conicalGradient);
    } else if (style == Qt::TexturePattern) {
        area->setBrush(QBrush(QPixmap(":/images/butterfly.png")));
    } else {
        area->setBrush(QBrush(Qt::green, style));
    }
}

