#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpinBox>

#include "mainwidget.h"
#include "paintarea.h"

MainWidget::MainWidget(QWidget *parent)
	: QWidget(parent)
{
    area = new PaintArea(this);
   
    ctrlFrame = new QFrame(this);
    createCtrlWidget();
    ctrlFrame->setMinimumSize(120,this->height());
    ctrlFrame->setMaximumSize(120,this->height());

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(area);
    mainLayout->addWidget(ctrlFrame);
    setLayout(mainLayout);

    this->setMinimumSize(640, 480);
    
    setWindowTitle(tr("Painter Transformation"));   
}
 
void MainWidget::createCtrlWidget()
{
    //  旋转控制
    QGroupBox *rotateGroup = new QGroupBox(tr("Rotate"));
    QSlider *rotateSlider = new QSlider;
    rotateSlider->setOrientation(Qt::Horizontal);
    rotateSlider->setRange(0,360*2);
    rotateSlider->setValue(0);
    connect(rotateSlider,SIGNAL(valueChanged(int)),this,SLOT(slotRotate(int)));
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(rotateSlider);
    rotateGroup->setLayout(l1);
        
    //  缩放控制
    QGroupBox *scaleGroup = new QGroupBox(tr("Scale"));
    QSlider *scaleSlider = new QSlider;
    scaleSlider->setOrientation(Qt::Horizontal);
    scaleSlider->setRange(1,200);
    scaleSlider->setValue(50);
    connect(scaleSlider,SIGNAL(valueChanged(int)),this,SLOT(slotScale(int)));
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->addWidget(scaleSlider);
    scaleGroup->setLayout(l2);   
   
    //  平移控制
    QGroupBox *translateGroup = new QGroupBox(tr("Translate"));
    QSlider *translateSlider = new QSlider;
    translateSlider->setOrientation(Qt::Horizontal);
    translateSlider->setRange(0,199);
    translateSlider->setValue(99);
    connect(translateSlider,SIGNAL(valueChanged(int)),this,SLOT(slotTranslate(int)));
    QHBoxLayout *l3 = new QHBoxLayout;
    l3->addWidget(translateSlider);
    translateGroup->setLayout(l3);

    // 扭曲控制
    QGroupBox *shearGroup = new QGroupBox(tr("Shear"));
    QSlider *shearSlider = new QSlider;
    shearSlider->setOrientation(Qt::Horizontal);
    shearSlider->setRange(0,100);
    shearSlider->setValue(0);
    connect(shearSlider,SIGNAL(valueChanged(int)),this,SLOT(slotShear(int)));
    QHBoxLayout *l4 = new QHBoxLayout;
    l4->addWidget(shearSlider);
    shearGroup->setLayout(l4);
    
    QVBoxLayout *ctrlLayout = new QVBoxLayout;
    ctrlLayout->addWidget(rotateGroup);
    ctrlLayout->addWidget(scaleGroup);
    ctrlLayout->addWidget(translateGroup);
    ctrlLayout->addWidget(shearGroup);
    ctrlLayout->addStretch();
    ctrlFrame->setLayout(ctrlLayout);
}

void MainWidget::slotRotate(int value)
{
    area->setRotate(value);
}

void MainWidget::slotScale(int value)
{
    area->setScale(value);
}

void MainWidget::slotTranslate(int value)
{
    area->setTranslate(value);
}

void MainWidget::slotShear(int value)
{
    area->setShear(value);
}