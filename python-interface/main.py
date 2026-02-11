import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QWidget, QGroupBox, QGridLayout
from PySide6.QtCore import QRect
from PySide6.QtCore import QTimer
import pyqtgraph as pg
from random import randint


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.mainWidget = QWidget(self)
        self.mainWidget.setObjectName("mainWidget")
        self.mainWidget.setWindowTitle("Speed Controller Interface")
        self.mainWidget.resize(1302, 657)
    
        # Create Group Box for Plots
        self.groupBox = QGroupBox(self.mainWidget)
        self.groupBox.setGeometry(QRect(380, 10, 901, 601))
        self.groupBox.setObjectName("groupBox")

        self.gridLayout = QGridLayout(self.groupBox)
        self.gridLayout.setObjectName("gridLayout")

        self.graphWidget = pg.PlotWidget()
        self.graphWidget.setBackground('w')
        self.gridLayout.addWidget(self.graphWidget, 0, 0, 1, 1)

        self.graphWidget.plot([1, 2, 3, 4], [10, 1, 5, 2]) 
        self.gridLayout.addWidget(self.graphWidget, 0, 0, 1, 1)

        self.pv_widget = QWidget(parent=self.groupBox)
        self.pv_widget.setObjectName("pv_widget")
        self.gridLayout.addWidget(self.pv_widget, 0, 1, 1, 1)

        self.op_widget = QWidget(parent=self.groupBox)
        self.op_widget.setObjectName("op_widget")
        self.gridLayout.addWidget(self.op_widget, 1, 0, 1, 1)

        self.error_widget = QWidget(parent=self.groupBox)
        self.error_widget.setObjectName("error_widget")
        self.gridLayout.addWidget(self.error_widget, 1, 1, 1, 1)



if __name__ == "__main__":
    app = QApplication(sys.argv)
    main = MainWindow()
    main.show()
    app.exec()
