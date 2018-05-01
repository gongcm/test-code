#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    this->connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));
}

Form::~Form()
{
    delete ui;
}
