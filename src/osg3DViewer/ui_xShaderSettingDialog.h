/********************************************************************************
** Form generated from reading UI file 'xShaderSettingDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XSHADERSETTINGDIALOG_H
#define UI_XSHADERSETTINGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_xShaderSettingDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *labelVertexShader;
    QLineEdit *lineEdit_vertexShader;
    QLabel *label_2;
    QLineEdit *lineEdit_fragmentShader;
    QPushButton *pushButton_vertexShader;
    QPushButton *pushButton_fragmentShader;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *xShaderSettingDialog)
    {
        if (xShaderSettingDialog->objectName().isEmpty())
            xShaderSettingDialog->setObjectName(QString::fromUtf8("xShaderSettingDialog"));
        xShaderSettingDialog->resize(400, 122);
        verticalLayout = new QVBoxLayout(xShaderSettingDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        labelVertexShader = new QLabel(xShaderSettingDialog);
        labelVertexShader->setObjectName(QString::fromUtf8("labelVertexShader"));

        gridLayout->addWidget(labelVertexShader, 0, 0, 1, 1);

        lineEdit_vertexShader = new QLineEdit(xShaderSettingDialog);
        lineEdit_vertexShader->setObjectName(QString::fromUtf8("lineEdit_vertexShader"));

        gridLayout->addWidget(lineEdit_vertexShader, 0, 1, 1, 1);

        label_2 = new QLabel(xShaderSettingDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_fragmentShader = new QLineEdit(xShaderSettingDialog);
        lineEdit_fragmentShader->setObjectName(QString::fromUtf8("lineEdit_fragmentShader"));

        gridLayout->addWidget(lineEdit_fragmentShader, 1, 1, 1, 1);

        pushButton_vertexShader = new QPushButton(xShaderSettingDialog);
        pushButton_vertexShader->setObjectName(QString::fromUtf8("pushButton_vertexShader"));

        gridLayout->addWidget(pushButton_vertexShader, 0, 2, 1, 1);

        pushButton_fragmentShader = new QPushButton(xShaderSettingDialog);
        pushButton_fragmentShader->setObjectName(QString::fromUtf8("pushButton_fragmentShader"));

        gridLayout->addWidget(pushButton_fragmentShader, 1, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(xShaderSettingDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(xShaderSettingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), xShaderSettingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), xShaderSettingDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(xShaderSettingDialog);
    } // setupUi

    void retranslateUi(QDialog *xShaderSettingDialog)
    {
        xShaderSettingDialog->setWindowTitle(QApplication::translate("xShaderSettingDialog", "Shader Setting", 0, QApplication::UnicodeUTF8));
        labelVertexShader->setText(QApplication::translate("xShaderSettingDialog", "Vertex Shader:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("xShaderSettingDialog", "Fragment Shader:", 0, QApplication::UnicodeUTF8));
        pushButton_vertexShader->setText(QApplication::translate("xShaderSettingDialog", "...", 0, QApplication::UnicodeUTF8));
        pushButton_fragmentShader->setText(QApplication::translate("xShaderSettingDialog", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class xShaderSettingDialog: public Ui_xShaderSettingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XSHADERSETTINGDIALOG_H
