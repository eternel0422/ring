# Form/Window View - Generated Source Code File 
# Generated by the Ring 1.15 Form Designer
# Date : 13/03/2021
# Time : 11:55:59

Load "stdlibcore.ring"
Load "guilib.ring"

import System.GUI

if IsMainSourceFile() { 
	new App {
		StyleFusion()
		new frmView { win.show() } 
		exec()
	}
}

class frmView from WindowsViewParent
	win = new MainWindow() { 
		move(0,0)
		resize(1317,586)
		setWindowTitle("Chess Endgame Data Set")
		setstylesheet("background-color:;") 

		setWinIcon(win,"images/WKnight.png") 

		oMenuBar = new qmenubar(win) {
			subMenu1 = addmenu("File")
			subMenu1 {

				oAction1_1 = new qAction(win) {
					setShortcut(new QKeySequence(""))
					setbtnimage(self,"")
					settext("Exit")
					setclickevent(Method(:closeapp))
				}
				addaction(oAction1_1)
			}
		}
		win.SetMenuBar(oMenuBar)
 
		ChessTableWidget = new tablewidget(win) {
			move(27,45)
			resize(755,529)
			setstylesheet("color:;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setRowCount(28056)
			setColumnCount(7)
			setHorizontalHeaderItem(1, new QTableWidgetItem("WhiteKingFile"))
			setHorizontalHeaderItem(2, new QTableWidgetItem("WhiteKingRank"))
			setHorizontalHeaderItem(3, new QTableWidgetItem("WhiteRockFile"))
			setHorizontalHeaderItem(4, new QTableWidgetItem("WhiteRockRank"))
			setHorizontalHeaderItem(5, new QTableWidgetItem("BlackKingFile"))
			setHorizontalHeaderItem(6, new QTableWidgetItem("BlackKingRank"))
			setHorizontalHeaderItem(7, new QTableWidgetItem("GameResult"))
			setSelectionBehavior(QAbstractItemView_SelectRows)
			setAlternatingRowColors(True)
			setcellActivatedEvent("")
			setcellChangedEvent("")
			setcellClickedEvent("")
			setcellDoubleClickedEvent("")
			setcellEnteredEvent("")
			setcellPressedEvent("")
			setcurrentCellChangedEvent("")
			setcurrentItemChangedEvent(Method(:NewSelection))
			setitemActivatedEvent("")
			setitemChangedEvent("")
			setitemClickedEvent("")
			setitemDoubleClickedEvent("")
			setitemEnteredEvent("")
			setitemPressedEvent("")
			setitemSelectionChangedEvent("")
			
		}
		Statusbar1 = new statusbar(win) {
			move(244,379)
			resize(96,12)
			setstylesheet("color:;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			ShowMessage("Ready!",0)
			win.setStatusBar(Statusbar1)
		}
		Board = new label(win) {
			move(859,71)
			resize(400,400)
			setstylesheet("color:black;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setPixMap(New qPixMap("images/chessboard.png"))
			
		}
		WK = new label(win) {
			move(883,483)
			resize(50,50)
			setstylesheet("color:black;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setPixMap(New qPixMap("images/WK.png"))
			
		}
		WR = new label(win) {
			move(1003,483)
			resize(50,50)
			setstylesheet("color:black;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setPixMap(New qPixMap("images/WR.png"))
			
		}
		BK = new label(win) {
			move(1126,483)
			resize(50,50)
			setstylesheet("color:black;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setPixMap(New qPixMap("images/BK.png"))
			
		}
	}

# End of the Generated Source Code File...