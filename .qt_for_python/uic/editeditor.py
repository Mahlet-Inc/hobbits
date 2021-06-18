# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/benjamin/Desktop/Development/work/hobbits/src/hobbits-plugins/operators/Edit/editeditor.ui'
#
# Created by: PyQt5 UI code generator 5.10.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_EditEditor(object):
    def setupUi(self, EditEditor):
        EditEditor.setObjectName("EditEditor")
        EditEditor.resize(604, 156)
        self.verticalLayout = QtWidgets.QVBoxLayout(EditEditor)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.verticalLayout_3 = QtWidgets.QVBoxLayout()
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.rb_bit = QtWidgets.QRadioButton(EditEditor)
        self.rb_bit.setChecked(True)
        self.rb_bit.setObjectName("rb_bit")
        self.verticalLayout_3.addWidget(self.rb_bit)
        self.rb_hex = QtWidgets.QRadioButton(EditEditor)
        self.rb_hex.setObjectName("rb_hex")
        self.verticalLayout_3.addWidget(self.rb_hex)
        self.rb_ascii = QtWidgets.QRadioButton(EditEditor)
        self.rb_ascii.setObjectName("rb_ascii")
        self.verticalLayout_3.addWidget(self.rb_ascii)
        self.horizontalLayout_3.addLayout(self.verticalLayout_3)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout()
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.horizontalLayout_4 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.lb_start = QtWidgets.QLabel(EditEditor)
        self.lb_start.setObjectName("lb_start")
        self.horizontalLayout_4.addWidget(self.lb_start)
        self.sb_start = QtWidgets.QSpinBox(EditEditor)
        self.sb_start.setObjectName("sb_start")
        self.horizontalLayout_4.addWidget(self.sb_start)
        self.verticalLayout_2.addLayout(self.horizontalLayout_4)
        self.horizontalLayout_5 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.lb_length = QtWidgets.QLabel(EditEditor)
        self.lb_length.setObjectName("lb_length")
        self.horizontalLayout_5.addWidget(self.lb_length)
        self.sb_length = QtWidgets.QSpinBox(EditEditor)
        self.sb_length.setObjectName("sb_length")
        self.horizontalLayout_5.addWidget(self.sb_length)
        self.verticalLayout_2.addLayout(self.horizontalLayout_5)
        self.horizontalLayout_3.addLayout(self.verticalLayout_2)
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_3.addItem(spacerItem)
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        self.pte_bits = QtWidgets.QPlainTextEdit(EditEditor)
        self.pte_bits.setObjectName("pte_bits")
        self.verticalLayout.addWidget(self.pte_bits)

        self.retranslateUi(EditEditor)
        QtCore.QMetaObject.connectSlotsByName(EditEditor)

    def retranslateUi(self, EditEditor):
        _translate = QtCore.QCoreApplication.translate
        EditEditor.setWindowTitle(_translate("EditEditor", "Form"))
        self.rb_bit.setText(_translate("EditEditor", "Bit"))
        self.rb_hex.setText(_translate("EditEditor", "Hex"))
        self.rb_ascii.setText(_translate("EditEditor", "Ascii"))
        self.lb_start.setText(_translate("EditEditor", "Bit Start:"))
        self.lb_length.setText(_translate("EditEditor", "Bit Length:"))

