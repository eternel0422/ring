# Form/Window View - Generated Source Code File 
# Generated by the Ring 1.16 Form Designer
# Date : 03/03/2022
# Time : 19:32:42

Load "stdlibcore.ring"
Load "guilib.ring"

import System.GUI

if IsMainSourceFile() { 
	new App {
		StyleFusion()
		new Rosetta_FormDesigner_TaskView { win.show() } 
		exec()
	}
}

class Rosetta_FormDesigner_TaskView from WindowsViewParent
	win = new MainWindow() { 
		move(20,20)
		resize(1251,758)
		setWindowTitle("Ring Tasks in Rosetta Code")
		setstylesheet("background-color:;") 
		ListWidget1 = new listwidget(win) {
			move(8,75)
			resize(410,643)
			setstylesheet("color:black;background-color:white;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setcurrentItemChangedEvent("")
			setcurrentRowChangedEvent("")
			setcurrentTextChangedEvent("")
			setitemActivatedEvent("")
			setitemChangedEvent("")
			setitemClickedEvent(Method(:ListWidgetClicked))
			setitemDoubleClickedEvent("")
			setitemEnteredEvent("")
			setitemPressedEvent("")
			setitemSelectionChangedEvent("")
			
		}
		Label1 = new label(win) {
			move(6,26)
			resize(87,27)
			setstylesheet("color:black;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("72,12,-1,5,75,0,0,0,0,0,Bold")
			setfont(oFont)
			oFont.delete()
			setText("Task Name")
			setAlignment(Qt_AlignLeft |  Qt_AlignVCenter)
		}
		LineEdit1 = new lineedit(win) {
			move(159,24)
			resize(686,30)
			setstylesheet("color:#0000ff;background-color:white;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("72,12,-1,5,75,0,0,0,0,0,Bold")
			setfont(oFont)
			oFont.delete()
			setText("")
			setTextChangedEvent("")
			setcursorPositionChangedEvent("")
			seteditingFinishedEvent("")
			setreturnPressedEvent("")
			setselectionChangedEvent("")
			settextEditedEvent("")
			
		}
		Label3 = new label(win) {
			move(862,23)
			resize(84,28)
			setstylesheet("color:black;background-color:;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("72,12,-1,5,75,0,0,0,0,0,Bold")
			setfont(oFont)
			oFont.delete()
			setText("Code Text")
			setAlignment(Qt_AlignLeft |  Qt_AlignVCenter)
		}
		Button1 = new pushbutton(win) {
			move(969,22)
			resize(90,29)
			setstylesheet("color:black;background-color:#aaffff;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("72,12,-1,5,75,0,0,0,0,0,Bold")
			setfont(oFont)
			oFont.delete()
			setText("Display")
			setClickEvent(Method(:DisplayCode))
			setBtnImage(Button1,"")
			
		}
		TextEdit1 = new textedit(win) {
			move(432,73)
			resize(793,649)
			setstylesheet("color:black;background-color:#ffffff;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("Arial")
			setfont(oFont)
			oFont.delete()
			setText("")
			setcopyAvailableEvent("")
			setcurrentCharFormatChangedEvent("")
			setcursorPositionChangedEvent("")
			setredoAvailableEvent("")
			setselectionChangedEvent("")
			settextChangedEvent("")
			setundoAvailableEvent("")
			
		}
		LineEdit2 = new lineedit(win) {
			move(100,25)
			resize(51,30)
			setstylesheet("color:blue;background-color:000000;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("72,12,-1,5,75,0,0,0,0,0,Bold")
			setfont(oFont)
			oFont.delete()
			setText("")
			setTextChangedEvent("")
			setcursorPositionChangedEvent("")
			seteditingFinishedEvent("")
			setreturnPressedEvent("")
			setselectionChangedEvent("")
			settextEditedEvent("")
			
		}
		Button2 = new pushbutton(win) {
			move(1085,24)
			resize(64,24)
			setstylesheet("color:#000000;background-color:#aaff7f;")
			oFont = new qfont("",0,0,0)
			oFont.fromstring("72,12,-1,5,75,0,0,0,0,0,Bold")
			setfont(oFont)
			oFont.delete()
			setText("Run")
			setClickEvent(Method(:Run))
			setBtnImage(Button2,"")
			
		}
	}

# End of the Generated Source Code File...
